/*
    Copyright (C) 2020 Fredrik Johansson

    This file is part of Calcium.

    Calcium is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "ca_poly.h"

static void
_ca_addmul(ca_t x, ca_t tmp, const ca_t a, const ca_t b, ca_ctx_t ctx)
{
    ca_mul(tmp, a, b, ctx);
    ca_add(x, x, tmp, ctx);
}

void
_ca_poly_mullow(ca_ptr res,
    ca_srcptr poly1, slong len1,
    ca_srcptr poly2, slong len2, slong n, ca_ctx_t ctx)
{
    len1 = FLINT_MIN(len1, n);
    len2 = FLINT_MIN(len2, n);

    if (n == 1)
    {
        ca_mul(res, poly1, poly2, ctx);
    }
    else if (poly1 == poly2 && len1 == len2)
    {
        slong i, start, stop;
        ca_t t;

        ca_init(t, ctx);

        ca_sqr(res, poly1, ctx);
        ca_mul(res + 1, poly1, poly1 + 1, ctx);
        ca_mul_ui(res + 1, res + 1, 2, ctx);

        for (i = 2; i < FLINT_MIN(n, 2 * len1 - 3); i++)
        {
            start = FLINT_MAX(0, i - len1 + 1);
            stop = FLINT_MIN(len1 - 1, (i + 1) / 2 - 1);

            ca_dot(res + i, NULL, 0, poly1 + start, 1,
                poly1 + i - start, -1, stop - start + 1, ctx);
            ca_mul_ui(res + i, res + i, 2, ctx);
            if (i % 2 == 0 && i / 2 < len1)
                _ca_addmul(res + i, t, poly1 + i / 2, poly1 + i / 2, ctx);
        }

        if (len1 > 2 && n >= 2 * len1 - 2)
        {
            ca_mul(res + 2 * len1 - 3, poly1 + len1 - 1, poly1 + len1 - 2, ctx);
            ca_mul_ui(res + 2 * len1 - 3, res + 2 * len1 - 3, 2, ctx);
        }

        if (n >= 2 * len1 - 1)
            ca_sqr(res + 2 * len1 - 2, poly1 + len1 - 1, ctx);

        ca_clear(t, ctx);
    }
    else if (len1 == 1)
    {
        _ca_vec_scalar_mul_ca(res, poly2, n, poly1, ctx);
    }
    else if (len2 == 1)
    {
        _ca_vec_scalar_mul_ca(res, poly1, n, poly2, ctx);
    }
    else
    {
        slong i, top1, top2;

        ca_mul(res, poly1, poly2, ctx);

        for (i = 1; i < n; i++)
        {
            top1 = FLINT_MIN(len1 - 1, i);
            top2 = FLINT_MIN(len2 - 1, i);

            ca_dot(res + i, NULL, 0, poly1 + i - top2, 1,
                poly2 + top2, -1, top1 + top2 - i + 1, ctx);
        }
    }
}

void
ca_poly_mullow(ca_poly_t res, const ca_poly_t poly1,
                                            const ca_poly_t poly2,
                                                slong n, ca_ctx_t ctx)
{
    slong len_out;

    if (poly1->length == 0 || poly2->length == 0 || n == 0)
    {
        ca_poly_zero(res, ctx);
        return;
    }

    len_out = poly1->length + poly2->length - 1;
    n = FLINT_MIN(n, len_out);

    if (res == poly1 || res == poly2)
    {
        ca_poly_t t;
        ca_poly_init2(t, n, ctx);
        _ca_poly_mullow(t->coeffs, poly1->coeffs, poly1->length,
                                    poly2->coeffs, poly2->length, n, ctx);
        ca_poly_swap(res, t, ctx);
        ca_poly_clear(t, ctx);
    }
    else
    {
        ca_poly_fit_length(res, n, ctx);
        _ca_poly_mullow(res->coeffs, poly1->coeffs, poly1->length,
                                    poly2->coeffs, poly2->length, n, ctx);
    }

    _ca_poly_set_length(res, n, ctx);
    _ca_poly_normalise(res, ctx);
}