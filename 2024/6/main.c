#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define NUM_ROWS 130
#define NUM_COLS 130

typedef struct
{
    int x;
    int y;
} vec2;

typedef char grid[NUM_ROWS][NUM_COLS];

grid area_golden;

int read_input(FILE *fp, vec2 *out_start_pos, vec2 *out_start_dir);
bool map_patrol_path(grid *area, vec2 *start_pos, vec2 *start_dir);
int count_visited_squares(grid *area);
int find_valid_obstructions(grid *path, vec2 *start_pos, vec2 *start_dir);

int main(void)
{
    FILE *fp = fopen("input.txt", "r");
    if (!fp)
    {
        printf("Could not open input file\n");
        return 1;
    }

    vec2 start_pos, start_dir;
    if (read_input(fp, &start_pos, &start_dir))
    {
        fclose(fp);
        return 1;
    }
    fclose(fp);

    grid area_unobstructed;
    memcpy(&area_unobstructed, &area_golden, sizeof(grid));
    map_patrol_path(&area_unobstructed, &start_pos, &start_dir);
    printf("There are %d visited squares\n", count_visited_squares(&area_unobstructed));
    printf("There are %d valid obstruction placements\n",
           find_valid_obstructions(&area_unobstructed, &start_pos, &start_dir));

    return 0;
}

int read_input(FILE *fp, vec2 *out_start_pos, vec2 *out_start_dir)
{
    int c;
    for (int i = 0; i < NUM_ROWS; i++)
    {
        for (int j = 0; j < NUM_COLS; j++)
        {
            c = fgetc(fp);
            if (c == EOF)
            {
                printf("Unexpected EOF\n");
                return 1;
            }
            area_golden[i][j] = (char)c;

            if (c == '^' || c == '<' || c == '>' || c == 'v')
            {
                *out_start_pos = (vec2){j, i};
                switch (c)
                {
                case '^':
                    *out_start_dir = (vec2){0, -1};
                    break;
                case '<':
                    *out_start_dir = (vec2){-1, 0};
                    break;
                case '>':
                    *out_start_dir = (vec2){1, 0};
                    break;
                case 'v':
                    *out_start_dir = (vec2){0, 1};
                    break;
                }
            }
        }
        // Consume newline character
        c = fgetc(fp);
    }
    return 0;
}

bool map_patrol_path(grid *area, vec2 *start_pos, vec2 *start_dir)
{
    vec2 pos = *start_pos;
    vec2 dir = *start_dir;
    vec2 next;
    grid seen_left = {0};
    grid seen_right = {0};
    grid seen_up = {0};
    grid seen_down = {0};
    while (1)
    {
        (*area)[pos.y][pos.x] = 'X';

        next.x = pos.x + dir.x;
        next.y = pos.y + dir.y;

        if (next.x < 0 || next.x >= NUM_COLS || next.y < 0 || next.y >= NUM_ROWS)
        {
            // True: the guard successfully leaves the area
            return true;
        }

        while ((*area)[next.y][next.x] == '#')
        {
            if ((dir.x == -1 && dir.y == 0 && seen_left[next.y][next.x]++) ||
                (dir.x == 1 && dir.y == 0 && seen_right[next.y][next.x]++) ||
                (dir.x == 0 && dir.y == -1 && seen_up[next.y][next.x]++) ||
                (dir.x == 0 && dir.y == 1 && seen_down[next.y][next.x]++))
            {
                // False: the guard is stuck in a loop (we've run into the same obstacle from the same direction twice)
                return false;
            }

            // Rotate by -90 degrees
            int temp = dir.x;
            dir.x = -1 * dir.y;
            dir.y = temp;

            next.x = pos.x + dir.x;
            next.y = pos.y + dir.y;
        }

        pos.x = next.x;
        pos.y = next.y;
    }
}

int count_visited_squares(grid *area)
{
    int accum = 0;
    for (size_t i = 0; i < NUM_ROWS; i++)
    {
        for (size_t j = 0; j < NUM_COLS; j++)
        {
            if ((*area)[i][j] == 'X')
            {
                accum++;
            }
        }
    }
    return accum;
}

int find_valid_obstructions(grid *path, vec2 *start_pos, vec2 *start_dir)
{
    int accum = 0;
    grid test;
    for (size_t i = 0; i < NUM_ROWS; i++)
    {
        for (size_t j = 0; j < NUM_COLS; j++)
        {
            if ((*path)[i][j] == 'X' && (i != start_pos->y || j != start_pos->x))
            {
                memcpy(&test, &area_golden, sizeof(grid));
                test[i][j] = '#';
                if (!map_patrol_path(&test, start_pos, start_dir))
                {
                    accum++;
                }
            }
        }
    }
    return accum;
}