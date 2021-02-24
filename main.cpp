#include <bits/stdc++.h>
#include "benchmarker.h"

void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

void shakesort(int *arr, size_t array_size, loginfo_t *loginfo)
{
    int len = (int)array_size;
    bool notSorted = true;
    while (notSorted)
    {
        loginfo->comparacoes++;
        notSorted = false;
        for (int a = 0; a < len - 1; a++)
        {
            if (arr[a] > arr[a + 1])
            {
                swap(&arr[a], &arr[a + 1]);
                notSorted = true;
                loginfo->trocas++;
            }
            loginfo->comparacoes += 2;
        }

        loginfo->comparacoes++;
        if (!notSorted)
            break;
        notSorted = false;

        for (int a = len - 1; a > 0; a--)
        {
            if (arr[a - 1] > arr[a])
            {
                swap(&arr[a], &arr[a - 1]);
                loginfo->trocas++;
                notSorted = true;
            }
            loginfo->comparacoes += 2;
        }
    }
}

void combsort(int *arr, size_t array_size, loginfo_t *loginfo)
{
    static const double shrink_factor = 1.247330950103979;
    bool swapped = true;
    int gap = (int)array_size;

    while (gap > 1 || swapped)
    {
        if (gap > 1)
            gap = (int)floor(gap / shrink_factor);

        loginfo->comparacoes += 2;

        swapped = false;
        int i = 0;
        int j = i + gap;

        for (; j < (int)array_size; i++, j++)
        {
            if (arr[i] > arr[j])
            {
                swap(&arr[i], &arr[j]);
                swapped = true;
                loginfo->trocas++;
            }
            loginfo->comparacoes += 2;
        }
    }
}

void bubblesort(int *arr, size_t array_size, loginfo_t *loginfo)
{
    int i, j;
    bool trocou;
    int n = (int)array_size;

    for (i = 0; i < n - 1; i++)
    {
        loginfo->comparacoes++;
        trocou = false;
        for (j = 0; j < n - i - 1; j++)
        {
            loginfo->comparacoes++;
            if (arr[j] > arr[j + 1])
            {
                swap(&arr[j], &arr[j + 1]);
                trocou = true;
                loginfo->trocas++;
            }
            loginfo->comparacoes++;
        }
        loginfo->comparacoes++;
        if (trocou == false)
            break;
    }
}

int partition_randomizada(int *arr, int p, int q, loginfo_t *loginfo)
{
    swap(&arr[p + (rand() % (q - p + 1))], &arr[q]);
    loginfo->trocas++;

    int i = p - 1;
    for (int j = p; j <= q; j++)
    {
        if (arr[j] <= arr[q])
        {
            swap(&arr[++i], &arr[j]);
            loginfo->trocas++;
        }
        loginfo->comparacoes += 2;
    }

    return i;
}

void quicksort(int *arr, int p, int q, loginfo_t *loginfo)
{
    if (p < q)
    {
        loginfo->comparacoes++;
        int pivo = partition_randomizada(arr, p, q, loginfo);

        quicksort(arr, p, pivo - 1, loginfo);
        quicksort(arr, pivo + 1, q, loginfo);
    }
}

void quicksort(int *array, size_t array_size, loginfo_t *loginfo)
{
    quicksort(array, 0, (int)array_size - 1, loginfo);
}

int main()
{

    vector<void (*)(int *, size_t, loginfo_t *)> functions;
    vector<string> function_names = {"Bubblesort", "Combsort", "Shakesort", "Quicksort"};

    functions.push_back(&bubblesort);
    functions.push_back(&combsort);
    functions.push_back(&shakesort);
    functions.push_back(&quicksort);

    Benchmark b(functions, function_names);
    //b.set_verbose();
    b.test_functions();
    b.results_to_csv();

    return 0;
}
