/*
 * types: types for the SCULL Langauge
 *
 * Scull Project Copyright (C) 2026, Samyak Bambole <bambole@duck.com>
 * Licensed under the GNU/GPL Version 3
 */

#ifndef TYPES_H
#define TYPES_H

#include "core/common.h"

#include "frontend/token.h"

/*
 * @enum type: represents data types.
 */
typedef enum type {
  TYPE_INVALID = 0,

  TYPE_U8,
  TYPE_U16,
  TYPE_U32,
  TYPE_U64,
  TYPE_U128,

  TYPE_I8,
  TYPE_I16,
  TYPE_I32,
  TYPE_I64,
  TYPE_I128,

  TYPE_CHAR,

  TYPE_STRING,

  TYPE_POINTER,
  TYPE_VOID
} type;

/*
 * @brief: get the size of a data type in bytes.
 *
 * @param t: data type.
 *
 * @return: size in bytes of the type t.
 */
u32 type_size(type t);

/*
 * @brief: convert a type enumeration to its string representation.
 */
const char *type_to_str(type type);

/*
 * @brief: deduce data type from type specifier token
 *
 * @param specifier: type specifier token kind (others skipped automatically,
 * returns TYPE_INVALID)
 *
 * @return mapped data type from the specifier token kind
 */
type type_from_specifier_token(token_kind specifier);

#endif // !TYPES_H
