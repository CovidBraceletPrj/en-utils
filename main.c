#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "exposure-notification.h"


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


void hex_to_char_arr(unsigned char h[], unsigned char b[], size_t numBytes) {
    for(size_t i=0; i < numBytes; i++) {
        unsigned char higher = hex_to_lower_char(h[i*2]);
        unsigned char lower = hex_to_lower_char(h[i*2+1]);
        b[i] = (higher << 4) | lower;
    }
}

void char_to_hex_arr(unsigned char b[], unsigned char h[], size_t numBytes) {

    for(size_t i=0; i < numBytes; i++) {
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
        printf("Usage: en_utils generate_identifiers <tek-hex> <tek-interval> <output_file (optional)>\n");
        exit(EXIT_FAILURE);
    }

    char *action = argv[1];


    if(!strcmp("generate_identifiers", action)) {

        if (argc <= 3) {
            printf("Usage: en_utils generate_identifiers <tek-hex> <tek-interval> <output_file (optional)>\n");
            exit(EXIT_FAILURE);
        }

        FILE * output_file_fp = NULL;
        if (argc >= 5) {
            char *output_file = argv[4];
            freopen(output_file, "ab", stdout);
        }

        ENIntervalNumber interval = 0;

        unsigned char * periodKeyHexArr = argv[2];

        if(strlen(periodKeyHexArr) != 32) {
            fprintf(stderr, "tek size mismatch");
            exit(EXIT_FAILURE);
        }

        if (sscanf (argv[3], "%i", &interval) != 1) {
            fprintf(stderr, "tek-interval is not an integer");
            exit(EXIT_FAILURE);
        }

        ENPeriodKey periodKey;
        hex_to_char_arr(periodKeyHexArr, periodKey.b, sizeof(periodKey.b));

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
    } else if(!strcmp("decrypt_metadata", action)) {

        if (argc <= 4) {
            printf("Usage: en_utils decrypt_metadata <tek> <interval> <metadata in hex>\n");
            exit(EXIT_FAILURE);
        }

        ENIntervalNumber interval = 0;

        unsigned char * periodKeyHexArr = argv[2];

        if(strlen(periodKeyHexArr) != 32) {
            fprintf(stderr, "tek size mismatch");
            exit(EXIT_FAILURE);
        }

        if (sscanf (argv[3], "%i", &interval) != 1) {
            fprintf(stderr, "interval is not an integer");
            exit(EXIT_FAILURE);
        }
        printf("Interval: %d\n", interval);

        unsigned char * encryptedMetadataHex = argv[4];

        if(strlen(encryptedMetadataHex) == 0) {
            fprintf(stderr, "Empty metadata");
            exit(EXIT_FAILURE);
        }

        if(strlen(encryptedMetadataHex) % 2 != 0) {
            fprintf(stderr, "Metadata needs to have an even hex length!");
            exit(EXIT_FAILURE);
        }

        size_t byteLength = strlen(encryptedMetadataHex)/2;

        unsigned char * encryptedMetadata = malloc(byteLength);
        unsigned char * decryptedMetadata = malloc(byteLength);
        unsigned char * decryptedMetadataHex = malloc(byteLength*2);
        hex_to_char_arr(encryptedMetadataHex, encryptedMetadata, byteLength);

        ENPeriodKey periodKey;
        hex_to_char_arr(periodKeyHexArr, periodKey.b, sizeof(periodKey.b));

        ENPeriodMetadataEncryptionKey metadataEncryptionKey;
        ENPeriodIdentifierKey periodIdentifierKey;
        ENIntervalIdentifier intervalIdentifier;

        en_derive_period_identifier_key(&periodIdentifierKey, &periodKey);
        en_derive_interval_identifier(&intervalIdentifier, &periodIdentifierKey, interval);
        en_derive_period_metadata_encryption_key(&metadataEncryptionKey, &periodKey);

        en_decrypt_interval_metadata(&metadataEncryptionKey, &intervalIdentifier, encryptedMetadata, decryptedMetadata, byteLength);

        char_to_hex_arr(decryptedMetadata, decryptedMetadataHex, byteLength);

        printf("%.*s\n", byteLength*2, decryptedMetadataHex);

        free(encryptedMetadata);
        free(decryptedMetadata);
        free(decryptedMetadataHex);
    }  else if(!strcmp("generate_csv", action)) {

        if (argc <= 3) {
            printf("Usage: en_utils generate_csv <tek-hex> <tek-interval> <output_file (optional)>\n");
            exit(EXIT_FAILURE);
        }

        int printHeader = 1;

        if (argc >= 5) {
            char *output_file = argv[4];

            // we check if the file exists already
            FILE *output_file_fp = NULL;
            output_file_fp = fopen(output_file, "r");

            if (output_file_fp){
                printHeader = 0;
                fclose(output_file_fp);
            }
            freopen(output_file, "ab", stdout);
        }

        ENIntervalNumber startInterval = 0;

        unsigned char * periodKeyHexArr = argv[2];

        if(strlen(periodKeyHexArr) != 32) {
            fprintf(stderr, "tek size mismatch");
            exit(EXIT_FAILURE);
        }

        if (sscanf (argv[3], "%i", &startInterval) != 1) {
            fprintf(stderr, "tek-interval is not an integer");
            exit(EXIT_FAILURE);
        }

        ENPeriodKey periodKey;
        hex_to_char_arr(periodKeyHexArr, periodKey.b, sizeof(periodKey.b));

        ENPeriodIdentifierKey  periodIdentifierKey;
        en_derive_period_identifier_key(&periodIdentifierKey, &periodKey);

        ENIntervalNumber interval = startInterval;

        if(printHeader) {
            printf("TEK, TEK Start Interval, Interval, Interval Identifier\n");
        }

        for(int i = 0; i < EN_TEK_ROLLING_PERIOD; i++) {
            ENIntervalIdentifier intervalIdentifier;
            en_derive_interval_identifier(&intervalIdentifier, &periodIdentifierKey, interval);
            {
                PRINT_CHAR_ARR(periodKey.b); printf(", ");
                printf("%d, %d, ", startInterval, interval);
                PRINT_CHAR_ARR(intervalIdentifier.b)
            }
            printf("\n");
            interval++;
        }

        exit(EXIT_SUCCESS);
    } else if(!strcmp("generate_metadata_key", action)) {

        if (argc < 4) {
            printf("Usage: en_utils generate_metadata_key <tek-hex> <tek-interval>\n");
            exit(EXIT_FAILURE);
        }

        ENIntervalNumber interval = 0;

        unsigned char * periodKeyHexArr = argv[2];

        if(strlen(periodKeyHexArr) != 32) {
            fprintf(stderr, "tek size mismatch");
            exit(EXIT_FAILURE);
        }

        if (sscanf (argv[3], "%i", &interval) != 1) {
            fprintf(stderr, "interval is not an integer");
            exit(EXIT_FAILURE);
        }

        ENPeriodKey periodKey;
        hex_to_char_arr(periodKeyHexArr, periodKey.b, sizeof(periodKey.b));

        ENPeriodMetadataEncryptionKey metadataEncryptionKey;
        en_derive_period_metadata_encryption_key(&metadataEncryptionKey, &periodKey);

        unsigned char metadataKeyHex[32];
        char_to_hex_arr(metadataEncryptionKey.b, metadataKeyHex, 16);

        printf("%.*s\n", 16*2, metadataKeyHex);
        exit(EXIT_SUCCESS);
    }
}
