#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define NUM_REPORTS 1000
#define MAX_LEVELS 10

typedef struct
{
    int levels[MAX_LEVELS];
    size_t num_levels;
} report;

int get_next_report(FILE *fp, report *out_report);
bool is_safe_report(report *r, bool use_dampener);
bool check_level_pair(bool is_increasing, int current, int next);
void remove_level(report *old, report *new, size_t index);

int main(void)
{
    FILE *fp = fopen("input.txt", "r");
    if (!fp)
    {
        printf("Could not open input file\n");
        return 1;
    }

    report r;
    int safe_count = 0;
    for (size_t i = 0; i < NUM_REPORTS; i++)
    {
        if (get_next_report(fp, &r))
        {
            fclose(fp);
            return 1;
        }

        if (is_safe_report(&r, true)) 
        {
            safe_count++;
        }
    }
    printf("There are %d safe reports\n", safe_count);

    fclose(fp);
    return 0;
}

int get_next_report(FILE *fp, report *out_report)
{
    char *line = NULL;
    size_t line_len;
    if(getline(&line, &line_len, fp) < 0)
    {
        printf("Failed to get line: %s\n", strerror(errno));
        free(line);
        return 1;
    }

    char *token = strtok(line, " ");
    size_t i = 0;
    while (token)
    {
        if (i >= MAX_LEVELS) 
        {
            printf("Number of levels in '%s' would exceed %d\n", line, MAX_LEVELS);
            free(line);
            return 1;
        }
        out_report->levels[i++] = atoi(token);
        token = strtok(NULL, " ");
    }
    out_report->num_levels = i;

    free(line);
    return 0;
}

bool is_safe_report(report *r, bool use_dampener)
{
    bool is_increasing = (r->levels[0] < r->levels[1]);
    
    for (size_t i = 0; i < r->num_levels - 1; i++)
    {
        int current = r->levels[i];
        int next = r->levels[i+1];
        if (!check_level_pair(is_increasing, current, next))
        {
            if (!use_dampener) 
            {
                return false;
            }

            // There can be 3 ways to make the report safe.
            // 1. Remove index 0. This may change the direction of the sequence.
            // 2. Remove the i'th index.
            // 2. Remove the i+1'th index.
            // Try all 3, short circuit to returning true if any method succeeds.

            report remove_first;
            remove_level(r, &remove_first, 0);
            if (is_safe_report(&remove_first, false)) 
            {
                return true;
            }

            report remove_current;
            remove_level(r, &remove_current, i);
            if (is_safe_report(&remove_current, false)) 
            {
                return true;
            }

            report remove_next;
            remove_level(r, &remove_next, i + 1);
            if (is_safe_report(&remove_next, false)) 
            {
                return true;
            }

            return false;
        }
    }
    return true;
}

inline bool check_level_pair(bool is_increasing, int current, int next)
{
    int abs_diff = abs(current - next);
    return (abs_diff >= 1 && abs_diff <= 3 && ((is_increasing && current < next) || (!is_increasing && current > next)));
}

void remove_level(report *old, report *new, size_t index)
{
    memcpy(new, old, sizeof(report));
    new->num_levels--;
    for (size_t j = index; j < new->num_levels; j++) 
    {
        new->levels[j] = new->levels[j + 1];
    }
}
