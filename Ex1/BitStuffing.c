#include <stdio.h>
#define MAX 100
int data[MAX], stuffed[MAX], destuffed[MAX];
int flag[] = {0,1,1,1,1,1,1,0};
int flagLen = 8;
void printBits(char *label, int arr[], int n) {
    printf("%s : ", label);
    int i;
    for(i=0;i<n;i++)
        printf("%d", arr[i]);
    printf("\n");
}
int main() {
    int n,i;
    int ones = 0;
    int j = 0;
    printf("Enter number of bits : ");
    scanf("%d",&n);
    printf("Enter bits (0/1):\n");
    for(i=0;i<n;i++)
        scanf("%d",&data[i]);
    printBits("Original Data", data, n);
    for(i=0;i<n;i++) {
        stuffed[j++] = data[i];
        if(data[i] == 1)
            ones++;
        else
            ones = 0;
        if(ones == 5) {
            stuffed[j++] = 0;
            ones = 0;
        }
    }
    int stuffedLen = j;
    int framed[MAX];
    int k = 0;
    for(i=0;i<flagLen;i++)
        framed[k++] = flag[i];
    for(i=0;i<stuffedLen;i++)
        framed[k++] = stuffed[i];
    for(i=0;i<flagLen;i++)
        framed[k++] = flag[i];
    int framedLen = k;
    printBits("Stuffed Data", stuffed, stuffedLen);
    printBits("Framed Data", framed, framedLen);
    ones = 0;
    j = 0;
    for(i=flagLen; i<framedLen-flagLen; i++) {
        if(framed[i] == 1)
            ones++;
        else
            ones = 0;
        destuffed[j++] = framed[i];
        if(ones == 5) {
            i++;
            if(i >= framedLen-flagLen || framed[i] != 0) {
                printf("\nError: Invalid Stuffing\n");
                return 0;
            }
            ones = 0;
        }
    }
    printBits("Destuffed Data", destuffed, j);
    return 0;
}
