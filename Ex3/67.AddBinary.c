char* addBinary(char* a, char* b) {
    int i, j, carry = 0;
    int c = 0, c1 = 0;
    for(i = 0; a[i] != '\0'; i++)
        c++;
    for(i = 0; b[i] != '\0'; i++)
        c1++;
    char *r = (char*)malloc(10005);
    i = c - 1;
    j = c1 - 1;
    int in = 0;
    while(i >= 0 || j >= 0 || carry != 0) {
        int s = carry;
        if(i >= 0) {
            s = s + (a[i] - '0');
            i--;
        }
        if(j >= 0) {
            s = s + (b[j] - '0');
            j--;
        }
        r[in] = (s % 2) + '0';
        carry = s / 2;
        in++;
    }
    r[in] = '\0';
    i = 0;
    j = in - 1;
    while(i < j) {
        char t = r[i];
        r[i] = r[j];
        r[j] = t;
        i++;
        j--;
    }
    return r;
}
