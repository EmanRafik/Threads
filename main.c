#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

int matrix1[100][100]; //first matrix for multiplication
int matrix2[100][100]; //second matrix for multiplication
int outputMatrix[100][100]; //result matrix from multiplication
int matrix1_rows,matrix1_columns,matrix2_rows, matrix2_columns;

int array[100]; //array to be sorted
int array_size=0;

/**structure data type for storing boundaries of array/position of element in maytrix**/
struct Indecises
{
    int i,j;
};

void sorting();
void matrixMultiplication();

void *multiplicationThread_v1(void *arg);
void *multiplicationThread_v2(void *arg);
void *sortThread(void *arg);

void merge(int first, int mid, int last);
void mergeSort(int first, int last);

void matrixMultiplication_readInput();
void matrixMultiplication_printOutput(double time);

void sorting_readInput();
void sorting_printOutput();

int main()
{
    sorting();
    matrixMultiplication();
}

/**Main function fo merge sort problem**/
void sorting()
{
    sorting_readInput();
    if (array_size < 0) //negative array size
    {
        printf("Error sorting !");
        exit(0);
    }
    mergeSort(0,array_size-1);
    sorting_printOutput();
}

/**Main function of matrix multiplication
Implementing two diffreent versions using threads
Calculating the elapsed time for each version**/
void matrixMultiplication()
{
    FILE *fclear;
    fclear = fopen("output.txt","w");
    fclose(fclear);

    matrixMultiplication_readInput();

    if (matrix1_rows <= 0 || matrix1_columns <= 0 || matrix2_rows <= 0 || matrix2_columns <= 0) //invalid dimensions
    {
        printf("Error multiplying !");
        exit(0);
    }
    if (matrix1_columns != matrix2_rows) //check columns and rows are compatable for multiplication
    {
        printf("Error multiplying !");
        exit(0);
    }

    /**Version1: Compute each element in the output matrix in a single thread**/
    clock_t c = clock();
    int threads_num = matrix1_rows*matrix2_columns;
    int threads_count = 0;
    pthread_t threads_v1[threads_num];
    for (int i = 0; i < matrix1_rows; i++)
    {
        for (int j = 0; j < matrix2_columns; j++)
        {
            struct Indecises *index = (struct Indecises *)malloc(sizeof(struct Indecises));
            (*index).i = i;
            (*index).j = j;
            int c1 = pthread_create(&threads_v1[threads_count++], NULL,multiplicationThread_v1,(void *)index);
            if (c1)
            {
                printf("Error creating thread !");
            }
        }
    }
    for (int i = 0; i < threads_num; i++)
    {
        pthread_join(threads_v1[i], NULL);
    }
    c = clock() - c;
    double elapsed = (double)c/CLOCKS_PER_SEC; //elapsed time in seconds
    matrixMultiplication_printOutput(elapsed);

    /**Version2: Compute each row in the output matrix in a single thread**/
    c = clock();
    threads_num = matrix1_rows;
    threads_count = 0;
    pthread_t thread_v2[threads_num];
    for (int i = 0; i < matrix1_rows; i++)
    {
        int c2 = pthread_create(&thread_v2[threads_count++], NULL,multiplicationThread_v2,(void*)&i);
        if (c2)
        {
            printf("Error creating thread !");
        }
    }
    for (int i = 0; i < threads_num; i++)
    {
        pthread_join(thread_v2[i],NULL);
    }
    c = clock() - c;
    elapsed = (double)c/CLOCKS_PER_SEC; //elapsed time in seconds
    matrixMultiplication_printOutput(elapsed);
}

/**Implementing merge sort using thread for each sub-array**/
void mergeSort(int first, int last)
{
    if (first < last)
    {
        int mid = (first+last)/2;
        pthread_t id_1,id_2;
        struct Indecises *bounds_1 = (struct Indecises *)malloc(sizeof(struct Indecises));
        (*bounds_1).i = first;
        (*bounds_1).j = mid;
        int c1 = pthread_create(&id_1, NULL,sortThread,(void *)bounds_1);
        if (c1)
        {
            printf("Error creating thread !");
        }
        struct Indecises *bounds_2 = (struct Indecises *)malloc(sizeof(struct Indecises));
        (*bounds_2).i = mid+1;;
        (*bounds_2).j = last;
        int c2 = pthread_create(&id_2, NULL,sortThread,(void *)bounds_2);
        if (c2)
        {
            printf("Error creating thread !");
        }
        pthread_join(id_1,NULL);
        pthread_join(id_2,NULL);
        merge(first, mid, last);
    }
}

