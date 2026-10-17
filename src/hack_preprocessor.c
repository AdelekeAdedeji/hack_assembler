// Copyright (c) 2026 Author. All Rights Reserved.

#include "hack_preprocessor.h"

int32_t string_length;

HppAttr *attributes = NULL;

HashTable *pd_symbols_table = NULL, *label_symbol_table = NULL,
          *variable_symbol_table = NULL, *mnemonic_comp_lookup_table = NULL,
          *mnemonic_dest_lookup_table = NULL,
          *mnemonic_jump_lookup_table = NULL;

char bin_form[BINARY_SIZE + 1];

void binary_equivalent(uint16_t number) {

  for (int i = BINARY_SIZE - 1; i >= 0; i--) {
    uint16_t bit = (number >> i) & 1;
    bin_form[(BINARY_SIZE - 1) - i] = bit + '0';
  }

  bin_form[BINARY_SIZE] = '\0';
  return;
}

long get_file_size(FILE *stream) {
  if (fseek(stream, 0L, SEEK_END) != 0) {
    fprintf(stderr, "Error getting to end of file\n");
  }

  int size = ftell(stream);

  if (size < 0) {
    return -1;
  }

  return size;
}

void read_file(const char *file_path) {

  FILE *asm_fp = fopen(file_path, "rb");

  if (asm_fp == NULL) {
    fprintf(stderr, "Error in open operation, can't open %s\n", file_path);
    exit(EXIT_FAILURE);
  }

  attributes = calloc(1, sizeof(HppAttr));

  if (attributes == NULL) {
    fprintf(stderr, "Error allocating memory\n");
    exit(EXIT_FAILURE);
  }

  long size = get_file_size(asm_fp);

  if (size < 0) {
    fprintf(stderr, "Error getting size of file: %s\n", file_path);
    fclose(asm_fp);
    exit(EXIT_FAILURE);
  }

  attributes->cache = malloc(size + 1);

  if (attributes->cache == NULL) {
    fprintf(stderr, "memory allocation falied!!!\n");
    exit(EXIT_FAILURE);
  }

  fseek(asm_fp, 0L, SEEK_SET);

  int size_read = fread(attributes->cache, sizeof(BYTE), size, asm_fp);

  if (size_read != size) {
    fprintf(stderr, "error: read wasn't complete!!!\n");
  }

  *(attributes->cache + size_read) = '\0';

  attributes->pc = 0, attributes->var_address_allocator = 16;

  create_program_tables();

  fclose(asm_fp);
}

