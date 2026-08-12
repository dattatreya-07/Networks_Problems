#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Performs 1's complement addition on two binary strings
int onescomplementadd(char *sum, char *segment, int segsize) {
    int i, carry = 0;

    for (i = segsize - 1; i >= 0; i--) {
        int a = sum[i] - '0';
        int b = segment[i] - '0';
        int total = a + b + carry;

        if (total == 0) {
            sum[i] = '0';
            carry = 0;
        } else if (total == 1) {
            sum[i] = '1';
            carry = 0;
        } else if (total == 2) {
            sum[i] = '0';
            carry = 1;
        } else {
            sum[i] = '1';
            carry = 1;
        }
    }
    return carry;
}

// Helper to apply the wrapped carry bit
void applycarry(char *sum, int segsize) {
    for (int j = segsize - 1; j >= 0; j--) {
        if (sum[j] == '1') {
            sum[j] = '0';
        } else {
            sum[j] = '1';
            break;
        }
    }
}

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

int main() {
    char text[100], data[800] = "";
    char segments[100][20], sum[20], checksum[20], complement[20];
    char codeword[900], received[900];
    int datalen, segsize = 8, numsegs, i, j, carry, choice, errorpos, codelen;
    int dataInitialized = 0, checksumGenerated = 0;

    while (1) {
        printf("\n====================================");
        printf("\n   MENU-DRIVEN CHECKSUM SIMULATOR   ");
        printf("\n====================================");
        printf("\n1. Enter Text Data & Convert to Binary");
        printf("\n2. Generate Checksum (Sender Side)");
        printf("\n3. Transmit Data & Check Validation (Receiver Side)");
        printf("\n4. Exit");
        printf("\nEnter your choice (1-4): ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }

        switch (choice) {
            case 1:
                printf("\nEnter text string (e.g., HELLO): ");
                scanf(" %[^\n]", text); // Reads string with spaces
                
                textToBinary(text, data);
                datalen = strlen(data);
                
                printf("\n[Success] Converted Text to Binary Bitstream:\n");
                printf("Binary Data: %s (Length: %d bits)\n", data, datalen);
                
                dataInitialized = 1;
                checksumGenerated = 0; // Reset state for new data
                break;

            case 2:
                if (!dataInitialized) {
                    printf("\n[Error] Please enter text data first (Option 1).\n");
                    break;
                }

                printf("\nEnter segment size (multiples of 8 recommended, e.g., 8): ");
                scanf("%d", &segsize);

                datalen = strlen(data);
                numsegs = datalen / segsize;

                // Handle uneven padding if text bits don't match segsize perfectly
                if (datalen % segsize != 0) {
                    int paddingNeeded = segsize - (datalen % segsize);
                    char padding[50] = "";
                    for(i=0; i<paddingNeeded; i++) strcat(padding, "0");
                    strcat(padding, data);
                    strcpy(data, padding);
                    datalen = strlen(data);
                    numsegs = datalen / segsize;
                    printf("[Notice] Data padded with leading zeros to match segment size.\n");
                }

                // Slice data into blocks
                for (i = 0; i < numsegs; i++) {
                    for (j = 0; j < segsize; j++) {
                        segments[i][j] = data[i * segsize + j];
                    }
                    segments[i][segsize] = '\0';
                }

                printf("\n--- Sender Side ---\n");
                printf("Segments:\n");
                for (i = 0; i < numsegs; i++) {
                    printf("Segment %d: %s\n", i + 1, segments[i]);
                }

                // Initialize sum block
                for (i = 0; i < segsize; i++) sum[i] = '0';
                sum[segsize] = '\0';

                // Add all segments
                for (i = 0; i < numsegs; i++) {
                    carry = onescomplementadd(sum, segments[i], segsize);
                    if (carry == 1) {
                        applycarry(sum, segsize);
                    }
                }
                printf("Sum: %s\n", sum);

                // Compute bitwise NOT for Checksum
                for (i = 0; i < segsize; i++) {
                    checksum[i] = (sum[i] == '0') ? '1' : '0';
                }
                checksum[segsize] = '\0';
                printf("Checksum generated: %s\n", checksum);

                // Append checksum to form final codeword
                strcpy(codeword, data);
                strcat(codeword, checksum);
                codelen = strlen(codeword);
                printf("Transmitted codeword: %s\n", codeword);

                checksumGenerated = 1;
                break;

            case 3:
                if (!checksumGenerated) {
                    printf("\n[Error] Please generate the checksum first (Option 2).\n");
                    break;
                }

                strcpy(received, codeword);
                
                int errChoice;
                printf("\nDo you want to inject a bit error during transmission? (1=Yes, 0=No): ");
                scanf("%d", &errChoice);

                if (errChoice == 1) {
                    printf("Enter bit position to flip (0 to %d): ", codelen - 1);
                    scanf("%d", &errorpos);
                    if(errorpos >= 0 && errorpos < codelen) {
                        received[errorpos] = (received[errorpos] == '0') ? '1' : '0';
                        printf("[Injected] Bit flipped at index %d!\n", errorpos);
                    } else {
                        printf("[Error] Invalid position. Proceeding without errors.\n");
                    }
                }

                printf("\n--- Receiver Side ---\n");
                printf("Received codeword: %s\n", received);

                int recvsegs = codelen / segsize;
                char recvsegments[100][20];

                // Slice received stream
                for (i = 0; i < recvsegs; i++) {
                    for (j = 0; j < segsize; j++) {
                        recvsegments[i][j] = received[i * segsize + j];
                    }
                    recvsegments[i][segsize] = '\0';
                }

                // Initialize receiver sum
                for (i = 0; i < segsize; i++) sum[i] = '0';
                sum[segsize] = '\0';

                // Add elements
                for (i = 0; i < recvsegs; i++) {
                    carry = onescomplementadd(sum, recvsegments[i], segsize);
                    if (carry == 1) {
                        applycarry(sum, segsize);
                    }
                }
                printf("Sum at receiver: %s\n", sum);

                // Complement the final receiver sum
                for (i = 0; i < segsize; i++) {
                    complement[i] = (sum[i] == '0') ? '1' : '0';
                }
                complement[segsize] = '\0';
                printf("Complement: %s\n", complement);

                // Verify if complement is completely composed of zeros
                int iszero = 1;
                for (i = 0; i < segsize; i++) {
                    if (complement[i] != '0') {
                        iszero = 0;
                    }
                }

                if (iszero == 1) {
                    printf("\n>>>> Result: No error detected (Data is valid) <<<<\n");
                } else {
                    printf("\n>>>> Result: Error detected in received data! <<<<\n");
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
