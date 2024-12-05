#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_ROWS 140
#define NUM_COLS 140

#define MATCH_PART1 "XMAS"
#define MATCH_PART2 "MAS"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(x[0]))

char puzzle[NUM_ROWS][NUM_COLS];

int read_puzzle(FILE *fp);
bool is_match(size_t i, size_t j, int dir_x, int dir_y, const char *const match);
int part1_num_matches_from(size_t i, size_t j);
int part2_is_x_mas(size_t i, size_t j);

int main(void)
{
    FILE *fp = fopen("input.txt", "r");
    if (!fp)
    {
        printf("Could not open input file\n");
        return 1;
    }

    if (read_puzzle(fp))
    {
        return 1;
    }

    int num_word = 0;
    int num_x = 0;
    for (size_t i = 0; i < NUM_ROWS; i++)
    {
        for (size_t j = 0; j < NUM_COLS; j++)
        {
            if (puzzle[i][j] == 'X')
            {
                num_word += part1_num_matches_from(i, j);
            }
            if (puzzle[i][j] == 'A')
            {
                num_x += part2_is_x_mas(i, j);
            }
        }
    }
    printf("Total number of 'XMAS' matches is %d\n", num_word);
    printf("Total number of 'X-MAS' matches is %d\n", num_x);

    fclose(fp);
    return 0;
}

int read_puzzle(FILE *fp)
{
    char line[NUM_COLS];
    int c;
    for (size_t i = 0; i < NUM_ROWS; i++)
    {
        if (feof(fp))
        {
            printf("EOF encountered before %d lines were read\n", NUM_ROWS);
            return 1;
        }
        if (fread(line, sizeof(char), ARRAY_SIZE(line), fp) != NUM_COLS)
        {
            printf("Failed to read %d characters from stream\n", NUM_COLS);
            return 1;
        }

        c = fgetc(fp);
        if (c != '\n')
        {
            printf("Did not find newline where expected\n");
            return 1;
        }

        strncpy(puzzle[i], line, NUM_COLS);
    }
    return 0;
}

bool is_match(size_t i, size_t j, int dir_x, int dir_y, const char *const match)
{
    if (i + (strlen(match) - 1) * dir_x >= NUM_COLS || i + (strlen(match) - 1) * dir_x < 0 ||
        j + (strlen(match) - 1) * dir_y >= NUM_ROWS || j + (strlen(match) - 1) * dir_y < 0)
    {
        return false;
    }

    char *test = malloc(sizeof(char) * strlen(match));
    for (size_t k = 0; k < strlen(match); k++)
    {
        test[k] = puzzle[i][j];
        i += dir_x;
        j += dir_y;
    }

    int res = strncmp(test, match, strlen(match));
    free(test);
    return res == 0;
}

int part1_num_matches_from(size_t i, size_t j)
{
    int count = 0;
    for (int dir_x = -1; dir_x <= 1; dir_x++)
    {
        for (int dir_y = -1; dir_y <= 1; dir_y++)
        {
            if (dir_x != 0 || dir_y != 0)
            {
                if (is_match(i, j, dir_x, dir_y, MATCH_PART1))
                {
                    count++;
                }
            }
        }
    }
    return count;
}

int part2_is_x_mas(size_t i, size_t j)
{
    if (i == 0 || i == NUM_COLS - 1 || j == 0 || j == NUM_ROWS - 1)
    {
        return 0;
    }

    // Match along the top-left to bottom-right diagonal in either direction
    bool match1 = (is_match(i - 1, j - 1, 1, 1, MATCH_PART2) || is_match(i + 1, j + 1, -1, -1, MATCH_PART2));

    // Match along the top-right to bottom-left diagonal in either direction
    bool match2 = (is_match(i + 1, j - 1, -1, 1, MATCH_PART2) || is_match(i - 1, j + 1, 1, -1, MATCH_PART2));

    return match1 && match2;
}