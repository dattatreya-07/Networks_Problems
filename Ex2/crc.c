#include <stdio.h>
#include <string.h>

int main() {
    char packet[50], divisor[20], total_bits[50], inbound[50];
    char residue[20], scratchpad[50];
    int pkt_len, div_len, final_len, x, y, toggle_opt, flip_idx;

    printf("--> Enter message payload (binary): ");
    scanf("%s", packet);

    printf("--> Enter divisor polynomial (binary): ");
    scanf("%s", divisor);

    pkt_len = strlen(packet);
    div_len = strlen(divisor);
    final_len = pkt_len + div_len - 1;

    // Pad binary string with trailing zeros
    for (x = 0; x < pkt_len; x++) {
        scratchpad[x] = packet[x];
    }
    for (x = 0; x < div_len - 1; x++) {
        scratchpad[pkt_len + x] = '0';
    }
    scratchpad[pkt_len + div_len - 1] = '\0';

    // Seed the first division block
    for (x = 0; x < div_len; x++) {
        residue[x] = scratchpad[x];
    }
    residue[div_len] = '\0';

    // Execute CRC XOR logic (Sender Side)
    for (x = 0; x <= final_len - div_len; x++) {
        if (residue[0] == '1') {
            for (y = 0; y < div_len; y++) {
                if (residue[y] == divisor[y]) {
                    residue[y] = '0';
                } else {
                    residue[y] = '1';
                }
            }
        }

        for (y = 0; y < div_len - 1; y++) {
            residue[y] = residue[y + 1];
        }

        if (x + div_len < final_len) {
            residue[div_len - 1] = scratchpad[x + div_len];
        } else {
            residue[div_len - 1] = '\0';
        }
    }
    residue[div_len - 1] = '\0';

    printf("\n================ [ TRANSMITTER ] ================");
    printf("\nSource Bits   : %s", packet);
    printf("\nPolynomial    : %s", divisor);
    printf("\nCRC Syndrome  : %s", residue);

    // Construct final block to transmit
    for (x = 0; x < pkt_len; x++) {
        total_bits[x] = packet[x];
    }
    for (x = 0; x < div_len - 1; x++) {
        total_bits[pkt_len + x] = residue[x];
    }
    total_bits[pkt_len + div_len - 1] = '\0';

    printf("\nFinal Code Word: %s\n", total_bits);

    strcpy(inbound, total_bits);

    printf("\nInject artificial bit flip? (1=Yes / 0=No): ");
    scanf("%d", &toggle_opt);

    if (toggle_opt == 1) {
        printf("Index boundary to toggle (0 to %d): ", final_len - 1);
        scanf("%d", &flip_idx);
        if (inbound[flip_idx] == '0') {
            inbound[flip_idx] = '1';
        } else {
            inbound[flip_idx] = '0';
        }
    }

    printf("\n================= [ RECEIVER ] =================");
    printf("\nArrived Packet : %s", inbound);

    for (x = 0; x < final_len; x++) {
        scratchpad[x] = inbound[x];
    }
    scratchpad[final_len] = '\0';

    for (x = 0; x < div_len; x++) {
        residue[x] = scratchpad[x];
    }
    residue[div_len] = '\0';

    // Execute CRC XOR logic (Receiver Side)
    for (x = 0; x <= final_len - div_len; x++) {
        if (residue[0] == '1') {
            for (y = 0; y < div_len; y++) {
                if (residue[y] == divisor[y]) {
                    residue[y] = '0';
                } else {
                    residue[y] = '1';
                }
            }
        }

        for (y = 0; y < div_len - 1; y++) {
            residue[y] = residue[y + 1];
        }

        if (x + div_len < final_len) {
            residue[div_len - 1] = scratchpad[x + div_len];
        } else {
            residue[div_len - 1] = '\0';
        }
    }
    residue[div_len - 1] = '\0';

    printf("\nFinal Remainder: %s", residue);

    int clean_flag = 1;
    for (x = 0; x < div_len - 1; x++) {
        if (residue[x] != '0') {
            clean_flag = 0;
        }
    }

    printf("\n------------------------------------------------");
    if (clean_flag == 1) {
        printf("\nSTATUS: Success (No anomalies caught)\n");
    } else {
        printf("\nSTATUS: Corrupted (Data transmission mismatch)\n");
    }
    printf("================================================\n");

    return 0;
}
