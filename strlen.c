
int strlen(const char *str) {
    int n = 0;
    while (*str++)
        n++;
    return n;
}

