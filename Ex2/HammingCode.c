#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Converts a standard text string into a binary bit string
void textToBinary(char *text, char *binaryStr) {
    binaryStr[0] = '\0';
    for (int i = 0; text[i] != '\0'; i++) {
        char ch = text[i];
        for (int j = 7; j >= 0; j--) {
            if ((ch >> j) & 1) {
                strcat(binaryStr, "1");
            } else {
                strcat(binaryStr, "0");
            }
        }
    }
}

// Converts a binary bit string back into a standard text string
void binaryToText(char *binaryStr, char *textStr) {
    int len = strlen(binaryStr);
    int textIndex = 0;
    
    for (int i = 0; i < len; i += 8) {
        char ch = 0;
        for (int j = 0; j < 8; j++) {
            ch = (ch << 1) | (binaryStr[i + j] - '0');
        }
        textStr[textIndex++] = ch;
    }
    textStr[textIndex] = '\0';
}

int main() {
    char text[50], data[400], correcteddata[400], extractedText[50];
    int datalen, r, codelen, i, j, pos, choice, errorpos;
    int codeword[500], received[500];
    
    int dataInitialized = 0;
    int codewordGenerated = 0;

    while (1) {
        printf("\n====================================");
        printf("\n    MENU-DRIVEN HAMMING CODE TOOL   ");
        printf("\n====================================");
        printf("\n1. Enter Text Data & Convert to Binary");
        printf("\n2. Generate Hamming Codeword (Sender Side)");
        printf("\n3. Transmit, Detect, Correct & Extract (Receiver Side)");
        printf("\n4. Exit");
        printf("\nEnter your choice (1-4): ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }

        switch (choice) {
            case 1:
                printf("\nEnter text string (up to 20 chars, e.g., HELLO): ");
                scanf(" %[^\n]", text); // Reads string with spaces
                
                textToBinary(text, data);
                datalen = strlen(data);
                
                printf("\n[Success] Converted Text to Binary Bitstream:\n");
                printf("Binary Data: %s (Length: %d bits)\n", data, datalen);
                
                dataInitialized = 1;
                codewordGenerated = 0; // Reset state machine flags
                break;

            case 2:
                if (!dataInitialized) {
                    printf("\n[Error] Please enter text data first (Option 1).\n");
                    break;
                }

                datalen = strlen(data);

                // Calculate required number of parity bits (r)
                r = 0;
                while ((1 << r) < datalen + r + 1) {
                    r = r + 1;
                }

                codelen = datalen + r;

                // Place data bits leaving positions that are powers of 2 blank (-1)
                j = 0;
                for (i = 1; i <= codelen; i++) {
                    if ((i & (i - 1)) == 0) {
                        codeword[i] = -1;
                    } else {
                        codeword[i] = data[j] - '0';
                        j = j + 1;
                    }
                }

                // Calculate parity bit values using even parity configuration
                for (i = 0; i < r; i++) {
                    pos = 1 << i;
                    int count = 0;
                    for (j = pos; j <= codelen; j++) {
                        if (((j / pos) % 2) == 1) {
                            if (codeword[j] == 1) {
                                count = count + 1;
                            }
                        }
                    }
                    if (count % 2 == 0) {
                        codeword[pos] = 0;
                    } else {
                        codeword[pos] = 1;
                    }
                }

                printf("\n--- Sender Side ---\n");
                printf("Data bits: %s\n", data);
                printf("Number of parity bits: %d\n", r);
                printf("Transmitted codeword: ");
                for (i = 1; i <= codelen; i++) {
                    printf("%d", codeword[i]);
                }
                printf("\n");

                codewordGenerated = 1;
                break;

            case 3:
                if (!codewordGenerated) {
                    printf("\n[Error] Please generate the codeword first (Option 2).\n");
                    break;
                }

                // Copy generated codeword to transmission stream array
                for (i = 1; i <= codelen; i++) {
                    received[i] = codeword[i];
                }

                int errChoice;
                printf("\nDo you want to introduce an error during transmission? (1 for yes, 0 for no): ");
                scanf("%d", &errChoice);

                if (errChoice == 1) {
                    printf("Enter bit position to flip (1 to %d): ", codelen);
                    scanf("%d", &errorpos);
                    if (errorpos >= 1 && errorpos <= codelen) {
                        received[errorpos] = (received[errorpos] == 0) ? 1 : 0;
                        printf("[Injected] Bit flipped at position %d!\n", errorpos);
                    } else {
                        printf("[Error] Invalid position. Proceeding without errors.\n");
                    }
                }

                printf("\n--- Receiver Side ---\n");
                printf("Received codeword: ");
                for (i = 1; i <= codelen; i++) {
                    printf("%d", received[i]);
                }
                printf("\n");

                // Evaluate error positions using the Syndrome value
                int syndrome = 0;
                for (i = 0; i < r; i++) {
                    pos = 1 << i;
                    int count = 0;
                    for (j = pos; j <= codelen; j++) {
                        if (((j / pos) % 2) == 1) {
                            if (received[j] == 1) {
                                count = count + 1;
                            }
                        }
                    }
                    if (count % 2 != 0) {
                        syndrome = syndrome + pos;
                    }
                }

                printf("Syndrome (calculated error position): %d\n", syndrome);

                if (syndrome == 0) {
                    printf("Result: No error detected\n");
                } else {
                    if (syndrome <= codelen) {
                        printf("Result: Error detected at position %d\n", syndrome);
                        received[syndrome] = (received[syndrome] == 0) ? 1 : 0; // Flip back to correct
                        printf("Corrected codeword: ");
                        for (i = 1; i <= codelen; i++) {
                            printf("%d", received[i]);
                        }
                        printf("\n");
                    } else {
                        printf("Result: Multiple errors suspected! Syndrome %d out of bounds.\n", syndrome);
                    }
                }

                // Strip the parity bits to get data bits back
                j = 0;
                for (i = 1; i <= codelen; i++) {
                    if ((i & (i - 1)) != 0) {
                        correcteddata[j] = received[i] + '0';
                        j = j + 1;
                    }
                }
                correcteddata[j] = '\0';

                printf("Extracted data bits: %s\n", correcteddata);
                
                // Unpack the verified binary back into clear text characters
                binaryToText(correcteddata, extractedText);
                printf(">>>> Decoded Text Output: %s <<<<\n", extractedText);
                break;

            case 4:
                printf("\nExiting program. Goodbye!\n");
                exit(0);

            default:
                printf("\n[Error] Invalid choice! Please select between 1 and 4.\n");
        }
    }
    return 0;
}
