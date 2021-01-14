/*
    Copyright (C) 2021 Fredrik Johansson

    This file is part of Calcium.

    Calcium is free software: you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License (LGPL) as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.  See <http://www.gnu.org/licenses/>.
*/

#ifndef FEXPR_H
#define FEXPR_H

#ifdef FEXPR_INLINES_C
#define FEXPR_INLINE
#else
#define FEXPR_INLINE static __inline__
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "flint/flint.h"
#include "flint/fmpz.h"
#include "flint/fmpq.h"
#include "calcium.h"
#include "fmpz_mpoly_q.h"

#define FEXPR_TYPE_SMALL_INT     UWORD(0)
#define FEXPR_TYPE_SMALL_SYMBOL  UWORD(1)
#define FEXPR_TYPE_SMALL_STRING  UWORD(2)
#define FEXPR_TYPE_BIG_INT_POS   UWORD(3)
#define FEXPR_TYPE_BIG_INT_NEG   UWORD(4)
#define FEXPR_TYPE_BIG_SYMBOL    UWORD(5)
#define FEXPR_TYPE_BIG_STRING    UWORD(6)
#define FEXPR_TYPE_CALL0         UWORD(7)
#define FEXPR_TYPE_CALL1         UWORD(8)
#define FEXPR_TYPE_CALL2         UWORD(9)
#define FEXPR_TYPE_CALL3         UWORD(10)
#define FEXPR_TYPE_CALL4         UWORD(11)
#define FEXPR_TYPE_CALLN         UWORD(12)

#define FEXPR_TYPE_BITS 4
#define FEXPR_TYPE_MASK ((UWORD(1) << FEXPR_TYPE_BITS) - 1)

#define FEXPR_COEFF_MAX ((WORD(1) << (FLINT_BITS - FEXPR_TYPE_BITS - 1)) - 1)
#define FEXPR_COEFF_MIN (-FEXPR_COEFF_MAX)

#define FEXPR_TYPE(head)  ((head) & FEXPR_TYPE_MASK)
#define FEXPR_SIZE(head)  ((slong) ((FEXPR_TYPE(head) <= FEXPR_TYPE_SMALL_STRING) ? 1 : (head) >> FEXPR_TYPE_BITS))

#define FEXPR_HEADER_SIZE        WORD(1)
#define FEXPR_SMALL_SYMBOL_LEN   ((FLINT_BITS / 8) - 1)

/* Builtin symbols */

/* Must be listed in alphabetical order with corresponding entry
   in fexpr_builtins (the alphabetical order is just so that
   we can do binary search on the strings). */
typedef enum
{
    FEXPR_Abs,
    FEXPR_Acos,
    FEXPR_Acosh,
    FEXPR_Add,
    FEXPR_AiryAi,
    FEXPR_AiryBi,
    FEXPR_Arg,
    FEXPR_Asin,
    FEXPR_Asinh,
    FEXPR_Atan,
    FEXPR_Atanh,
    FEXPR_BesselI,
    FEXPR_BesselJ,
    FEXPR_BesselK,
    FEXPR_BesselY,
    FEXPR_Ceil,
    FEXPR_Conjugate,
    FEXPR_Cos,
    FEXPR_Cosh,
    FEXPR_Div,
    FEXPR_Erf,
    FEXPR_Erfc,
    FEXPR_Erfi,
    FEXPR_Euler,
    FEXPR_Exp,
    FEXPR_Floor,
    FEXPR_Gamma,
    FEXPR_HurwitzZeta,
    FEXPR_I,
    FEXPR_Im,
    FEXPR_JacobiTheta,
    FEXPR_LambertW,
    FEXPR_Log,
    FEXPR_LogGamma,
    FEXPR_Mul,
    FEXPR_Neg,
    FEXPR_Pi,
    FEXPR_Pos,
    FEXPR_Pow,
    FEXPR_Psi,
    FEXPR_Re,
    FEXPR_RiemannZeta,
    FEXPR_Root,
    FEXPR_RootOfUnity,
    FEXPR_Sign,
    FEXPR_Sin,
    FEXPR_Sinh,
    FEXPR_Sqrt,
    FEXPR_Sub,
    FEXPR_Tan,
    FEXPR_Tanh,
    FEXPR_BUILTIN_LENGTH
}
fexpr_builtin_symbol;

