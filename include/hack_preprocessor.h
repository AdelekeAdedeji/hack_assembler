// Copyright (c) 2026 Author. All Rights Reserved.

#include "utils.h"

#define LINE_LENGTH 18

typedef char BYTE;

typedef enum {
    TOKEN_INVALID,
    TOKEN_NUMBER,
    TOKEN_VARIABLE
} TokenType;

typedef struct hack_preprocessor_attributes {
  BYTE* cache;
  char** fomatted_cache;
  int32_t pc, var_address_allocator;
  char** bin_file_cache;
  int32_t bin_cache_line_count;
  
} HppAttr;

extern HppAttr* attributes;
extern HashTable *pd_symbols_table; 
extern HashTable *label_symbol_table;
extern HashTable* variable_symbol_table;
extern HashTable* mnemonic_comp_lookup_table;
extern HashTable* mnemonic_dest_lookup_table;
extern HashTable* mnemonic_jump_lookup_table;


void read_file(const char*);

void write_file(const char* out_path);

void generate_binary_file(const char* path);

void create_program_tables();

long get_file_size(FILE *);

void parse_hack_c_instruction(const char *raw_line); 
    
void scan_source_label_pass();

void scan_source_pass();

void format_hpp_cache();

void free_hpp_attributes();

void free_all_memory();

TokenType classify_variable(const char *str);
