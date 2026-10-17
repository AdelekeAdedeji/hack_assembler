#include "./utils.h"


HashTable* create_hash_table(void) {
    HashTable* hash_table = calloc(1, sizeof(HashTable));

    if (hash_table == NULL) {
        
        fprintf(stderr, "Error: memory allocation failed for hash table\n");
        
        exit(EXIT_FAILURE);
    }
    
    hash_table->mask = SPARSE_TABLE_SIZE - 1;

    hash_table->sparse_table_count = 0;

    hash_table->sparse_table_capacity = SPARSE_TABLE_SIZE;
  
    hash_table->dense_table_length = 0;

    hash_table->dense_table_capacity = DENSE_TABLE_SIZE;

    hash_table->sparse_table = calloc(hash_table->sparse_table_capacity, sizeof(int32_t));
    
    if (hash_table->sparse_table == NULL) {
        
        fprintf(stderr, "Error: memory allocation sparse table failed\n");
        
        free(hash_table);
        
        exit(EXIT_FAILURE);
    }
  
    memset(hash_table->sparse_table, -1, sizeof(int32_t) * hash_table->sparse_table_capacity);
    
    hash_table->dense_table = calloc(hash_table->dense_table_capacity, sizeof(EntryObject));

    if (hash_table->dense_table == NULL) {
        
        fprintf(stderr, "Error: memory allocation failed for dense table\n");
        
        free(hash_table->sparse_table);
        
        free(hash_table);
        
        exit(EXIT_FAILURE);
    }

    return hash_table;
}

uint32_t _fnv_1a_32(uint8_t* data) {
    uint32_t i = 0x00, fnv_prime = 0x01000193, hash_val = 0x811c9dc5;

    while ( *(data + i) ) {
        hash_val ^= *(data + i);
        hash_val *= fnv_prime;
        i++;
    }

    return hash_val;
}

void table_insert(const char* string_key, uint32_t value, HashTable* hash_table) {
    
    if ( ( hash_table->sparse_table_count * 4) > (hash_table->sparse_table_capacity * 3) ) {
        _resize_sparse_table(hash_table);
    }  

    int32_t dense_slot;
    
    uint32_t free_index, raw_hash = _fnv_1a_32((uint8_t*) string_key), sparse_slot = raw_hash & hash_table->mask;
    
    while ((dense_slot = hash_table->sparse_table[sparse_slot]) != -1) {
        
        if (strcmp((hash_table->dense_table + dense_slot)->key.string, string_key) == 0) {
            
            if ((hash_table->dense_table + dense_slot)->active == true) {
                
                (hash_table->dense_table + dense_slot)->value = value; 
            
                return;
            }

            else {
                
                (hash_table->dense_table + dense_slot)->active = true;
                
                (hash_table->dense_table + dense_slot)->value = value;
                
                hash_table->active_count++;
                
                return;
            }
        }
        
        sparse_slot = (sparse_slot + 1) & hash_table->mask;

    } 

    if ((free_index = hash_table->dense_table_length) >= hash_table->dense_table_capacity) {
        _resize_dense_table(hash_table, &free_index);

        sparse_slot = raw_hash & hash_table->mask;

        while ((dense_slot = hash_table->sparse_table[sparse_slot]) != -1) {

            sparse_slot = (sparse_slot + 1) & hash_table->mask;
        }
    }

    (hash_table->dense_table + free_index)->active = true;
        
    (hash_table->dense_table + free_index)->value = value;
        
    (hash_table->dense_table + free_index)->hash_value = raw_hash;

    char* owned_key = strdup(string_key);

    if (owned_key == NULL) { 
        fprintf(stderr, "Error: key allocation failed\n"); 
        exit(EXIT_FAILURE); 
    }

    (hash_table->dense_table + free_index)->key.string = owned_key;       

    hash_table->sparse_table[sparse_slot] = free_index;
   
    hash_table->active_count++;

    hash_table->dense_table_length++;

    hash_table->sparse_table_count++;

    return;
}

int32_t table_get(const char* string_key, HashTable* hash_table) {
    
    uint32_t raw_hash = _fnv_1a_32((uint8_t*) string_key), sparse_slot = raw_hash & hash_table->mask, dense_slot;
        
    while ( (dense_slot = hash_table->sparse_table[sparse_slot]) != -1) {
    
        if (strcmp((hash_table->dense_table + dense_slot)->key.string, string_key) == 0) {
        
            if ((hash_table->dense_table + dense_slot)->active == true) {

                return (hash_table->dense_table + dense_slot)->value;
            }
        }

        sparse_slot = (sparse_slot + 1) & hash_table->mask;

    }

    fprintf(stderr, "Key Error: \"%s\" doesn't exist in hash table\n", string_key);

    return -1;
}