typedef struct
{
    fexpr_builtin_symbol symbol;
    const char * string;
}
fexpr_symbol_info;

extern const fexpr_symbol_info fexpr_builtins[FEXPR_BUILTIN_LENGTH];

slong fexpr_get_builtin_str(const char * s);

#define FEXPR_SYMBOL_Pos  (FEXPR_TYPE_SMALL_SYMBOL | (FEXPR_Pos << 16))
#define FEXPR_SYMBOL_Neg  (FEXPR_TYPE_SMALL_SYMBOL | (FEXPR_Neg << 16))
#define FEXPR_SYMBOL_Add  (FEXPR_TYPE_SMALL_SYMBOL | (FEXPR_Add << 16))
#define FEXPR_SYMBOL_Sub  (FEXPR_TYPE_SMALL_SYMBOL | (FEXPR_Sub << 16))
#define FEXPR_SYMBOL_Mul  (FEXPR_TYPE_SMALL_SYMBOL | (FEXPR_Mul << 16))
#define FEXPR_SYMBOL_Div  (FEXPR_TYPE_SMALL_SYMBOL | (FEXPR_Div << 16))
#define FEXPR_SYMBOL_Pow  (FEXPR_TYPE_SMALL_SYMBOL | (FEXPR_Pow << 16))

#define FEXPR_SYMBOL_I   (FEXPR_TYPE_SMALL_SYMBOL | (FEXPR_I << 16))
#define FEXPR_SYMBOL_Pi  (FEXPR_TYPE_SMALL_SYMBOL | (FEXPR_Pi << 16))
#define FEXPR_SYMBOL_Euler   (FEXPR_TYPE_SMALL_SYMBOL | (FEXPR_Euler << 16))
#define FEXPR_SYMBOL_Re  (FEXPR_TYPE_SMALL_SYMBOL | (FEXPR_Re << 16))
#define FEXPR_SYMBOL_Im  (FEXPR_TYPE_SMALL_SYMBOL | (FEXPR_Im << 16))
#define FEXPR_SYMBOL_Abs  (FEXPR_TYPE_SMALL_SYMBOL | (FEXPR_Abs << 16))
#define FEXPR_SYMBOL_Exp  (FEXPR_TYPE_SMALL_SYMBOL | (FEXPR_Exp << 16))
#define FEXPR_SYMBOL_Log  (FEXPR_TYPE_SMALL_SYMBOL | (FEXPR_Log << 16))
#define FEXPR_SYMBOL_Cos  (FEXPR_TYPE_SMALL_SYMBOL | (FEXPR_Cos << 16))
#define FEXPR_SYMBOL_Sin  (FEXPR_TYPE_SMALL_SYMBOL | (FEXPR_Sin << 16))
#define FEXPR_SYMBOL_Tan  (FEXPR_TYPE_SMALL_SYMBOL | (FEXPR_Tan << 16))


typedef struct
{
    ulong * data;
    slong alloc;
}
fexpr_struct;

typedef fexpr_struct fexpr_t[1];
typedef fexpr_struct * fexpr_ptr;
typedef const fexpr_struct * fexpr_srcptr;

typedef struct
{
    fexpr_struct * entries;
    slong length;
    slong alloc;
}
fexpr_vec_struct;

typedef fexpr_vec_struct fexpr_vec_t[1];

#define fexpr_vec_entry(vec, i) ((vec)->entries + (i))

FEXPR_INLINE void
fexpr_init(fexpr_t expr)
{
    expr->data = flint_malloc(sizeof(ulong));
    expr->data[0] = 0;
    expr->alloc = 1;
}

FEXPR_INLINE void
fexpr_clear(fexpr_t expr)
{
    flint_free(expr->data);
}

FEXPR_INLINE fexpr_ptr
_fexpr_vec_init(slong len)
{
    slong i;
    fexpr_ptr vec = flint_malloc(sizeof(fexpr_struct) * len);
    for (i = 0; i < len; i++)
        fexpr_init(vec + i);
    return vec;
}

FEXPR_INLINE void
_fexpr_vec_clear(fexpr_ptr vec, slong len)
{
    slong i;
    for (i = 0; i < len; i++)
        fexpr_clear(vec + i);
    flint_free(vec);
}

