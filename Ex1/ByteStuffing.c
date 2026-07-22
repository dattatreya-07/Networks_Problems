#include <stdio.h>
#include <string.h>
#define START_END_BORDER "01111110"
#define ESCAPE_SEQUENCE "01111101"
char raw_bits[100][9];
char transmitted_stream[300][9];
char recovered_bits[100][9];

void convertCharToBin(char character, char stream[]) {
    int bit_index;
    for(bit_index = 7; bit_index >= 0; bit_index--) {
        stream[7 - bit_index] = ((character >> bit_index) & 1) + '0';
    }
    stream[8] = '\0';
}

char convertBinToChar(char stream[]) {
    int bit_index, decimal_sum = 0;
    for(bit_index = 0; bit_index < 8; bit_index++) {
        decimal_sum = decimal_sum * 2 + (stream[bit_index] - '0');
    }
    return (char)decimal_sum;
}
int main() {
    char user_input[100], output_text[100];
    int idx, total_elements = 0, extracted_count = 0, input_length;
    printf("Provide the input text: ");
    scanf("%s", user_input);
    input_length = strlen(user_input);
    printf("\n--- Character to Binary Conversion ---\n");
    for(idx = 0; idx < input_length; idx++) {
        convertCharToBin(user_input[idx], raw_bits[idx]);
        printf("[%c] represents bits: %s\n", user_input[idx], raw_bits[idx]);
    }
    strcpy(transmitted_stream[total_elements++], START_END_BORDER);
    for(idx = 0; idx < input_length; idx++) {
        if(strcmp(raw_bits[idx], START_END_BORDER) == 0 || strcmp(raw_bits[idx], ESCAPE_SEQUENCE) == 0) {
            strcpy(transmitted_stream[total_elements++], ESCAPE_SEQUENCE);
        }
        strcpy(transmitted_stream[total_elements++], raw_bits[idx]);
    }
    strcpy(transmitted_stream[total_elements++], START_END_BORDER);

    printf("\n--- Formatted Bitstream Frame ---\n");
    for(idx = 0; idx < total_elements; idx++) {
        printf("%s ", transmitted_stream[idx]);
    }
    printf("\n\n--- Extracted Payload Bits ---\n");
    for(idx = 1; idx < total_elements - 1; idx++) {
        if(strcmp(transmitted_stream[idx], ESCAPE_SEQUENCE) == 0) {
            continue;
        }
        strcpy(recovered_bits[extracted_count], transmitted_stream[idx]);
        printf("%s ", recovered_bits[extracted_count]);
        extracted_count++;
    }
    for(idx = 0; idx < extracted_count; idx++) {
        output_text[idx] = convertBinToChar(recovered_bits[idx]);
    }
    output_text[extracted_count] = '\0';

    printf("\n\nFinal Restored Text: %s\n", output_text);
    return 0;
}