void table_delete(const char* string_key, HashTable* hash_table) {

    uint32_t raw_hash = _fnv_1a_32((uint8_t*) string_key), sparse_slot = raw_hash & hash_table->mask, dense_slot;

    while ( (dense_slot = hash_table->sparse_table[sparse_slot]) != -1 ) {

        if (strcmp((hash_table->dense_table + dense_slot)->key.string, string_key) == 0) {

            if ( (hash_table->dense_table + dense_slot)->active == true ) {
               
                (hash_table->dense_table + dense_slot)->active = false;

                hash_table->active_count--;

                return;
            }

            else {
                
                fprintf(stderr, "Key Error: \"%s\" doesn't exist in hash table\n", string_key);

                return;

            }
        }

        sparse_slot = (sparse_slot + 1) & hash_table->mask;

    }
    
    fprintf(stderr, "Key Error: \"%s\" doesn't exist in hash table\n", string_key);

    return;
}


void _resize_dense_table(HashTable* hash_table, uint32_t* index) {

    if (hash_table->active_count < hash_table->dense_table_capacity) {
    
        _compact_hash_table(hash_table, index);
        
        return;
    }

    size_t new_length = hash_table->dense_table_capacity + (0.5 * hash_table->dense_table_capacity);

    EntryObject* new_dense_table = realloc(hash_table->dense_table, sizeof(EntryObject) * new_length);

    if (new_dense_table == NULL) {
        
        fprintf(stderr, "Error: re-size of dense_table failed\n");
        
        free(hash_table->dense_table);
        
        free(hash_table->sparse_table);
        
        free(hash_table);

        exit(EXIT_FAILURE);
    }

    hash_table->dense_table_capacity = new_length;

    hash_table->dense_table = new_dense_table;
}


void _resize_sparse_table(HashTable* hash_table) {

    int32_t new_sparse_capacity = hash_table->sparse_table_capacity * 2, *new_sparse_table = calloc(new_sparse_capacity, sizeof(int32_t));

    uint32_t new_mask = new_sparse_capacity - 1, new_sparse_slot;

    if (new_sparse_table == NULL) {
        
        fprintf(stderr, "Error: re-size of sparse table failed\n");
        
        free(hash_table->sparse_table);
        
        free(hash_table->dense_table);
        
        free(hash_table);

        exit(EXIT_FAILURE);
    }

    memset(new_sparse_table, -1, sizeof(int32_t) * new_sparse_capacity);

    for (int i = 0; i < hash_table->dense_table_length; i++) {

        if ((hash_table->dense_table + i)->active == true) {
            
            new_sparse_slot = (hash_table->dense_table + i)->hash_value & new_mask;
            
            while (*(new_sparse_table + new_sparse_slot) != -1) {

                new_sparse_slot = (new_sparse_slot + 1) & new_mask;
            };

            *(new_sparse_table + new_sparse_slot) = i;
        }
    }

    free(hash_table->sparse_table);
    
    hash_table->mask = new_mask;

    hash_table->sparse_table_capacity = new_sparse_capacity;

    hash_table->sparse_table = new_sparse_table;
}

void _compact_hash_table(HashTable* hash_table, uint32_t* index) {
    uint32_t sparse_slot;

    memset(hash_table->sparse_table, -1, sizeof(int32_t) * hash_table->sparse_table_capacity);
    
    for (int i = 0, j = 0; i < hash_table->dense_table_length; i++) {

        if ((hash_table->dense_table + i)->active == true) {

            *(hash_table->dense_table + j) = *(hash_table->dense_table + i);

            sparse_slot = (hash_table->dense_table + i)->hash_value & hash_table->mask;

            while (*(hash_table->sparse_table + sparse_slot) != -1) {
                sparse_slot = (sparse_slot + 1) & hash_table->mask;
            }

            *(hash_table->sparse_table + sparse_slot) = j;

            j++;
        }
    }

    *index = hash_table->active_count;

    hash_table->dense_table_length = hash_table->active_count;

    hash_table->sparse_table_count = hash_table->active_count;

}

void print_hash_table(HashTable* hash_table) {

    printf("{");

    int _ = 1;

    for (int i = 0; i < hash_table->dense_table_length; i++) {
        
        if ((hash_table->dense_table + i)->active == true){

            if (!_) {
                printf(", ");

            }
            
            printf("\"%s\": %u", (hash_table->dense_table + i)->key.string, (hash_table->dense_table + i)->value);

            _ = 0;
        }
    }

    printf("}\n");
}

void free_hash_table(HashTable* hash_table) {
    for (uint32_t i = 0; i < hash_table->dense_table_length; i++) {
        free((char*)(hash_table->dense_table + i)->key.string);
    }

    free(hash_table->sparse_table);

    free(hash_table->dense_table);
    free(hash_table);
}