FEXPR_INLINE void
fexpr_fit_size(fexpr_t expr, slong size)
{
    if (expr->alloc < size)
    {
        size = FLINT_MAX(size, 2 * expr->alloc);
        expr->data = flint_realloc(expr->data, size * sizeof(ulong));
        expr->alloc = size;
    }
}

FEXPR_INLINE slong
_fexpr_size(const ulong * expr)
{
    ulong head = expr[0];
    return FEXPR_SIZE(head);
}

FEXPR_INLINE slong
fexpr_size(const fexpr_t expr)
{
    return _fexpr_size(expr->data);
}

FEXPR_INLINE void
fexpr_set(fexpr_t res, const fexpr_t expr)
{
    if (res != expr)
    {
        slong size = fexpr_size(expr);
        fexpr_fit_size(res, size);
        flint_mpn_copyi(res->data, expr->data, size);
    }
}

FEXPR_INLINE void
fexpr_swap(fexpr_t a, fexpr_t b)
{
    fexpr_struct tmp = *a;
    *a = *b;
    *b = tmp;
}


FEXPR_INLINE int
_mpn_equal(mp_srcptr a, mp_srcptr b, slong len)
{
    slong i;

    for (i = 0; i < len; i++)
        if (a[i] != b[i])
            return 0;

    return 1;
}

FEXPR_INLINE int
fexpr_equal(const fexpr_t a, const fexpr_t b)
{
    ulong ha, hb;
    slong sa, sb;

    ha = a->data[0];
    hb = b->data[0];

    if (ha != hb)
        return 0;

    sa = FEXPR_SIZE(ha);
    sb = FEXPR_SIZE(hb);

    if (sa != sb)
        return 0;

    return _mpn_equal(a->data + 1, b->data + 1, sa - 1);
}

int fexpr_cmp_fast(const fexpr_t a, const fexpr_t b);

FEXPR_INLINE void
_fexpr_vec_sort_fast(fexpr_ptr vec, slong len)
{
    qsort(vec, len, sizeof(fexpr_struct), (int(*)(const void*,const void*)) fexpr_cmp_fast);
}

FEXPR_INLINE int
_fexpr_is_integer(const ulong * expr)
{
    ulong type = FEXPR_TYPE(expr[0]);
    return (type == FEXPR_TYPE_SMALL_INT) || (type == FEXPR_TYPE_BIG_INT_POS) || (type == FEXPR_TYPE_BIG_INT_NEG);
}

FEXPR_INLINE int
fexpr_is_integer(const fexpr_t expr)
{
    return _fexpr_is_integer(expr->data);
}

FEXPR_INLINE int
_fexpr_is_symbol(const ulong * expr)
{
    ulong type = FEXPR_TYPE(expr[0]);
    return (type == FEXPR_TYPE_SMALL_SYMBOL) || (type == FEXPR_TYPE_BIG_SYMBOL);
}

FEXPR_INLINE int
fexpr_is_symbol(const fexpr_t expr)
{
    return _fexpr_is_symbol(expr->data);
}

FEXPR_INLINE int
_fexpr_is_string(const ulong * expr)
{
    ulong type = FEXPR_TYPE(expr[0]);
    return (type == FEXPR_TYPE_SMALL_STRING) || (type == FEXPR_TYPE_BIG_STRING);
}

FEXPR_INLINE int
fexpr_is_string(const fexpr_t expr)
{
    return _fexpr_is_string(expr->data);
}

FEXPR_INLINE int
_fexpr_is_atom(const ulong * expr)
{
    return FEXPR_TYPE(expr[0]) <= FEXPR_TYPE_BIG_STRING;
}

FEXPR_INLINE int
fexpr_is_atom(const fexpr_t expr)
{
    return _fexpr_is_atom(expr->data);
}

FEXPR_INLINE void
fexpr_zero(fexpr_t res)
{
    res->data[0] = 0;
}

FEXPR_INLINE int
fexpr_is_zero(const fexpr_t expr)
{
    return expr->data[0] == 0;
}

