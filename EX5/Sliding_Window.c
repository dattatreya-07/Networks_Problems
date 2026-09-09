#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Function Prototypes
void stopAndWait(int n, int lost);
void goBack(int n, int w, int lost);
void selectiveRepeat(int n, int w, int lost);

int main() {
    int choice;
    int n, w, lost;

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

        // Input gathering common to the protocols
        printf("Enter the total number of frames to send: ");
        scanf("%d", &n);

        if (choice == 2 || choice == 3) {
            printf("Enter the window size (W): ");
            scanf("%d", &w);
        }

        printf("Enter the frame number that gets lost/damaged (0 to %d, or -1 for no loss): ", n - 1);
        scanf("%d", &lost);

        // Execute the chosen protocol
        switch (choice) {
            case 1:
                stopAndWait(n, lost);
                break;
            case 2:
                goBack(n, w, lost);
                break;
            case 3:
                selectiveRepeat(n, w, lost);
                break;
        }
    }

    return 0;
}

/**
 * Simulates the Stop-and-Wait ARQ Protocol.
 * Window size is implicitly 1.
 */
void stopAndWait(int n, int lost) {
    printf("\n--- Simulating Stop-and-Wait ARQ ---\n");
    bool isLostHandled = false;

    for (int i = 0; i < n; i++) {
        printf("\nSender: Sending Frame %d...", i);

        // Simulate a frame loss event on the specified frame index
        if (i == lost && !isLostHandled) {
            printf("\n[!] Frame %d was LOST or DAMAGED in transit.", i);
            printf("\nReceiver: (No response due to timeout)");
            printf("\nSender: Timeout expired! Retransmitting Frame %d...", i);
            isLostHandled = true; // Mark as handled so retransmission succeeds
            i--;                  // Decrement loop counter to re-send this frame
            continue;
        }

        printf("\nReceiver: Frame %d received successfully.", i);
        printf("\nReceiver: Sending ACK %d...", i + 1);
        printf("\nSender: ACK %d received.", i + 1);
    }
    printf("\n\nAll %d frames sent and acknowledged successfully!\n", n);
}

/**
 * Simulates the Go-Back-N ARQ Protocol.
 * If a frame is lost, the entire current window from that point forward is retransmitted.
 */
void goBack(int n, int w, int lost) {
    printf("\n--- Simulating Go-Back-N ARQ (Window Size = %d) ---\n", w);

    int i = 0; // Tracks the next frame to be successfully acknowledged
    bool isLostHandled = false;

    while (i < n) {
        int windowEnd = (i + w < n) ? (i + w) : n;
        printf("\n--- Current Window: Frames [");
        for (int k = i; k < windowEnd; k++) {
            printf("%d%s", k, (k == windowEnd - 1) ? "" : ", ");
        }
        printf("] ---\n");

        bool triggerRetransmission = false;
        int firstFailure = -1;

        // Sender transmits the whole available window
        for (int j = i; j < windowEnd; j++) {
            printf("Sender: Sending Frame %d...\n", j);

            // Check if this specific frame triggers a simulated loss
            if (j == lost && !isLostHandled) {
                printf("[!] Frame %d encountered an error/loss.\n", j);
                triggerRetransmission = true;
                firstFailure = j;
                isLostHandled = true; // Prevent infinite simulation loss loops
                break; // GBN drops subsequent frames in the current pipeline burst
            }
        }

        if (triggerRetransmission) {
            // Receiver discards everything from the point of failure
            printf("Receiver: Discarded tracking. Timeout event triggered at Sender side.\n");
            printf("Sender: NAK received / Timeout for Frame %d. Going Back N!\n", firstFailure);
            // i remains unchanged, causing the loop to reset transmission from the failed frame
        } else {
            // All frames in the current window batch successfully made it through
            for (int j = i; j < windowEnd; j++) {
                printf("Receiver: Frame %d received. Sending ACK %d.\n", j, j + 1);
                printf("Sender: ACK %d received. Sliding window forward.\n", j + 1);
            }
            i = windowEnd; // Slide window completely past this successful block
        }
    }
    printf("\nAll %d frames sent and acknowledged successfully!\n", n);
}

/**
 * Simulates the Selective Repeat ARQ Protocol.
 * If a frame is lost, only that specific frame is retransmitted.
 * Subsequent frames are buffered by the receiver.
 */
void selectiveRepeat(int n, int w, int lost) {
    printf("\n--- Simulating Selective Repeat ARQ (Window Size = %d) ---\n", w);

    bool *ackStatus = (bool *)calloc(n, sizeof(bool));
    if (ackStatus == NULL) {
        printf("Memory allocation failed.\n");
        return;
    }

    int i = 0; // Left edge of the sender window
    bool isLostHandled = false;

    while (i < n) {
        int windowEnd = (i + w < n) ? (i + w) : n;
        printf("\n--- Current Window Base: Frame %d (Window Span: [", i);
        for (int k = i; k < windowEnd; k++) {
            printf("%d%s", k, (k == windowEnd - 1) ? "" : ", ");
        }
        printf("]) ---\n");

        // Step 1: Transmit un-ACKed frames currently inside the window
        for (int j = i; j < windowEnd; j++) {
            if (!ackStatus[j]) {
                printf("Sender: Transmitting Frame %d...\n", j);
            }
        }

        // Step 2: Simulate reception and ACK generation
        for (int j = i; j < windowEnd; j++) {
            if (ackStatus[j]) continue; // Skip if already successfully processed

            if (j == lost && !isLostHandled) {
                printf("[!] Frame %d was LOST. Receiver sends NAK %d.\n", j, j);
                isLostHandled = true;
                // Frame remains un-ACKed; out-of-order execution allows subsequent items to be processed
            } else {
                printf("Receiver: Frame %d received out-of-order/in-order. Buffering/Processing. Sending ACK %d.\n", j, j);
                ackStatus[j] = true;
            }
        }

        // Step 3: Slide the window past sequentially ACKed elements at the front edge
        while (i < n && ackStatus[i]) {
            printf("Sender: Core ACK %d acknowledged. Sliding window base forward.\n", i);
            i++;
        }
    }

    free(ackStatus);
    printf("\nAll %d frames sent and acknowledged successfully!\n", n);
}
