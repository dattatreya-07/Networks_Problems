#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Converts a standard text string into a continuous binary bit string
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

// Converts a binary bit string back into standard text characters
void binaryToText(char *binaryStr, int len, char *textStr) {
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
    // Increased bounds to safely hold text strings and conversions
    char text[50], databits[400];
    int senderdata[405], receiverdata[405];
    int n = 0, i, count, paritybit, paritytype = 1, errorpos, choice;
    
    int dataInitialized = 0;
    int parityGenerated = 0;

    while (1) {
        printf("\n====================================");
        printf("\n    MENU-DRIVEN PARITY CHECK TOOL   ");
        printf("\n====================================");
        printf("\n1. Enter Text Data & Convert to Binary");
        printf("\n2. Generate Parity Bit (Sender Side)");
        printf("\n3. Transmit, Detect Error & Extract Text (Receiver Side)");
        printf("\n4. Exit");
        printf("\nEnter your choice (1-4): ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }

        switch (choice) {
            case 1:
                printf("\nEnter text string (e.g., HELLO): ");
                scanf(" %[^\n]", text); // Reads string including spaces
                
                textToBinary(text, databits);
                n = strlen(databits);
                
                for (i = 0; i < n; i++) {
                    senderdata[i] = databits[i] - '0';
                }
                
                printf("\n[Success] Converted Text to Binary Bitstream:\n");
                printf("Binary Data: %s (Length: %d bits)\n", databits, n);
                
                dataInitialized = 1;
                parityGenerated = 0; // Reset workflow engine state
                break;

            case 2:
                if (!dataInitialized) {
                    printf("\n[Error] Please enter text data first (Option 1).\n");
                    break;
                }

                printf("\nChoose parity configuration:\n");
                printf("1. Even Parity\n");
                printf("2. Odd Parity\n");
                printf("Enter your choice (1 or 2): ");
                scanf("%d", &paritytype);

                if (paritytype != 1 && paritytype != 2) {
                    printf("[Error] Invalid configuration selected. Defaulting to Even Parity.\n");
                    paritytype = 1;
                }

                // Count total number of set bits (1s)
                count = 0;
                for (i = 0; i < n; i++) {
                    if (senderdata[i] == 1) {
                        count++;
                    }
                }

                // Compute the trailing parity bit value
                if (paritytype == 1) {
                    paritybit = (count % 2 == 0) ? 0 : 1;
                } else {
                    paritybit = (count % 2 == 0) ? 1 : 0;
                }

                senderdata[n] = paritybit;

                printf("\n--- Sender Side ---\n");
                printf("Data bits: ");
                for (i = 0; i < n; i++) {
                    printf("%d", senderdata[i]);
                }
                printf("\nParity bit generated: %d (%s Parity)\n", paritybit, (paritytype == 1) ? "Even" : "Odd");
                printf("Transmitted codeword: ");
                for (i = 0; i <= n; i++) {
                    printf("%d", senderdata[i]);
                }
                printf("\n");

                parityGenerated = 1;
                break;

            case 3:
                if (!parityGenerated) {
                    printf("\n[Error] Please generate the parity bit first (Option 2).\n");
                    break;
                }

                // Stage data into the network transmission array
                for (i = 0; i <= n; i++) {
                    receiverdata[i] = senderdata[i];
                }

                int errChoice;
                printf("\nDo you want to inject a bit error during transmission? (1 for yes, 0 for no): ");
                scanf("%d", &errChoice);

                if (errChoice == 1) {
                    printf("Enter bit position to flip (0 to %d): ", n);
                    scanf("%d", &errorpos);
                    if (errorpos >= 0 && errorpos <= n) {
                        receiverdata[errorpos] = (receiverdata[errorpos] == 0) ? 1 : 0;
                        printf("[Injected] Bit flipped at position %d!\n", errorpos);
                    } else {
                        printf("[Error] Invalid position. Proceeding without errors.\n");
                    }
                }

                printf("\n--- Receiver Side ---\n");
                printf("Received codeword: ");
                for (i = 0; i <= n; i++) {
                    printf("%d", receiverdata[i]);
                }
                printf("\n");

                // Evaluate the bit counts at destination
                count = 0;
                for (i = 0; i <= n; i++) {
                    if (receiverdata[i] == 1) {
                        count++;
                    }
                }

                int errorDetected = 0;
                if (paritytype == 1) {
                    if (count % 2 != 0) errorDetected = 1;
                } else {
                    if (count % 2 == 0) errorDetected = 1;
                }

                if (errorDetected == 0) {
                    printf("\n>>>> Result: No error detected (Data is valid) <<<<\n");
                    
                    // Recover original binary data stream bits back into characters
                    char extractedBinary[400];
                    for (i = 0; i < n; i++) {
                        extractedBinary[i] = receiverdata[i] + '0';
                    }
                    extractedBinary[n] = '\0';

                    char extractedText[50];
                    binaryToText(extractedBinary, n, extractedText);
                    printf("Decoded Text Output: %s\n", extractedText);
                } else {
                    printf("\n>>>> Result: Error detected in received data! <<<<\n");
                    printf("(Note: Single parity check can detect the error but cannot correct it.)\n");
                }
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