void fexpr_set_si(fexpr_t res, slong c);
void fexpr_set_ui(fexpr_t res, ulong c);
void fexpr_set_fmpz(fexpr_t res, const fmpz_t c);
int fexpr_get_fmpz(fmpz_t c, const fexpr_t x);
void fexpr_set_fmpq(fexpr_t res, const fmpq_t x);

void fexpr_set_symbol_str(fexpr_t res, const char * s);
char * fexpr_get_symbol_str(const fexpr_t expr);

FEXPR_INLINE slong
fexpr_nargs(const fexpr_t expr)
{
    ulong type = FEXPR_TYPE(expr->data[0]);

    if (FEXPR_TYPE_CALL0 <= type && type <= FEXPR_TYPE_CALL4)
    {
        return type - FEXPR_TYPE_CALL0;
    }
    else if (type == FEXPR_TYPE_CALLN)
    {
        return expr->data[1];
    }
    else
    {
        return -1;
    }
}

void fexpr_func(fexpr_t res, const fexpr_t expr);
void fexpr_view_func(fexpr_t res, const fexpr_t expr);

void fexpr_arg(fexpr_t res, const fexpr_t expr, slong i);
void fexpr_view_arg(fexpr_t res, const fexpr_t expr, slong i);

FEXPR_INLINE void
fexpr_view_next(fexpr_t view)
{
    view->data += fexpr_size(view);
}

/* todo: handle aliasing! */
void fexpr_call0(fexpr_t res, const fexpr_t f);
void fexpr_call1(fexpr_t res, const fexpr_t f, const fexpr_t x1);
void fexpr_call2(fexpr_t res, const fexpr_t f, const fexpr_t x1, const fexpr_t x2);
void fexpr_call3(fexpr_t res, const fexpr_t f, const fexpr_t x1, const fexpr_t x2, const fexpr_t x3);
void fexpr_call4(fexpr_t res, const fexpr_t f, const fexpr_t x1, const fexpr_t x2, const fexpr_t x3, const fexpr_t x4);

void fexpr_call_vec(fexpr_t res, const fexpr_t f, fexpr_srcptr args, slong len);

void fexpr_write(calcium_stream_t stream, const fexpr_t expr);
void fexpr_print(const fexpr_t expr);
char * fexpr_get_str(const fexpr_t expr);

FEXPR_INLINE void
fexpr_neg(fexpr_t res, const fexpr_t a)
{
    fexpr_t tmp;
    ulong tmp_head = FEXPR_SYMBOL_Neg;
    tmp->data = &tmp_head;
    tmp->alloc = 1;
    fexpr_call1(res, tmp, a);
}

FEXPR_INLINE void
fexpr_add(fexpr_t res, const fexpr_t a, const fexpr_t b)
{
    fexpr_t tmp;
    ulong tmp_head = FEXPR_SYMBOL_Add;
    tmp->data = &tmp_head;
    tmp->alloc = 1;
    fexpr_call2(res, tmp, a, b);
}

FEXPR_INLINE void
fexpr_sub(fexpr_t res, const fexpr_t a, const fexpr_t b)
{
    fexpr_t tmp;
    ulong tmp_head = FEXPR_SYMBOL_Sub;
    tmp->data = &tmp_head;
    tmp->alloc = 1;
    fexpr_call2(res, tmp, a, b);
}

FEXPR_INLINE void
fexpr_mul(fexpr_t res, const fexpr_t a, const fexpr_t b)
{
    fexpr_t tmp;
    ulong tmp_head = FEXPR_SYMBOL_Mul;
    tmp->data = &tmp_head;
    tmp->alloc = 1;
    fexpr_call2(res, tmp, a, b);
}

FEXPR_INLINE void
fexpr_div(fexpr_t res, const fexpr_t a, const fexpr_t b)
{
    fexpr_t tmp;
    ulong tmp_head = FEXPR_SYMBOL_Div;
    tmp->data = &tmp_head;
    tmp->alloc = 1;
    fexpr_call2(res, tmp, a, b);
}

FEXPR_INLINE void
fexpr_pow(fexpr_t res, const fexpr_t a, const fexpr_t b)
{
    fexpr_t tmp;
    ulong tmp_head = FEXPR_SYMBOL_Pow;
    tmp->data = &tmp_head;
    tmp->alloc = 1;
    fexpr_call2(res, tmp, a, b);
}