void create_program_tables() {
  pd_symbols_table = create_hash_table();
  label_symbol_table = create_hash_table();
  variable_symbol_table = create_hash_table();
  mnemonic_comp_lookup_table = create_hash_table();
  mnemonic_dest_lookup_table = create_hash_table();
  mnemonic_jump_lookup_table = create_hash_table();

  table_insert("R0", 0, pd_symbols_table);
  table_insert("R1", 1, pd_symbols_table);
  table_insert("R2", 2, pd_symbols_table);
  table_insert("R3", 3, pd_symbols_table);
  table_insert("R4", 4, pd_symbols_table);
  table_insert("R5", 5, pd_symbols_table);
  table_insert("R6", 6, pd_symbols_table);
  table_insert("R7", 7, pd_symbols_table);
  table_insert("R8", 8, pd_symbols_table);
  table_insert("R9", 9, pd_symbols_table);
  table_insert("R10", 10, pd_symbols_table);
  table_insert("R11", 11, pd_symbols_table);
  table_insert("R12", 12, pd_symbols_table);
  table_insert("R13", 13, pd_symbols_table);
  table_insert("R14", 14, pd_symbols_table);
  table_insert("R15", 15, pd_symbols_table);
  table_insert("SP", 0, pd_symbols_table);
  table_insert("LCL", 1, pd_symbols_table);
  table_insert("ARG", 2, pd_symbols_table);
  table_insert("THIS", 3, pd_symbols_table);
  table_insert("THAT", 4, pd_symbols_table);
  table_insert("SCREEN", 16384, pd_symbols_table);
  table_insert("KBD", 24576, pd_symbols_table);

  table_insert("0", 42, mnemonic_comp_lookup_table);
  table_insert("1", 63, mnemonic_comp_lookup_table);
  table_insert("-1", 58, mnemonic_comp_lookup_table);
  table_insert("D", 12, mnemonic_comp_lookup_table);
  table_insert("A", 48, mnemonic_comp_lookup_table);
  table_insert("M", 112, mnemonic_comp_lookup_table);
  table_insert("!D", 13, mnemonic_comp_lookup_table);
  table_insert("!A", 49, mnemonic_comp_lookup_table);
  table_insert("!M", 113, mnemonic_comp_lookup_table);
  table_insert("-D", 15, mnemonic_comp_lookup_table);
  table_insert("-A", 51, mnemonic_comp_lookup_table);
  table_insert("-M", 115, mnemonic_comp_lookup_table);
  table_insert("D+1", 31, mnemonic_comp_lookup_table);
  table_insert("1+D", 31, mnemonic_comp_lookup_table);
  table_insert("A+1", 55, mnemonic_comp_lookup_table);
  table_insert("1+A", 55, mnemonic_comp_lookup_table);
  table_insert("M+1", 119, mnemonic_comp_lookup_table);
  table_insert("1+M", 119, mnemonic_comp_lookup_table);
  table_insert("D-1", 14, mnemonic_comp_lookup_table);
  table_insert("A-1", 50, mnemonic_comp_lookup_table);
  table_insert("M-1", 114, mnemonic_comp_lookup_table);
  table_insert("D+A", 2, mnemonic_comp_lookup_table);
  table_insert("A+D", 2, mnemonic_comp_lookup_table);
  table_insert("D+M", 66, mnemonic_comp_lookup_table);
  table_insert("M+D", 66, mnemonic_comp_lookup_table);
  table_insert("D-A", 19, mnemonic_comp_lookup_table);
  table_insert("D-M", 83, mnemonic_comp_lookup_table);
  table_insert("A-D", 7, mnemonic_comp_lookup_table);
  table_insert("M-D", 71, mnemonic_comp_lookup_table);
  table_insert("D&A", 0, mnemonic_comp_lookup_table);
  table_insert("A&D", 0, mnemonic_comp_lookup_table);
  table_insert("D&M", 64, mnemonic_comp_lookup_table);
  table_insert("M&D", 64, mnemonic_comp_lookup_table);
  table_insert("D|A", 21, mnemonic_comp_lookup_table);
  table_insert("A|D", 21, mnemonic_comp_lookup_table);
  table_insert("D|M", 85, mnemonic_comp_lookup_table);
  table_insert("M|D", 85, mnemonic_comp_lookup_table);

  table_insert("", 0,
               mnemonic_dest_lookup_table); // "" represents null with opcode
                                            // 000 i.e. destination is null
  table_insert("M", 1, mnemonic_dest_lookup_table);
  table_insert("D", 2, mnemonic_dest_lookup_table);
  table_insert("MD", 3, mnemonic_dest_lookup_table);
  table_insert("DM", 3, mnemonic_dest_lookup_table);
  table_insert("A", 4, mnemonic_dest_lookup_table);
  table_insert("AM", 5, mnemonic_dest_lookup_table);
  table_insert("MA", 5, mnemonic_dest_lookup_table);
  table_insert("AD", 6, mnemonic_dest_lookup_table);
  table_insert("DA", 6, mnemonic_dest_lookup_table);
  table_insert("AMD", 7, mnemonic_dest_lookup_table);
  table_insert("ADM", 7, mnemonic_dest_lookup_table);
  table_insert("MAD", 7, mnemonic_dest_lookup_table);
  table_insert("MDA", 7, mnemonic_dest_lookup_table);
  table_insert("DAM", 7, mnemonic_dest_lookup_table);
  table_insert("DMA", 7, mnemonic_dest_lookup_table);

  table_insert("", 0,
               mnemonic_jump_lookup_table); // "" represents null with opcode
                                            // 000 i.e. jump is null
  table_insert("JGT", 1, mnemonic_jump_lookup_table);
  table_insert("JEQ", 2, mnemonic_jump_lookup_table);
  table_insert("JGE", 3, mnemonic_jump_lookup_table);
  table_insert("JLT", 4, mnemonic_jump_lookup_table);
  table_insert("JNE", 5, mnemonic_jump_lookup_table);
  table_insert("JLE", 6, mnemonic_jump_lookup_table);
  table_insert("JMP", 7, mnemonic_jump_lookup_table);
}

