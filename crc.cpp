#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

unsigned long long int pow(int x) {
    unsigned long long int res = 1;
    return res << x;
}

unsigned long long int crc(unsigned long long int * mx, unsigned long long int * cx) {
    int len_mx = length(*mx);
    int len_cx = length(*cx);
    unsigned long long int * tx = (unsigned long long int *)malloc(len_cx+len_mx-1);
    unsigned long long int * buf = (unsigned long long int *)malloc(len_cx);
    *buf = 0;
    *tx = *mx << (len_cx-1);
    int i;
    for(i=len_cx+len_mx-1;i>=0;i--) {
        if((*tx>>i)&1) {
            *buf = *buf | 1;
        } else {
            *buf = *buf & (~1);
        }
        *buf = *buf & (pow(len_cx)-1);
        if(!(*buf>>(len_cx-1))) {
            *buf = *buf<<1;
        } else {
            *buf = *buf^*cx;
            *buf = *buf<<1;
        }
        *buf = *buf & (pow(len_cx)-1);
    }
    return *tx | (*buf>>1);
}

int main() {
    printf("// TestMode: 154 and 13\n");
    unsigned long long int m, c;
    printf("Please input the message: ");
    scanf("%llu", &m);
    printf("Please input the divisor: ");
    scanf("%llu", &c);

    printf(" --------------- \n");

    printf("Message M(x) is: ");bprint(m);
    printf("Divisor C(x) is: ");bprint(c);
    printf("        T(x) is: ");bprint(crc(&m, &c));
    return 0;
}
