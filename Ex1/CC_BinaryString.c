#include <stdio.h>
#include <string.h>

int main() {
    int t;
    scanf("%d", &t);
    while (t--) {
        char A[101], B[101];
        scanf("%s%s", A, B);

        int a_count = 0, b_count = 0, common = 0;
        for (int i = 0; i < strlen(A); i++) {
            if (A[i] == '1') {
                a_count++;
            }
        }

        for (int i = 0; i < strlen(B); i++) {
            if (B[i] == '1') {
                b_count++;
            }
        }

        for (int i = 0; i < strlen(A); i++) {
            if (A[i] == B[i]) {
                common++;
            }
        }

        printf("%d %d %d\n", a_count, b_count, common);
    }
    return 0;
}