void format_hpp_cache() {
  char *save_ptr;
  int line_count = 0;
  attributes->fomatted_cache = NULL;

  // get the first raw line
  char *token = strtok_r(attributes->cache, "\r\n", &save_ptr);

  while (token != NULL) {

    // we want to strip inline comments so we look for "//" anywhere inside
    // the current line

    char *comment_start = strstr(token, "//");

    if (comment_start != NULL) {
      // overwrite the first '/' with a null terminator, cutting the
      // string short and erasing the comment
      *comment_start = '\0';
    }

    // skips leading whitespaces i.e. advance the pointer past any spaces or
    // tabs
    while (*token == ' ' || *token == '\t') {
      token++;
    }

    // skips whitespaces in between line
    int32_t write_index = 0;

    for (int32_t read_index = 0; *(token + read_index) != '\0'; read_index++) {
      if (*(token + read_index) != ' ') {
        *(token + write_index) = *(token + read_index);
        write_index++;
      }
    }

    *(token + write_index) = '\0';

    // filtering empty lines: If the line was empty, was just spaces, or was
    // a full-line comment, *token is now '\0' courtesy of the steps
    // performed above.
    if (*token == '\0') {
      token = strtok_r(NULL, "\r\n", &save_ptr);
      continue; // skip allocation and grab the next line
    }

    // allocate and store valid lines
    attributes->fomatted_cache =
        realloc(attributes->fomatted_cache, (line_count + 1) * sizeof(char *));
    if (attributes->fomatted_cache == NULL) {

      fprintf(stderr, "Allocation failed\n");

      free_hpp_attributes();

      exit(EXIT_FAILURE);
    }

    // save our cleaned pointer back into the cache (i.e. attributes->cache)
    *(attributes->fomatted_cache + line_count) = token;

    line_count++;

    // move to next line in the buffer
    token = strtok_r(NULL, "\r\n", &save_ptr);
  }

  // allocate one final pointer slot to mark the end of the array i.e. point
  // final slot to NULL

  attributes->fomatted_cache =
      realloc(attributes->fomatted_cache, (line_count + 1) * sizeof(char *));
  if (attributes->fomatted_cache == NULL) {
    fprintf(stderr, "Final slot allocation failed\n");

    free_hpp_attributes();

    exit(EXIT_FAILURE);
  }

  *(attributes->fomatted_cache + line_count) = NULL;

  attributes->bin_cache_line_count = line_count + 1;
}

void scan_source_label_pass() {
  char **formatted_cache = attributes->fomatted_cache;

  while (*formatted_cache) {

    string_length = strlen(*(formatted_cache));

    // check label

    char label[string_length - 1];
    if (**(formatted_cache) == '(') {
      if (*(*(formatted_cache) + string_length - 1) == ')' &&
          string_length > 2) {
        for (int i = 1; i < string_length - 1; i++) {
          label[i - 1] = *(*(formatted_cache) + i);
        }

        label[string_length - 2] = '\0';

        if (table_get(label, label_symbol_table) == -1) {
          table_insert(label, attributes->pc, label_symbol_table);
          attributes->bin_cache_line_count--;
        }
      }

      else {
        fprintf(stderr, "Error: Unknown syntax %s\n", *(formatted_cache));

        free_all_memory();

        exit(EXIT_FAILURE);
      }
    }

    else {
      attributes->pc++;
    }

    formatted_cache++;
  }
}

void parse_hack_c_instruction(const char *raw_line) {
  // 1. Calculate the exact length of the incoming line
  size_t len = strlen(raw_line);

  // 2. Create a stack array sized EXACTLY to line length + 1 byte for '\0'
  char line_copy[len + 1];

  // 3. Copy the string content into our temporary stack array
  strcpy(line_copy, raw_line);

  // Pointers that will anchor to the start of our sub-strings
  char *dest = NULL;
  char *comp = NULL;
  char *jump = NULL;

  // 4. LOCATE OPTIONAL DEST SEPARATOR '='
  char *equal_sign = strchr(line_copy, '=');
  if (equal_sign != NULL) {
    // Everything up to the '=' is the destination register(s)
    dest = line_copy;

    // Overwrite '=' with a string stop-sign ('\0')
    *equal_sign = '\0';

    // Computation logic begins immediately after the split
    comp = equal_sign + 1;
  } else {
    // If there's no '=', dest is empty, comp starts at line beginning
    dest = "";
    comp = line_copy;
  }

  // 5. LOCATE OPTIONAL JUMP SEPARATOR ';'
  char *semicolon = strchr(comp, ';');
  if (semicolon != NULL) {
    // Jump directive starts immediately after the ';'
    jump = semicolon + 1;

    // if (!jump) jump = "null";
    // Overwrite ';' with a string stop-sign ('\0') to slice comp short
    *semicolon = '\0';
  } else {
    // If there's no ';', jump fields are blank
    jump = "";
  }

  int comp_value = table_get(comp, mnemonic_comp_lookup_table),
      dest_value = table_get(dest, mnemonic_dest_lookup_table),
      jump_value = table_get(jump, mnemonic_jump_lookup_table);

  if (comp_value == -1) {
    fprintf(stderr, "Error: comp Unknown c instruction %s\n", comp);
    free_all_memory();
    exit(EXIT_FAILURE);
  }

  if (dest_value == -1) {
    fprintf(stderr, "Error: dest Unknown c instruction %s\n", dest);
    free_all_memory();
    exit(EXIT_FAILURE);
  }

  if (jump_value == -1) {
    fprintf(stderr, "Error: jump Unknown c instruction %s\n", jump);
    free_all_memory();
    exit(EXIT_FAILURE);
  }

  int c_instruction =
      57344 | (comp_value << 6) | (dest_value << 3) | (jump_value);

  binary_equivalent(c_instruction);
}

