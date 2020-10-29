#include "kupyna.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <math.h>

uint64_t BUFFER_SIZE;
extern size_t byte_size;
extern kupyna_config kupyna_256;
extern kupyna_config kupyna_384;
extern kupyna_config kupyna_512;
extern kupyna_config * kupyna;

int parse_arguments(int argc, char ** argv, FILE ** input, size_t * file_size, int *zeros);

int main(int argc, char ** argv) {
    FILE * input;
    size_t file_size;
    int zeros = -1;
    
    if (parse_arguments(argc, argv, &input, &file_size, &zeros) != 0) {
        return 1;
    }

    BUFFER_SIZE = (file_size/kupyna->block_bsize + 3) * kupyna->block_bsize;
    byte * rbuffer = malloc(BUFFER_SIZE * sizeof(byte));
    byte * wbuffer = malloc(kupyna->message_diggest_bsize + 1);

    size_t reads;
    size_t read_blocks;

    if ((reads = fread(rbuffer, sizeof(byte), BUFFER_SIZE, input)) != 0) {
        read_blocks = reads / kupyna->block_bsize;

        pad_block((dw *)(rbuffer + read_blocks * kupyna->block_bsize), reads * byte_size);
        read_blocks++;

        if (zeros == -1) {
            hash((dw *)rbuffer, read_blocks, (dw *)wbuffer); 
        } else {
            proof_of_work((dw *)rbuffer, read_blocks, zeros, (dw *)wbuffer);
            read_blocks++;
        }

        for (int i = 0; i < kupyna->message_diggest_bsize/sizeof(dw); i++) {
            if (fprintf(stdout, "%16lx", ((dw *)wbuffer)[i]) == 0) {
                fprintf(stderr, "Error occured while writting to the output.");
                return -1;
            }
        }
    } else {
        fprintf(stderr, "Error occured while reading the file.");
    }

    fprintf(stdout, "\n");

    free(rbuffer);
    free(wbuffer);
    fclose(input);

    return 0;
}

int parse_arguments(int argc, char ** argv, FILE ** input, size_t * file_size, int * zeros) {
    int option;
    if (argc >= 2) {
        *input = fopen(argv[1], "r");
        fseek(*input, 0L, SEEK_END);
        *file_size = ftell(*input);
        fseek(*input, 0L, SEEK_SET);
    }

    while ((option = getopt(argc, argv, "s:p:h")) != -1) {
        switch (option) {
            case 's':
                if (strcasecmp(optarg, "kupyna_256") == 0) {
                    setup(&kupyna_256);
                } else if (strcasecmp(optarg, "kupyna_384") == 0) {
                    setup(&kupyna_384);
                } else if (strcasecmp(optarg, "kupyna_512") == 0) {
                    setup(&kupyna_512);
                } else {
                    fprintf(stderr, "%s wrong standart name, looks -h for supported standarts.\n", optarg);
                    return -1;
                }
                break;
            case 'p':
                *zeros = atoi(optarg);
                break;
            case 'h':
                printf("Usage:  kupyna file_path [options]\n\n\
Kupyna standart hash function.\n\
Options:\n\
  -s    Standart name. Allowed options: kupyna_256, kupyna_384, kupyna_512,\n\
  -p    [size] - Proof of Work. size zeros in hash res.\n\
  -h    display this help.\n");

                return 1;
        }
    }

    return 0;
}
