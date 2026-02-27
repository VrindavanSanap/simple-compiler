/*
 * sclc: yet another systems programming language
 * Initial compiler implementation in C
 *
 * Scull Project Copyright (C) 2026, Samyak Bambole <bambole@duck.com>
 * Licensed under the GNU/GPL Version 3
 */

#include "backend/backend.h"
#include "cstate.h"
#include "ds/dynamic_array.h"
#include "fstate.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "utils.h"

#include <time.h>

int main(int argc, char *argv[]) {
  // Initialize compiler state
  cstate cst = {0};
  cstate_init(&cst, (u32)argc, argv);

  backend backend;
  backend_init(&backend, &cst);

  clock_t start, end;
  double time_taken;
  start = clock();

  for (u64 i = 0; i < cst.files.count; i++) {
    fstate *fst;
    dynamic_array_get(&cst.files, i, &fst);

    // Lexing
    lexer_tokenize(fst->code_buffer, fst->code_buffer_len, &fst->tokens,
                   cst.include_dir);

    // Lexing debug statements
    if (cst.options.verbose) {
      scu_pdebug("Lexing Debug Statements for %s:\n", fst->filepath);
      lexer_print_tokens(&fst->tokens);
    }

    // Parsing
    parser_parse_program(&fst->tokens, &fst->program_ast);

    // Parsing debug statements
    if (cst.options.verbose) {
      scu_pdebug("Parsing Debug Statements for %s:\n", fst->filepath);
      print_ast(&fst->program_ast);
    }

    // Semantic Analysis
    check_semantics(&fst->program_ast.instrs, &fst->variables, &fst->functions);

    // Semantic Debug Statement
    if (cst.options.verbose)
      scu_pdebug("Semantic Analysis Complete for %s\n", fst->filepath);

    // Initiate backend compilation
    backend_compile(&backend, &cst, fst);

    // Codegen Debug Statements
    if (cst.options.verbose)
      scu_pdebug("Codegen Complete for %s\n", fst->filepath);

    if (cst.options.verbose)
      scu_psuccess("COMPILED %s\n", fst->filepath);
  }

  if (!(cst.options.compile_only))
    backend.link(&cst);

  backend_free(&backend);

  if (cst.options.emit_llvm || cst.options.emit_asm) {
    cstate_free(&cst);
    return 0;
  }

  end = clock();
  time_taken = (double)(end - start) / CLOCKS_PER_SEC;

  if (cst.options.verbose)
    scu_psuccess("  LINKED %s - %.2fs total time taken\n", cst.output_filepath,
                 time_taken);

  cstate_free(&cst);

  return 0;
}
