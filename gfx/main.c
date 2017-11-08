#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tybo_compression.h"

//This needs to be big enough to hold unknown size data
//being compressed or decompressed.
#define BUFF_LEN (1 * 1024 * 1024) //1mb

void print_usage() {
    printf("Usage:\n\n");
    printf("tybogfx d|c|cc address rom.smc gfx.bin\n\n");
    printf("d = Decompress data at address\n");
    printf("c = Compress data and insert at address\n");
    printf("cc = Compress data and aks before inserting\n\n");
    printf("Examples:\n\n");
    printf("tybogfx d 19b5fb tybo.smc title.bin\n");
    printf("tybogfx d 060010 tybo.smc ifont.bin\n");
    printf("tybogfx c 060010 tybo.smc edited.bin\n");
}

int main(int argc, char **argv) {
    printf("TYBOGfx Tadaima Yuusha Boshuuchuu Okawari Graphics (De)Compressor\n");
    printf("This is FREE software released into the PUBLIC DOMAIN\n");
    printf("0xc0ded by gdkchan\n");
    printf("Version 1.1\n\n");

    if (argc != 5) {
        print_usage();

        return EXIT_SUCCESS;
    }

    if (strcmp(argv[1], "d")  != 0 &&
        strcmp(argv[1], "c")  != 0 &&
        strcmp(argv[1], "cc") != 0) {
        print_usage();

        return EXIT_SUCCESS;
    }

    FILE *rom = NULL;
    FILE *gfx = NULL;

    unsigned char *cmp = NULL;
    unsigned char *dec = NULL;

    int e_code = EXIT_FAILURE;

    long int rom_addr = strtol(argv[2], NULL, 16);

#define ALLOC_BUFFS \
    do { \
        cmp = malloc(cmp_len); \
        dec = malloc(dec_len); \
\
        if (cmp == NULL || dec == NULL) { \
            printf("ERR: Can't allocate memory!\n"); \
\
            goto program_exit; \
        } \
    } while (0)

    if (strcmp(argv[1], "d") == 0) {
        rom = fopen(argv[3], "rb");

        if (rom == NULL) {
            printf("ERR: Can't open ROM file!\n");
    
            goto program_exit;
        }

        fseek(rom, 0, SEEK_END);

        int rom_len = ftell(rom);

        int cmp_len = rom_len - rom_addr;
        int dec_len = BUFF_LEN;

        ALLOC_BUFFS;

        if (fseek(rom, rom_addr, SEEK_SET) != 0) {
            printf("ERR: Invalid ROM address!\n");
    
            goto program_exit;
        }

        fread(cmp, sizeof(char), cmp_len, rom);

        if (tybo_decompress(cmp, dec, &cmp_len, &dec_len) != TYBO_ERR_SUCCESS) {
            printf("ERR: Data decompression failed! ;_;\n");

            goto program_exit;
        }

        gfx = fopen(argv[4], "wb");

        if (gfx == NULL) {
            printf("ERR: Can't open output file!\n");

            goto program_exit;
        }

        printf("Compressed size: %d bytes\n", cmp_len);
        printf("Decompressed size: %d bytes\n", dec_len);

        fwrite(dec, sizeof(char), dec_len, gfx);

        printf("Data was extracted successfully! Yaaay!\n");

        e_code = EXIT_SUCCESS;
    } else {
        gfx = fopen(argv[4], "rb");

        if (gfx == NULL) {
            printf("ERR: Can't open input file!\n");

            goto program_exit;
        }

        fseek(gfx, 0, SEEK_END);

        int dec_len = ftell(gfx);
        int cmp_len = BUFF_LEN;

        ALLOC_BUFFS;

        fseek(gfx, 0, SEEK_SET);
        fread(dec, sizeof(char), dec_len, gfx);

        if (tybo_compress(cmp, dec, &cmp_len, &dec_len) != TYBO_ERR_SUCCESS) {
            printf("ERR: Data compression failed!\n");

            goto program_exit;
        }

        rom = fopen(argv[3], "rb+");

        if (rom == NULL) {
            printf("ERR: Can't open ROM file!\n");
    
            goto program_exit;
        }

        if (fseek(rom, rom_addr, SEEK_SET) != 0) {
            printf("ERR: Invalid ROM address!\n");
    
            goto program_exit;
        }

        printf("Compressed size: %d bytes\n", cmp_len);
        printf("Decompressed size: %d bytes\n", dec_len);

        if (strcmp(argv[1], "cc") == 0) {
            printf("Are you sure you want to insert the compressed data? (y/n) ");

            char choice;

            scanf("%c", &choice);

            if (choice != 'y') {
                e_code = EXIT_SUCCESS;

                goto program_exit;
            }
        }

        if (fwrite(cmp, sizeof(char), cmp_len, rom) != cmp_len) {
            printf("ERR: Can't write all data to ROM!\n");
            
            goto program_exit;
        }

        printf("Data was inserted successfully!\n");

        e_code = EXIT_SUCCESS;
    }

program_exit:
    if (rom != NULL) fclose(rom);
    if (gfx != NULL) fclose(gfx);

    if (cmp != NULL) free(cmp);
    if (dec != NULL) free(dec);

    return e_code;
}