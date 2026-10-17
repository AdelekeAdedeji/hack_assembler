/*

 * Copyright (c) 2026 Adedeji Adeleke. All Rights Reserved.

 * Hack assembler written to translate .asm programs into hack binaries (.hack)

*/

#include "hack_preprocessor.h"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s file.asm\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    generate_binary_file(argv[1]);

    return 0;
}
