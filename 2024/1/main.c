#include <stdio.h>
#include <stdlib.h>

#define LIST_MAX_LEN 1000

int get_lists(FILE *fp, int *list1, int *list2, size_t *out_len);
int compare_ints(const void *first, const void *second);
long compare_lists(int *list1, int *list2, size_t len);
long compare_similarity(int *list1, int *list2, size_t len);

int main(void)
{
    FILE *fp = fopen("input.txt", "r");
    if (!fp)
    {
        printf("Could not open input file\n");
        return 1;
    }

    int list1[LIST_MAX_LEN], list2[LIST_MAX_LEN];
    size_t list_len;
    if (get_lists(fp, list1, list2, &list_len))
    {
        return 1;
    }

    qsort(list1, list_len, sizeof(int), compare_ints);
    qsort(list2, list_len, sizeof(int), compare_ints);

    printf("The sum of differences is %ld\n", compare_lists(list1, list2, list_len));
    printf("The similarity score is %ld\n", compare_similarity(list1, list2, list_len));

    fclose(fp);

    return 0;
}

int get_lists(FILE *fp, int *list1, int *list2, size_t *out_len)
{
    size_t i = 0;
    while (!feof(fp))
    {
        if (i >= LIST_MAX_LEN) 
        {
            printf("Number of lines in file exceeded %d\n", LIST_MAX_LEN);
            return 1;
        }
        fscanf(fp, "%d%d\n", &list1[i], &list2[i]);
        i++;
    }
    *out_len = i;
    return 0;
}

int compare_ints(const void *first, const void *second)
{
    int a = *(int *)first;
    int b = *(int *)second;
    return (a > b) - (a < b);
}

long compare_lists(int *list1, int *list2, size_t len)
{
    long accum = 0;
    for (size_t i = 0; i < len; i++)
    {
        accum += labs(list1[i] - list2[i]);
    }
    return accum;
}

long compare_similarity(int *list1, int *list2, size_t len)
{
    long accum = 0;
    for (size_t i = 0; i < len; i++)
    {
        long appear_count = 0;
        size_t j = 0;
        while (list1[i] >= list2[j]) 
        {
            if (list1[i] == list2[j]) 
            {
                appear_count++;
            }
            j++;
        }
        accum += list1[i] * appear_count;
    }
    return accum;
}