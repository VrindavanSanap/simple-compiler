/*
 * var: variable, symbol and types for the SCULL Langauge
 *
 * Scull Project Copyright (C) 2026, Samyak Bambole <bambole@duck.com>
 * Licensed under the GNU/GPL Version 3
 */

#ifndef VAR_H
#define VAR_H

#include "core/common.h"
#include "core/ds/ht.h"
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
 * @struct variable: represents a variable.
 */
typedef struct variable {
  type type;
  char *name;
  u64 line;
  u64 stack_offset;

  bool is_array;
  u64 dimensions;
  u64 *dimension_sizes;
} variable;

/*
 * @brief: get the size of a data type in bytes.
 *
 * @param t: data type.
 *
 * @return: size in bytes of the type t.
 */
u32 type_size(type t);

/*
 * @brief: check if a certain variable exists in a dynamic_array of variables.
 *
 * @param variables: pointer to hash table of variables.
 * @param var_to_find: pointer to a variable struct which we intend to find in
 * the dynamic_array.
 *
 * @return: int
 */
u64 get_var_stack_offset(ht *variables, variable *var_to_find);

/*
 * @brief: check for a variable's type by its name / identifier and line data.
 *
 * @param variables: pointer to hash table of variable.
 * @param var_to_find: pointer to a variable struct which we intend to find in
 * the dynamic_array.
 *
 * @return: data type of the variable (enumeration)
 */
type get_var_type(ht *variables, variable *var_to_find);

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

#endif // !VAR_H