void scan_source_pass() {

  attributes->bin_file_cache =
      calloc(attributes->bin_cache_line_count, sizeof(char *));

  if (attributes->bin_file_cache == NULL) {

    fprintf(stderr, "Error allocating memory\n");

    free_all_memory();

    exit(EXIT_FAILURE);
  }

  char **formatted_cache = attributes->fomatted_cache,
       **bin_file_cache = attributes->bin_file_cache;

  for (int i = 0; i < attributes->bin_cache_line_count - 1; i++) {

    *(bin_file_cache + i) = calloc(LINE_LENGTH, sizeof(char));

    if (*(bin_file_cache + i) == NULL) {

      fprintf(stderr, "Error allocating memory\n");

      free_all_memory();

      exit(EXIT_FAILURE);
    }

    *(*(bin_file_cache + i) + LINE_LENGTH - 1) = '\0';

    *(*(bin_file_cache + i) + LINE_LENGTH - 2) = '\n';
  }

  *(bin_file_cache + (attributes->bin_cache_line_count - 1)) = NULL;

  while (*formatted_cache) {

    string_length = strlen(*(formatted_cache));

    char variable[string_length];

    if (**formatted_cache == '@') {

      // printf("In @ block\n");

      for (int i = 1; i < string_length; i++) {
        variable[i - 1] = *(*formatted_cache + i);
      }

      variable[string_length - 1] = '\0';

      if (table_get(variable, label_symbol_table) == -1) {

        if (table_get(variable, pd_symbols_table) == -1) {

          if (table_get(variable, variable_symbol_table) == -1) {

            if (classify_variable(variable) == TOKEN_VARIABLE) {

              table_insert(variable, attributes->var_address_allocator,
                           variable_symbol_table);

              binary_equivalent(table_get(variable, variable_symbol_table));

              strncpy(*bin_file_cache, bin_form, 16);

              attributes->var_address_allocator++;
            }

            else if (classify_variable(variable) == TOKEN_NUMBER) {
              char *end_ptr;

              long value = strtol(variable, &end_ptr, 10);

              if (value < 0 || value > 32767) {

                fprintf(stderr,
                        "Error: A-instruction must be between "
                        "0 and 32767, got %ld\n",
                        value);

                free_all_memory();

                exit(EXIT_FAILURE);
              }

              binary_equivalent(value);

              strncpy(*bin_file_cache, bin_form, 16);
            }

            else if (classify_variable(variable) == TOKEN_INVALID) {
              fprintf(stderr, "Error: Bad variable name convention %s\n",
                      variable);
              free_all_memory();
              exit(EXIT_FAILURE);
            }
          }

          else {
            binary_equivalent(table_get(variable, variable_symbol_table));
            strncpy(*bin_file_cache, bin_form, 16);
          }
        }

        else {
          binary_equivalent(table_get(variable, pd_symbols_table));

          strncpy(*bin_file_cache, bin_form, 16);
        }
      }

      else {
        binary_equivalent(table_get(variable, label_symbol_table));

        strncpy(*bin_file_cache, bin_form, 16);
      }
    }

    else if (**formatted_cache == '(') {
      formatted_cache++;
      continue;
    }

    else {

      // printf("In C block\n");

      parse_hack_c_instruction(*formatted_cache);

      strncpy(*bin_file_cache, bin_form, 16);
    }

    formatted_cache++, bin_file_cache++;
  }
}

