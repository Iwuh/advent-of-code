#include <stdint.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))

char* WORDS[] = {
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine"
};

int get_num(FILE* fp);

int main(void)
{
    FILE* fp = fopen("input.txt", "r");
    if (!fp)
    {
        printf("Could not open file\n");
        return 1;
    }

    int sum = 0;
    while (!feof(fp))
    {
        sum += get_num(fp);
    }

    printf("The sum is %d\n", sum);
}

int get_num(FILE* fp)
{
    int first_val, last_val;
    size_t first_idx = SIZE_MAX, last_idx = SIZE_MAX;
    size_t tmp_idx;
    size_t word_len;
    char line[256];
    char *tmp, *next;

    fgets(line, 256, fp);
    printf("%s", line);

    // Process text digits ("one", "two", etc...) first.
    for (size_t i = 0; i < ARRAY_SIZE(WORDS); i++) 
    {
        next = line;
        word_len = strlen(WORDS[i]);
        while ((tmp = strstr(next, WORDS[i]))) 
        {
            tmp_idx = tmp - line;
            
            if (first_idx == SIZE_MAX || tmp_idx < first_idx)
            {
                // We may find the first digit out of order, eg. "two5one"
                // If a last digit hasn't been found yet and we replace the first digit, make the replaced digit the last digit.
                if (last_idx == SIZE_MAX)
                {
                    last_idx = first_idx;
                    last_val = first_val;
                }
                first_idx = tmp_idx;
                first_val = i + 1;
            }
            else if (last_idx == SIZE_MAX || tmp_idx > last_idx)
            {
                last_idx = tmp_idx;
                last_val = i + 1;
            }
            next = tmp + word_len;
        }
    }

    // Repeat the above process but for numeric digits instead.
    tmp_idx = 0;
    while (line[tmp_idx])
    {
        char c = line[tmp_idx];

        if (isdigit(c)) 
        {
            if (first_idx == SIZE_MAX || tmp_idx < first_idx)
            {
                if (last_idx == SIZE_MAX)
                {
                    last_idx = first_idx;
                    last_val = first_val;
                }
                first_idx = tmp_idx;
                first_val = (char)c - '0';
            }
            else if (last_idx == SIZE_MAX|| tmp_idx > last_idx)
            {
                last_idx = tmp_idx;
                last_val = (char)c - '0';
            }
        }

        tmp_idx++;
    }

    if (first_idx == SIZE_MAX && last_idx == SIZE_MAX)
    {
        return 0;
    }
    else if (last_idx == SIZE_MAX) 
    {
        last_val = first_val;
    }

    printf("%d\n", first_val * 10 + last_val);
    return first_val * 10 + last_val;
}
