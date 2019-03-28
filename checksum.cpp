#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void bprint_internal(unsigned long long int x) {
    if (x)
        bprint_internal(x>>1);
    else
        return;
    printf("%llu", x&1);
}

void bprint(unsigned long long int x) {
    bprint_internal(x);
    printf("\n");
}

int length(unsigned long long int x) {
    int res=0;
    while(x){
        x = x>>1;
        res += 1;
    }
    return res;
}

unsigned long long int checksum(unsigned long long int * mx) {
    int len_mx = length(*mx);
    if (len_mx%16) {
        *mx = *mx << (16-(len_mx%16));
    }
    unsigned long long int buf = 0;
    while (*mx) {
        buf += *mx & 65535;
        *mx = *mx >> 16;
    }
    while (length(buf) > 16) {
        buf = (buf & 65535) + (buf >> 16);
    }
    return (~buf) & 65535;
}

int main() {
    printf("// TestMode: 24781725487143\n");
    unsigned long long int m;
    printf("Please input the message: ");
    scanf("%llu", &m);

    printf(" --------------- \n");

    printf("Message M(x) is: ");bprint(m);
    printf("Checksum     is: ");bprint(checksum(&m));
    return 0;
}