/**Merging two sorted sub-arrays**/
void merge(int first, int mid, int last)
{
    int n1 = mid-first+1;
    int n2 = last-mid;
    int a1[n1];
    int a2[n2];
    for (int i = 0; i < n1; i++)
    {
        a1[i] = array[first+i];
    }
    for (int i = 0; i < n2; i++)
    {
        a2[i] = array[mid+1+i];
    }
    int i = 0;
    int j = 0;
    int k = first;
    while (i < n1 && j < n2)
    {
        if (a1[i] < a2[j])
        {
            array[k] = a1[i];
            k++;
            i++;
        }
        else
        {
            array[k] = a2[j];
            k++;
            j++;
        }
    }
    while (i < n1)
    {
        array[k] = a1[i];
        k++;
        i++;
    }
    while (j < n2)
    {
        array[k] = a2[j];
        k++;
        j++;
    }
}

void *sortThread(void *arg)
{
    struct Indecises *bounds = (struct Indecises *)arg;
    mergeSort((*bounds).i,(*bounds).j);
}

void *multiplicationThread_v1(void *arg)
{
    struct Indecises *index = (struct Indecises*)arg;
    int ans=0;
    int i = (*index).i;
    int j = (*index).j;
    for (int t = 0; t < matrix1_columns; t++)
    {
        ans+=matrix1[i][t]*matrix2[t][j];
    }
    outputMatrix[i][j]=ans;
}

void *multiplicationThread_v2(void *arg)
{
    int *i = (int*) arg;
    for (int s = 0; s < matrix2_columns; s++)
    {
        int ans = 0;
        for (int t = 0; t < matrix1_columns; t++)
        {
            ans+=matrix1[*i][t]*matrix2[t][s];
        }
        outputMatrix[*i][s]=ans;
    }
}

/**read input file for matrix multiplication problem**/
void matrixMultiplication_readInput()
{
    FILE *f;
    f = fopen("input.txt", "r");
    fscanf(f, "%d %d",&matrix1_rows,&matrix1_columns);
    for (int i = 0; i < matrix1_rows; i++)
    {
        for (int j = 0; j < matrix1_columns; j++)
        {
            fscanf(f,"%d",&matrix1[i][j]);
        }
    }
    fscanf(f, "%d %d", &matrix2_rows,&matrix2_columns);
    for (int i = 0; i < matrix2_rows; i++)
    {
        for (int j = 0; j < matrix2_columns; j++)
        {
            fscanf(f,"%d",&matrix2[i][j]);
        }
    }
    fclose(f);
}

/**Output file for matrix multiplication problem**/
void matrixMultiplication_printOutput(double time)
{
    FILE *f;
    f = fopen("output.txt","a");
    for (int i = 0; i < matrix1_rows; i++)
    {
        for (int j = 0; j < matrix2_columns; j++)
        {
            fprintf(f, "%d ",outputMatrix[i][j]);
        }
        fprintf(f,"\n");
    }
    fprintf(f, "Elapsed time: %lf\n", time);
    fclose(f);
}

/**Reading input file for merge sort problem**/
void sorting_readInput()
{
    FILE *f;
    f = fopen("sorting_input.txt", "r");
    fscanf(f,"%d",&array_size);
    for (int i = 0; i < array_size; i++)
    {
        fscanf(f,"%d",&array[i]);
    }
    fclose(f);
}

/**Output file of merge sort problem**/
void sorting_printOutput()
{
    FILE *f;
    f = fopen("sorting_output.txt", "w");
    for (int i = 0; i < array_size; i++)
    {
        fprintf(f,"%d ",array[i]);
    }
    fclose(f);
}
