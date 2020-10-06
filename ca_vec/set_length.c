/*
    Copyright (C) 2020 Fredrik Johansson

    This file is part of Calcium.

    Calcium is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#include "ca_vec.h"

void
_ca_vec_fit_length(ca_vec_t vec, slong len, ca_ctx_t ctx)
{
    if (len > vec->alloc)
    {
        slong i;

        if (len < 2 * vec->alloc)
            len = 2 * vec->alloc;

        vec->coeffs = flint_realloc(vec->coeffs, len * sizeof(ca_struct));

        for (i = vec->alloc; i < len; i++)
            ca_init(ca_vec_entry(vec, i), ctx);

        vec->alloc = len;
    }
}

void
ca_vec_set_length(ca_vec_t vec, slong len, ca_ctx_t ctx)
{
    slong i;

    if (vec->length > len)
    {
        for (i = len; i < vec->length; i++)
            ca_zero(ca_vec_entry(vec, i), ctx);
    }
    else if (vec->length < len)
    {
        _ca_vec_fit_length(vec, len, ctx);

        for (i = vec->length; i < len; i++)
            ca_zero(ca_vec_entry(vec, i), ctx);
    }

    vec->length = len;
}