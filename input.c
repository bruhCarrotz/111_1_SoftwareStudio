#include <stdio.h>
#include <stdlib.h>

void func2(int num) {
    printf("This is func2 with num %d\n", num);
}

void func1(int num) {
    printf("This is func1 with num %d\n", num);
}

int main(int argc, char *argv[]) {
    int num = atoi(argv[1]);

    if (num == 1) {
        func1(num);
    } else if (num == 2) {
        func2(num);
    } else {
        func1(num);
        func2(num);
    }

    return 0;
}
