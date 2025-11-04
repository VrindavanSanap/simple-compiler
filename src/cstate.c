#include "cstate.h"
#include "ds/dynamic_array.h"
#include "fstate.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

cstate *cstate_create_from_args(int argc, char *argv[]) {
  cstate *s = scu_checked_malloc(sizeof(cstate));

  if (argc <= 1) {
    /*
     * Default output.
     * Should list out each option and a short description.
     * Take care of wrapping after ~80 characters.
     */
    printf("Simple Compiler - Just as the name suggests\n");
    printf("Usage: sclc [OPTIONS] <filename(s)>\n\n");
    printf("OPTIONS:\n");
    printf("--verbose      OR -v \t Print progress messages for various "
           "stages.\n");
    printf("--output       OR -o \t Specify output binary filename.\n");
    printf("--include_dir  OR -i \t Specify include directory path.\n");
    exit(1);
  }

  int i = 1;
  dynamic_array filenames;
  dynamic_array_init(&filenames, sizeof(char *));
  s->output_filepath = NULL;
  s->include_dir = NULL;
  s->error_count = 0;

  while (i < argc) {
    char *arg = argv[i];

    if (strcmp(arg, "--verbose") == 0 || strcmp(arg, "-v") == 0) {
      s->options.verbose = true;
      i++;
      continue;
    }

    if (strcmp(arg, "--output") == 0 || strcmp(arg, "-o") == 0) {
      if (i + 1 >= argc) {
        scu_perror(&s->error_count, "Missing filename after %s\n", arg);
        exit(1);
      }

      if (s->output_filepath != NULL) {
        scu_perror(&s->error_count, "Output specified more than once: %s\n",
                   argv[i + 1]);
        exit(1);
      }

      s->output_filepath = strdup(argv[i + 1]);
      s->options.output = true;
      i += 2;
      continue;
    }

    if (strcmp(arg, "--include_dir") == 0 || strcmp(arg, "-i") == 0) {
      if (i + 1 >= argc) {
        scu_perror(&s->error_count, "Missing directory path after %s\n", arg);
        exit(1);
      }

      if (s->include_dir != NULL) {
        scu_perror(&s->error_count,
                   "Include directory specified more than once: %s\n",
                   argv[i + 1]);
        exit(1);
      }

      struct stat st;
      if (stat(argv[i + 1], &st) != 0) {
        scu_perror(&s->error_count, "Include directory does not exist: %s\n",
                   argv[i + 1]);
        exit(1);
      }

      if (!S_ISDIR(st.st_mode)) {
        scu_perror(&s->error_count, "Path is not a directory: %s\n",
                   argv[i + 1]);
        exit(1);
      }

      s->include_dir = strdup(argv[i + 1]);
      s->options.include_dir_specified = true;
      i += 2;
      continue;
    }

    if (arg[0] != '-') {
      char *filename_copy = strdup(arg);
      dynamic_array_append(&filenames, &filename_copy);
      i++;
      continue;
    }

    scu_perror(&s->error_count, "Unknown option: %s\n", arg);
    exit(1);
  }

  if (s->include_dir == NULL)
    s->include_dir = strdup(".");

  if (filenames.count == 0) {
    scu_perror(&s->error_count, "Missing input filename\n");
    exit(1);
  }

  if (s->output_filepath == NULL) {
    char *first_filename;
    dynamic_array_get(&filenames, 0, &first_filename);
    s->output_filepath = scu_extract_name(first_filename);
    if (!s->output_filepath) {
      scu_perror(&s->error_count, "Failed to extract filename.\n");
      exit(1);
    }
  }

  dynamic_array_init(&s->files, sizeof(fstate *));

  for (size_t i = 0; i < filenames.count; i++) {
    char *filepath;
    dynamic_array_get(&filenames, i, &filepath);

    fstate *fst = create_new_fstate(filepath);
    if (fst == NULL) {
      scu_perror(&s->error_count, "Failed to create fstate for: %s\n",
                 filepath);
      exit(1);
    }

    dynamic_array_append(&s->files, &fst);
    free(filepath);
  }

  dynamic_array_free(&filenames);

  return s;
}

void cstate_free(cstate *s) {
  if (s == NULL)
    return;

  if (s->include_dir != NULL)
    free(s->include_dir);

  if (s->output_filepath != NULL)
    free(s->output_filepath);

  for (size_t i = 0; i < s->files.count; i++) {
    fstate *fst;
    dynamic_array_get(&s->files, i, &fst);
    free_fstate(fst);
  }

  dynamic_array_free(&s->files);

  free(s);
}
