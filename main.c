#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "exposure-notification.h"

typedef struct HexString { unsigned char h[32];} _HexString;


#define PRINT_CHAR_ARR(arr) for(size_t i = 0; i < sizeof(arr)/sizeof(arr[0]); i++){ printf("%02x", arr[i]);}
#define PRINT_HEX_ARR(arr) for(size_t i = 0; i < sizeof(arr)/sizeof(arr[0]); i++){ printf("%c", arr[i]);}

unsigned char hex_to_lower_char(unsigned char h) {
    unsigned char c = 0;

    if (48 <= h && h <= 57) {
        c = h-48;
    } else if (65 <= h && h <= 70) {
        c = h-65+10;
    } else if(97 <= h && h <= 102) {
        c = h-97+10;
    }

    return c;
}

unsigned char lower_char_to_hex(unsigned char c) {
    unsigned char h = 0;

    if (c < 10) {
        h = c+48;
    } else if (c < 16) {
        h = (c-10)+97;
    }

    return h;
}


void hex_to_char_arr(unsigned char h[32], unsigned char b[16]) {
    for(size_t i=0; i < 16; i++) {
        unsigned char higher = hex_to_lower_char(h[i*2]);
        unsigned char lower = hex_to_lower_char(h[i*2+1]);
        b[i] = (higher << 4) | lower;
    }
}

void char_to_hex_arr(unsigned char b[16], unsigned char h[32]) {

    for(size_t i=0; i < 16; i++) {
        unsigned char higher = (b[i] >> 4) & 0xF;
        unsigned char lower = (b[i] & 0xF);
        h[i*2] = lower_char_to_hex(higher);
        h[i*2+1] = lower_char_to_hex(lower);
    }
}


void test_conversion() {
    if (lower_char_to_hex(0) != '0') {
        printf("Failed lower_char_to_hex with 0\n");
    }
    if (lower_char_to_hex(9) != '9') {
        printf("Failed lower_char_to_hex with 9\n");
    }
    if (lower_char_to_hex(10) != 'a') {
        printf("Failed lower_char_to_hex with 10\n");
    }
    if (lower_char_to_hex(15) != 'f') {
        printf("Failed lower_char_to_hex with 16\n");
    }

    if (hex_to_lower_char('0') != 0) {
        printf("Failed hex_to_lower_char with 0 \n");
    }
    if (hex_to_lower_char('9') != 9) {
        printf("Failed hex_to_lower_char with 9 \n");
    }
    if (hex_to_lower_char('A') != 10) {
        printf("Failed hex_to_lower_char with 10 \n");
    }
    if (hex_to_lower_char('F') != 15) {
        printf("Failed hex_to_lower_char with 15 \n");
    }
}


int main(int argc, char **argv) {

    if (argc <= 1) {
        printf("Usage: en-utils generate_identifiers <tek-hex> <tek-interval> <output_file (optional)>\n");
        exit(EXIT_FAILURE);
    }

    char *action = argv[1];


    if(!strcmp("generate_identifiers", action)) {

        if (argc <= 3) {
            printf("Usage: en-utils generate_identifiers <tek-hex> <tek-interval> <output_file (optional)>\n");
            exit(EXIT_FAILURE);
        }

        FILE * output_file_fp = NULL;
        if (argc >= 5) {
            char *output_file = argv[4];
            freopen(output_file, "ab", stdout);
        }

        int tekTimestamp = 0;

        unsigned char * periodKeyHexArr = argv[2];

        if(strlen(periodKeyHexArr) != 32) {
            fprintf(stderr, "tek size mismatch");
            exit(EXIT_FAILURE);
        }

        if (sscanf (argv[3], "%i", &tekTimestamp) != 1) {
            fprintf(stderr, "tek-timestamp is not an integer");
            exit(EXIT_FAILURE);
        }

        ENPeriodKey periodKey;
        hex_to_char_arr(periodKeyHexArr, periodKey.b);
        ENIntervalNumber interval = tekTimestamp;

        ENPeriodIdentifierKey  periodIdentifierKey;
        en_derive_period_identifier_key(&periodIdentifierKey, &periodKey);

        for(int i = 0; i < EN_TEK_ROLLING_PERIOD; i++) {

            ENIntervalIdentifier intervalIdentifier;
            en_derive_interval_identifier(&intervalIdentifier, &periodIdentifierKey, interval);
            PRINT_CHAR_ARR(intervalIdentifier.b)
            printf("\n");
            interval++;
        }
        // we generate all

        if(output_file_fp)
            fclose(output_file_fp);
        exit(EXIT_SUCCESS);
    }
}