TokenType classify_variable(const char *str) {
  if (str == NULL || *str == '\0') {
    return TOKEN_INVALID;
  }

  // 1. CHECK IF IT IS A PURE NUMBER (A-instruction address context)
  char *endptr;
  strtol(str, &endptr, 10);
  if (*endptr == '\0') {
    return TOKEN_NUMBER;
  }

  // 2. STAGE ACCORDING TO HACK SYNTAX RULES
  // Rule A: A variable name cannot start with a digit!
  if (isdigit((unsigned char)*str)) {
    return TOKEN_INVALID; // Catches "160a", "0_loop", etc.
  }

  // Rule B: Loop and check every remaining symbol explicitly
  // Allowing only alphanumeric characters, underscores '_', and colons ':'
  while (*str != '\0') {
    if (!isalnum((unsigned char)*str) && *str != '_' && *str != ':' &&
        *str != '$' && *str != '.') {
      return TOKEN_INVALID; // Catches forbidden characters like '$', '.',
                            // or '-'
    }
    str++;
  }

  return TOKEN_VARIABLE;
}

void write_file(const char *out_path) {
  FILE *out = fopen(out_path, "w");
  if (out == NULL) {
    perror("fopen");
    free_all_memory();
    exit(EXIT_FAILURE);
  }

  for (char **line = attributes->bin_file_cache; *line; line++) {
    if (fputs(*line, out) == EOF) {
      perror("fputs");
      fclose(out);
      free_all_memory();
      exit(EXIT_FAILURE);
    }
  }

  if (fclose(out) != 0) { // fclose flushes; this can fail (disk full)
    perror("fclose");
    free_all_memory();
    exit(EXIT_FAILURE);
  }
}

void generate_binary_file(const char *path) {
  char out_path[512];
  int n = snprintf(out_path, sizeof(out_path), "%s", path);
  if (n < 0 || (size_t)n >= sizeof(out_path)) {
    fprintf(stderr, "Error: path too long\n");
    exit(EXIT_FAILURE); // nothing allocated yet
  }

  char *slash = strrchr(out_path, '/');
  char *dot = strrchr(out_path, '.');
  if (dot != NULL && (slash == NULL || dot > slash))
    *dot = '\0';

  strncat(out_path, ".hack", sizeof(out_path) - strlen(out_path) - 1);

  read_file(path);

  /*
  int i = 0;

  while (1) {
      if (*(attributes->cache + i) == '\0'){
          break;
      }
      printf("%c", *(attributes->cache + i));
      i++;
  }
  */

  format_hpp_cache();

  scan_source_label_pass();

  scan_source_pass();

  /*
  char** ptr = attributes->fomatted_cache;

  int count1 = 0;

  while (*ptr){
      printf("%s\n", *ptr);
      ptr++, count1++;
  }

  printf("formatted_cache is %d lines", count1);

  printf("\n");
  printf("-------------------------");
  printf("\n\n");

  char** ptor = attributes->bin_file_cache;

  int count2 = 0;

  while (*ptor){
      printf("%s", *ptor);
      ptor++, count2++;
  }

  printf("bin_cache is %d lines\n", count2);
  */
  // print_hash_table(pd_symbols_table);
  // print_hash_table(label_symbol_table);
  // print_hash_table(variable_symbol_table);
  // print_hash_table(mnemonic_comp_lookup_table);
  // print_hash_table(mnemonic_dest_lookup_table);
  // print_hash_table(mnemonic_jump_lookup_table);

  write_file(out_path);

  free_all_memory();
}

void free_hpp_attributes() {

  // if the attributes is already null, return
  if (attributes == NULL) {
    fprintf(stderr, "Error: attributes struct is already NULL or freed\n");
    return;
  }

  if (attributes->bin_file_cache != NULL) {

    for (int i = 0; i < attributes->bin_cache_line_count - 1; i++) {

      if (*(attributes->bin_file_cache + i) != NULL) {

        free(*(attributes->bin_file_cache + i));

        *(attributes->bin_file_cache + i) = NULL;
      }
    }
  }

  if (attributes->bin_file_cache != NULL) {
    free(attributes->bin_file_cache);
    attributes->bin_file_cache = NULL;
  }

  // free attributes->fomatted_cache, we are not freeing the string items
  // (i.e. (char*)s') inside it
  if (attributes->fomatted_cache != NULL) {
    free(attributes->fomatted_cache);
    attributes->fomatted_cache = NULL;
  }

  // free attributes->cache
  if (attributes->cache != NULL) {
    free(attributes->cache);
    attributes->cache = NULL;
  }

  // free attributes
  free(attributes);
  attributes = NULL;
}

void free_all_memory() {
  free_hpp_attributes();
  free_hash_table(pd_symbols_table);
  free_hash_table(label_symbol_table);
  free_hash_table(variable_symbol_table);
  free_hash_table(mnemonic_comp_lookup_table);
  free_hash_table(mnemonic_dest_lookup_table);
  free_hash_table(mnemonic_jump_lookup_table);
}
