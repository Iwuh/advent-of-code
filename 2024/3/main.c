#include "fsm.h"

int main(void)
{
    FILE *fp = fopen("input.txt", "r");
    if (!fp)
    {
        printf("Could not open input file\n");
        return 1;
    }

    long long accum = 0;
    fsm f = fsm_init(fp);
    while (!fsm_next(&f))
    {
        if (f.result_ready)
        {
            printf("mul(%d,%d)\n", f.op1, f.op2);
            accum += f.result;
        }
    }
    printf("The sum of multiplications is %lld\n", accum);

    fclose(fp);
    return 0;
}