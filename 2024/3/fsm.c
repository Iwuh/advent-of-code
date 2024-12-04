#include "fsm.h"
#include <ctype.h>
#include <string.h>

// static bool find_mul(FILE *fp)
// {
//     char *target = "mul";
//     int c;
//     while ((c = fgetc(fp)) != EOF)
//     {
//         if (c == *target)
//         {
//             target++;
//             if (*target == '\0')
//             {
//                 return true;
//             }
//         }
//         else
//         {
//             target = "mul";
//         }
//     }
//     return false;
// }

static instruction find_instruction(FILE *fp)
{
    int c;
    while ((c = fgetc(fp)) != EOF)
    {
        if (c == 'm')
        {
            char buf[2];
            if (fread(buf, sizeof(char), 2, fp) != 2)
            {
                return END;
            }
            if (strncmp(buf, "ul", strlen("ul")) == 0)
            {
                return MUL;
            }
            fseek(fp, -2, SEEK_CUR);
            continue;
        }
        if (c == 'd')
        {
            char buf[6];
            if (fread(buf, sizeof(char), 6, fp) != 6)
            {
                return END;
            }
            if (strncmp(buf, "o()", strlen("o()")) == 0)
            {
                fseek(fp, -6 + strlen("o()"), SEEK_CUR);
                return DO;
            }
            if (strncmp(buf, "on't()", strlen("on't()")) == 0)
            {
                return DONT;
            }
            fseek(fp, -6, SEEK_CUR);
            continue;
        }
    }
    return END;
}

static int process_instruction(fsm *f, instruction instr)
{
    switch (instr)
    {
    case MUL:
        f->state = FIND_OPEN_PAREN;
        break;
    case DO:
        f->enabled = true;
        break;
    case DONT:
        f->enabled = false;
        break;
    default:
        printf("Unknown instruction\n");
    case END:
        return 1;
    }
    return 0;
}

static bool find_char(FILE *fp, char match)
{
    int c = fgetc(fp);
    return c == match;
}

static bool find_op(FILE *fp, int *out_op)
{
    int op = 0;
    int c;
    for (size_t i = 0; i < 3; i++)
    {
        c = fgetc(fp);
        if (isdigit(c))
        {
            op *= 10;
            op += (c - '0');
        }
        else
        {
            fseek(fp, -1, SEEK_CUR);
            break;
        }
    }

    if (op == 0)
    {
        return false;
    }

    *out_op = op;
    return true;
}

fsm fsm_init(FILE *fp)
{
    fsm f = {
        .fp = fp, .state = FIND_INSTRUCTION, .op1 = 0, .op2 = 0, .enabled = true, .result_ready = false, .result = 0};

    return f;
}

int fsm_next(fsm *f)
{
    instruction instr;
    switch (f->state)
    {
    case FIND_INSTRUCTION:
        f->result_ready = false;
        instr = find_instruction(f->fp);
        if (process_instruction(f, instr))
        {
            return 1;
        }
        break;
    case FIND_OPEN_PAREN:
        if (find_char(f->fp, '('))
        {
            f->state = FIND_OP1;
        }
        else
        {
            f->state = FIND_INSTRUCTION;
        }
        break;
    case FIND_OP1:
        if (find_op(f->fp, &f->op1))
        {
            f->state = FIND_COMMA;
        }
        else
        {
            f->state = FIND_INSTRUCTION;
        }
        break;
    case FIND_COMMA:
        if (find_char(f->fp, ','))
        {
            f->state = FIND_OP2;
        }
        else
        {
            f->state = FIND_INSTRUCTION;
        }
        break;
    case FIND_OP2:
        if (find_op(f->fp, &f->op2))
        {
            f->state = FIND_CLOSE_PAREN;
        }
        else
        {
            f->state = FIND_INSTRUCTION;
        }
        break;
    case FIND_CLOSE_PAREN:
        if (find_char(f->fp, ')') && f->enabled)
        {
            f->result_ready = true;
            f->result = f->op1 * f->op2;
        }
        f->state = FIND_INSTRUCTION;
        break;
    default:
        printf("Unknown state\n");
        return 1;
    }
    return 0;
}