int fexpr_is_arithmetic_operation(const fexpr_t expr);

void fexpr_arithmetic_nodes(fexpr_vec_t nodes, const fexpr_t expr);

/* todo: document/change */
FEXPR_INLINE int
fexpr_is_Pow(const fexpr_t expr)
{
    fexpr_t op;
    ulong op_head;

    if (fexpr_is_atom(expr))
        return 0;

    fexpr_view_func(op, expr);
    op_head = op->data[0];

    return op_head == FEXPR_SYMBOL_Pow;
}

int fexpr_get_fmpz_mpoly_q(fmpz_mpoly_q_t res, const fexpr_t expr, const fexpr_vec_t vars, const fmpz_mpoly_ctx_t ctx);

void fexpr_set_fmpz_mpoly(fexpr_t res, const fmpz_mpoly_t poly, const fexpr_vec_t vars, const fmpz_mpoly_ctx_t ctx);
void fexpr_set_fmpz_mpoly_q(fexpr_t res, const fmpz_mpoly_q_t frac, const fexpr_vec_t vars, const fmpz_mpoly_ctx_t ctx);

int fexpr_expanded_normal_form(fexpr_t res, const fexpr_t expr, ulong flags);

/* Vectors */

FEXPR_INLINE void
fexpr_vec_init(fexpr_vec_t vec)
{
    vec->entries = NULL;
    vec->length = 0;
    vec->alloc = 0;
}

FEXPR_INLINE void
fexpr_vec_print(const fexpr_vec_t F)
{
    slong i;

    flint_printf("[");
    for (i = 0; i < F->length; i++)
    {
        fexpr_print(F->entries + i);
        if (i < F->length - 1)
            flint_printf(", ");
    }
    flint_printf("]");
}

FEXPR_INLINE void
fexpr_vec_swap(fexpr_vec_t x, fexpr_vec_t y)
{
    fexpr_vec_t tmp;
    *tmp = *x;
    *x = *y;
    *y = *tmp;
}

FEXPR_INLINE void
fexpr_vec_fit_length(fexpr_vec_t vec, slong len)
{
    if (len > vec->alloc)
    {
        slong i;

        if (len < 2 * vec->alloc)
            len = 2 * vec->alloc;

        vec->entries = flint_realloc(vec->entries, len * sizeof(fexpr_struct));

        for (i = vec->alloc; i < len; i++)
            fexpr_init(vec->entries + i);

        vec->alloc = len;
    }
}

FEXPR_INLINE void
fexpr_vec_clear(fexpr_vec_t vec)
{
    slong i;

    for (i = 0; i < vec->alloc; i++)
        fexpr_clear(vec->entries + i);

    flint_free(vec->entries);
}

FEXPR_INLINE void
fexpr_vec_set(fexpr_vec_t dest, const fexpr_vec_t src)
{
    if (dest != src)
    {
        slong i;

        fexpr_vec_fit_length(dest, src->length);

        for (i = 0; i < src->length; i++)
            fexpr_set(dest->entries + i, src->entries + i);

        dest->length = src->length;
    }
}

FEXPR_INLINE void
fexpr_vec_append(fexpr_vec_t vec, const fexpr_t f)
{
    fexpr_vec_fit_length(vec, vec->length + 1);
    fexpr_set(vec->entries + vec->length, f);
    vec->length++;
}

FEXPR_INLINE slong
fexpr_vec_insert_unique(fexpr_vec_t vec, const fexpr_t f)
{
    slong i;

    for (i = 0; i < vec->length; i++)
    {
        if (fexpr_equal(vec->entries + i, f))
            return i;
    }

    fexpr_vec_append(vec, f);
    return vec->length - 1;
}

FEXPR_INLINE void
fexpr_vec_set_length(fexpr_vec_t vec, slong len)
{
    slong i;

    if (len > vec->length)
    {
        fexpr_vec_fit_length(vec, len);
        for (i = vec->length; i < len; i++)
            fexpr_zero(vec->entries + i);
    }
    else if (len < vec->length)
    {
        for (i = len; i < vec->length; i++)
           fexpr_zero(vec->entries + i);
    }

    vec->length = len;
}


#ifdef __cplusplus
}
#endif

#endif

