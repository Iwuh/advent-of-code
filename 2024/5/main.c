#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RELATION_SIZE 1176
#define NUM_JOBS 193
#define JOB_MAX_LEN 30

typedef struct
{
    int a;
    int b;
} ordered_pair;

typedef struct
{
    int pages[JOB_MAX_LEN];
    size_t num_pages;
} job;

ordered_pair relation[RELATION_SIZE];

int read_relation(FILE *fp);
int compare_ordered_pair(const void *first, const void *second);
int read_next_job(FILE *fp, job *out_job);
bool is_related(int a, int b);
bool is_valid_job(job *j);
void sort_job(job *j);

int main(void)
{
    FILE *fp = fopen("input.txt", "r");
    if (!fp)
    {
        printf("Could not open input file\n");
        return 1;
    }

    if (read_relation(fp))
    {
        fclose(fp);
        return 1;
    }

    int accum_valid = 0;
    int accum_sort = 0;
    job j;
    for (size_t i = 0; i < NUM_JOBS; i++)
    {
        if (read_next_job(fp, &j))
        {
            fclose(fp);
            return 1;
        }

        if (is_valid_job(&j))
        {
            accum_valid += j.pages[(j.num_pages - 1) / 2];
        }
        else
        {
            sort_job(&j);
            if (is_valid_job(&j))
            {
                accum_sort += j.pages[(j.num_pages - 1) / 2];
            }
        }
    }
    printf("The sum of middle values from initially-valid jobs is %d\n", accum_valid);
    printf("The sum of middle values from sorted jobs is %d\n", accum_sort);

    fclose(fp);
    return 0;
}

int read_relation(FILE *fp)
{
    for (size_t i = 0; i < RELATION_SIZE; i++)
    {
        if (fscanf(fp, "%d|%d\n", &relation[i].a, &relation[i].b) != 2)
        {
            printf("Failed to read expected format at line %ld\n", i + 1);
        };
    }
    // Sort ascending so we can do a binary search later
    qsort(relation, RELATION_SIZE, sizeof(ordered_pair), compare_ordered_pair);
    return 0;
}

int compare_ordered_pair(const void *first, const void *second)
{
    int a1 = ((ordered_pair *)first)->a;
    int a2 = ((ordered_pair *)second)->a;
    return (a1 > a2) - (a1 < a2);
}

int read_next_job(FILE *fp, job *out_job)
{
    char *line = NULL;
    size_t line_len;
    if (getline(&line, &line_len, fp) < 0)
    {
        printf("Failed to get line: %d %s\n", errno, strerror(errno));
        free(line);
        return 1;
    }

    char *token = strtok(line, ",");
    size_t i = 0;
    while (token)
    {
        if (i >= JOB_MAX_LEN)
        {
            printf("Number of pages in '%s' would exceed %d\n", line, JOB_MAX_LEN);
            free(line);
            return 1;
        }
        out_job->pages[i++] = atoi(token);
        token = strtok(NULL, ",");
    }
    out_job->num_pages = i;

    free(line);
    return 0;
}

bool is_related(int a, int b)
{
    // Find any relation with matching "a" using a binary search
    size_t first = 0;
    size_t last = RELATION_SIZE - 1;
    size_t mid;
    do
    {
        mid = (first + last) / 2;
        if (relation[mid].a > a)
        {
            last = mid;
        }
        else if (relation[mid].a < a)
        {
            first = mid;
        }
    } while (relation[mid].a != a);

    // Search in both directions for a matching "b". If found, a is related to b.
    for (size_t i = mid; i-- > 0 && relation[i].a == a;)
    {
        if (relation[i].b == b)
        {
            return true;
        }
    }
    for (size_t i = mid; i < RELATION_SIZE && relation[i].a == a; i++)
    {
        if (relation[i].b == b)
        {
            return true;
        }
    }

    return false;
}

bool is_valid_job(job *test)
{
    for (size_t i = 0; i < test->num_pages; i++)
    {
        printf("%d ", test->pages[i]);
    }
    printf("\n");
    for (size_t i = test->num_pages; i-- > 0;)
    {
        for (size_t j = 0; j < i; j++)
        {
            if (is_related(test->pages[i], test->pages[j]))
            {
                printf("Invalid because %d|%d\n", test->pages[i], test->pages[j]);
                return false;
            }
        }
    }
    printf("Valid\n");
    return true;
}

void sort_job(job *job)
{
    // We're just going to assume that the subset of our relation is acyclic and transitive...
    bool done = false;
    while (!done)
    {
        done = true;
        for (size_t i = 1; i < job->num_pages; i++)
        {
            for (size_t j = i; j-- > 0;)
            {
                if (is_related(job->pages[i], job->pages[j]))
                {
                    done = false;
                    int temp = job->pages[i];
                    job->pages[i] = job->pages[j];
                    job->pages[j] = temp;
                }
            }
        }
    }
}