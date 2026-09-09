#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void stopAndWait(int n, int lost);
void goBack(int n, int w, int lost);
void selectiveRepeat(int n, int w, int lost);

int main() {
    int choice, n, w, lost;

    while (1) {
        printf("\n=============================================");
        printf("\n       SLIDING WINDOW PROTOCOLS MENU         ");
        printf("\n=============================================");
        printf("\n1. Stop and Wait ARQ");
        printf("\n2. Go-Back-N ARQ");
        printf("\n3. Selective Repeat ARQ");
        printf("\n4. Exit");
        printf("\nEnter your choice (1-4): ");
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting.\n");
            break;
        }

        if (choice == 4) {
            printf("Exiting program. Goodbye!\n");
            break;
        }

        if (choice < 1 || choice > 4) {
            printf("Invalid choice! Please select a valid option.\n");
            continue;
        }

        printf("Enter the total number of frames to send: ");
        scanf("%d", &n);

        if (choice == 2 || choice == 3) {
            printf("Enter the window size (W): ");
            scanf("%d", &w);
        }

        printf("Enter the frame number that gets lost/damaged (0 to %d, or -1 for no loss): ", n - 1);
        scanf("%d", &lost);

        switch (choice) {
            case 1: stopAndWait(n, lost); break;
            case 2: goBack(n, w, lost); break;
            case 3: selectiveRepeat(n, w, lost); break;
        }
    }
    return 0;
}

void stopAndWait(int n, int lost) {
    printf("\n--- Simulating Stop-and-Wait ARQ ---\n");
    bool isLostHandled = false;

    for (int i = 0; i < n; i++) {
        printf("\nSender: Sending Frame %d...", i);

        if (i == lost && !isLostHandled) {
            printf("\n[!] Frame %d was LOST or DAMAGED in transit.", i);
            printf("\nReceiver: (No response due to timeout)");
            printf("\nSender: Timeout expired! Retransmitting Frame %d...", i);
            isLostHandled = true; 
            i--; // Retry same frame
            continue;
        }

        printf("\nReceiver: Frame %d received successfully.", i);
        printf("\nReceiver: Sending ACK %d...", i + 1);
        printf("\nSender: ACK %d received.", i + 1);
    }
    printf("\n\nAll %d frames sent and acknowledged successfully!\n", n);
}

void goBack(int n, int w, int lost) {
    printf("\n--- Simulating Go-Back-N ARQ (Window Size = %d) ---\n", w);

    int i = 0; // Base of the window
    bool isLostHandled = false;

    while (i < n) {
        int windowEnd = (i + w < n) ? (i + w) : n;
        printf("\n--- Current Window: Frames [");
        for (int k = i; k < windowEnd; k++) {
            printf("%d%s", k, (k == windowEnd - 1) ? "" : ", ");
        }
        printf("] ---\n");

        bool errorInWindow = false;
        int firstFailure = -1;

        // Sender transmits the ENTIRE window pipeline burst
        for (int j = i; j < windowEnd; j++) {
            printf("Sender: Sending Frame %d...\n", j);
            if (j == lost && !isLostHandled) {
                errorInWindow = true;
                if (firstFailure == -1) firstFailure = j;
            }
        }

        // Receiver processes the frames sequentially
        if (errorInWindow) {
            for (int j = i; j < windowEnd; j++) {
                if (j < firstFailure) {
                    printf("Receiver: Frame %d received successfully. Sending ACK %d.\n", j, j + 1);
                } else if (j == firstFailure) {
                    printf("[!] Receiver: Frame %d was LOST/DAMAGED.\n", j);
                } else {
                    printf("Receiver: Frame %d discarded (Out of order! Waiting for %d).\n", j, firstFailure);
                }
            }
            printf("Sender: Timeout / NAK for Frame %d. Going Back N!\n", firstFailure);
            isLostHandled = true;
            i = firstFailure; // Slide window base back to the failed frame position
        } else {
            // Success scenario
            for (int j = i; j < windowEnd; j++) {
                printf("Receiver: Frame %d received. Sending ACK %d.\n", j, j + 1);
                printf("Sender: ACK %d received. Sliding window forward.\n", j + 1);
            }
            i = windowEnd; 
        }
    }
    printf("\nAll %d frames sent and acknowledged successfully!\n", n);
}

void selectiveRepeat(int n, int w, int lost) {
    printf("\n--- Simulating Selective Repeat ARQ (Window Size = %d) ---\n", w);

    bool *ackStatus = (bool *)calloc(n, sizeof(bool));
    bool *sentStatus = (bool *)calloc(n, sizeof(bool));
    bool isLostHandled = false;
    int i = 0; // Window Base

    while (i < n) {
        int windowEnd = (i + w < n) ? (i + w) : n;
        printf("\n--- Current Window Base: Frame %d (Span: [", i);
        for (int k = i; k < windowEnd; k++) {
            printf("%d%s", k, (k == windowEnd - 1) ? "" : ", ");
        }
        printf("]) ---\n");

        // 1. Transmit any unsent or unACKed frames within the active window span
        for (int j = i; j < windowEnd; j++) {
            if (!ackStatus[j]) {
                if (!sentStatus[j]) {
                    printf("Sender: Transmitting Frame %d...\n", j);
                    sentStatus[j] = true;
                } else if (j == lost && !isLostHandled) {
                    printf("Sender: Retransmitting ONLY Lost Frame %d...\n", j);
                }
            }
        }

        // 2. Simulate reception execution block
        for (int j = i; j < windowEnd; j++) {
            if (ackStatus[j]) continue;

            if (j == lost && !isLostHandled) {
                printf("[!] Frame %d lost in transit. Receiver returns NAK %d.\n", j, j);
                isLostHandled = true; 
            } else {
                printf("Receiver: Frame %d received. Buffering/Processing. Sending ACK %d.\n", j, j + 1);
                ackStatus[j] = true;
            }
        }

        // 3. Slide window forward dynamically over sequentially ACKed front frames
        while (i < n && ackStatus[i]) {
            printf("Sender: Base ACK %d received. Sliding window base forward.\n", i + 1);
            i++;
        }
    }

    free(ackStatus);
    free(sentStatus);
    printf("\nAll %d frames sent and acknowledged successfully!\n", n);
}
