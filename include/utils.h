#pragma once

#include "stdhd.h"

// The sparse table size should be a power of 2 as we are using bitwise & in
// place of modulo %

#define SPARSE_TABLE_SIZE 4096

#define DENSE_TABLE_SIZE 1024

typedef union custom_type {

    const char *string;

    int integer;

    float decimal;

} custom_type;

typedef struct entry {

    bool active;

    custom_type key;

    uint32_t hash_value, value;

} EntryObject;

typedef struct ht {

    size_t sparse_table_count, sparse_table_capacity, dense_table_length,
        dense_table_capacity, active_count;

    int32_t *sparse_table;

    uint32_t mask;

    EntryObject *dense_table;

} HashTable;

uint32_t _fnv_1a_32(uint8_t *data);

void _resize_sparse_table(HashTable *hash_table);

void _compact_hash_table(HashTable *hash_table, uint32_t *index);

void _resize_dense_table(HashTable *hash_table, uint32_t *index);

void print_hash_table(HashTable *hash_table);

void free_hash_table(HashTable *hash_table);

HashTable *create_hash_table(void);

void table_insert(const char *string_key, uint32_t value,
                  HashTable *hash_table);

void table_delete(const char *string_key, HashTable *hash_table);

int32_t table_get(const char *string_key, HashTable *hash_table);
