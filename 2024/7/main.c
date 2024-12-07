#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_EQUATIONS 850
#define MAX_OPERANDS 16

#define OP_ADD 0b00
#define OP_MUL 0b01
#define OP_CONCAT 0b10
#define OP_INVAL 0b11

typedef struct
{
    uint64_t result;
    uint16_t operands[MAX_OPERANDS];
    size_t num_operands;
} equation;

int read_next_equation(FILE *fp, equation *out_eqn);
bool test_operands(equation *eqn, uint32_t ops);
bool test_all_permutations(equation *eqn);
void do_concat(uint64_t *accum, uint16_t operand);

int main(void)
{
    FILE *fp = fopen("input.txt", "r");
    if (!fp)
    {
        printf("Could not open input file\n");
        return 1;
    }

    equation eqn;
    uint64_t accum = 0;
    for (size_t i = 0; i < NUM_EQUATIONS; i++)
    {
        if (read_next_equation(fp, &eqn))
        {
            fclose(fp);
            return 1;
        }

        if (test_all_permutations(&eqn))
        {
            accum += eqn.result;
        }
    }
    printf("%lu\n", accum);

    fclose(fp);
    return 0;
}

int read_next_equation(FILE *fp, equation *out_eqn)
{
    char *line = NULL;
    size_t line_len;
    if (getline(&line, &line_len, fp) < 0)
    {
        printf("Failed to get line: %d %s\n", errno, strerror(errno));
        free(line);
        return 1;
    }

    char *token = strtok(line, ": ");
    out_eqn->result = atoll(token);
    token = strtok(NULL, " ");
    size_t i = 0;
    while (token)
    {
        if (i >= MAX_OPERANDS)
        {
            printf("Number of operands in '%s' would exceed %d\n", line, MAX_OPERANDS);
            free(line);
            return 1;
        }
        out_eqn->operands[i++] = atoi(token);
        token = strtok(NULL, " ");
    }
    out_eqn->num_operands = i;

    free(line);
    return 0;
}

bool test_operands(equation *eqn, uint32_t ops)
{
    uint64_t accum = eqn->operands[0];
    for (size_t i = 1; i < eqn->num_operands; i++)
    {
        if (accum > eqn->result)
        {
            return false;
        }

        switch (ops & 0b11)
        {
        case OP_ADD:
            accum += eqn->operands[i];
            break;
        case OP_MUL:
            accum *= eqn->operands[i];
            break;
        case OP_CONCAT:
            do_concat(&accum, eqn->operands[i]);
            break;
        default:
            return false;
        }

        ops >>= 2;
    }

    return accum == eqn->result;
}

bool test_all_permutations(equation *eqn)
{
    // For n operands, there are (n-1) operators and thus 3^(n-1) permutations.
    // However it's easier to consider a 4th 'invalid' operation and iterate through 4^(n-1) = 2^2(n-1) permutations.
    const uint32_t end = 1 << 2 * (eqn->num_operands - 1);

    for (uint32_t ops = 0; ops < end; ops++)
    {
        if (test_operands(eqn, ops))
        {
            return true;
        }
    }

    return false;
}

void do_concat(uint64_t *accum, uint16_t operand)
{
    uint32_t pow = 10;
    while (pow <= operand)
    {
        pow *= 10;
    }
    *accum = *accum * pow + operand;
}
