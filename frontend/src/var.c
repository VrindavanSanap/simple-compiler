/*
 * var: variable, symbol and types for the SCULL Langauge
 *
 * Scull Project Copyright (C) 2026, Samyak Bambole <bambole@duck.com>
 * Licensed under the GNU/GPL Version 3
 */

#include "frontend/var.h"

#include "core/common.h"
#include "core/utils.h"
#include "frontend/token.h"

u32 type_size(type t) {
  switch (t) {
  case TYPE_INVALID:
  case TYPE_VOID:
    return 0;

  case TYPE_U8:
  case TYPE_I8:
  case TYPE_CHAR:
    return 1;

  case TYPE_U16:
  case TYPE_I16:
    return 2;

  case TYPE_U32:
  case TYPE_I32:
    return 4;

  case TYPE_U64:
  case TYPE_I64:
  case TYPE_STRING:
  case TYPE_POINTER:
    return 8;

  case TYPE_U128:
  case TYPE_I128:
    return 16;
  }
}

type get_var_type(ht *variables, variable *var_to_find) {
  if (!variables || !var_to_find || !var_to_find->name)
    return -1;

  variable *var = ht_search(variables, var_to_find->name);

  if (!var) {
    scu_perror("Use of undeclared variable: %s [line %u]\n", var_to_find->name,
               var_to_find->line);
    return -1;
  }

  return var->type;
}

u64 get_var_stack_offset(ht *variables, variable *var_to_find) {
  if (!variables || !var_to_find || !var_to_find->name)
    return -1;

  variable *var = ht_search(variables, var_to_find->name);

  if (!var) {
    scu_perror("Use of undeclared variable: %s [line %u]\n", var_to_find->name,
               var_to_find->line);
    return -1;
  }

  return var->stack_offset;
}

const char *type_to_str(type type) {
  switch (type) {
  case TYPE_INVALID:
    return "invalid";

  case TYPE_U8:
    return "u8";
  case TYPE_U16:
    return "u16";
  case TYPE_U32:
    return "u32";
  case TYPE_U64:
    return "u64";
  case TYPE_U128:
    return "u128";

  case TYPE_I8:
    return "i8";
  case TYPE_I16:
    return "i16";
  case TYPE_I32:
    return "i32";
  case TYPE_I64:
    return "i64";
  case TYPE_I128:
    return "i128";

  case TYPE_CHAR:
    return "char";

  case TYPE_STRING:
    return "string";

  case TYPE_POINTER:
    return "ptr";

  case TYPE_VOID:
    return "void";
  }
}

type type_from_specifier_token(token_kind specifier) {
  switch (specifier) {
  case TOKEN_TYPE_U8:
    return TYPE_U8;
  case TOKEN_TYPE_U16:
    return TYPE_U16;
  case TOKEN_TYPE_U32:
    return TYPE_U32;
  case TOKEN_TYPE_U64:
    return TYPE_U64;
  case TOKEN_TYPE_U128:
    return TYPE_U128;

  case TOKEN_TYPE_I8:
    return TYPE_I8;
  case TOKEN_TYPE_I16:
    return TYPE_I16;
  case TOKEN_TYPE_I32:
    return TYPE_I32;
  case TOKEN_TYPE_I64:
    return TYPE_I64;
  case TOKEN_TYPE_I128:
    return TYPE_I128;

  case TOKEN_TYPE_CHAR:
    return TYPE_CHAR;

  default:
    return TYPE_INVALID;
  }
}
