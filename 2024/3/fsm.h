#ifndef FSM_H
#define FSM_H

#include <stdbool.h>
#include <stdio.h>

typedef enum
{
    FIND_INSTRUCTION,
    FIND_OPEN_PAREN,
    FIND_OP1,
    FIND_COMMA,
    FIND_OP2,
    FIND_CLOSE_PAREN,
} fsm_state;

typedef enum
{
    MUL,
    DO,
    DONT,
    END,
} instruction;

typedef struct
{
    FILE *fp;
    fsm_state state;
    int op1;
    int op2;
    bool enabled;
    bool result_ready;
    int result;

} fsm;

fsm fsm_init(FILE *fp);
int fsm_next(fsm *f);

#endif