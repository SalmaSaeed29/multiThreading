#include <stdio.h>
#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

int rowA;
int colA;
int rowB;
int colB;
char globalfile[1000];
int matA[1000][1000];
int matB[1000][1000];
int matC[1000][1000];
char args[1000];
char dest[10];

struct cell{
    int row;
    int col;
};

void scanFileToMat(FILE *fptr, int mat[1000][1000], int row, int col)
{
    for(int i =0 ; i < row ; i++)
        for(int j = 0 ; j < col ; j++ )
            fscanf(fptr, "%d", &mat[i][j]);  //scan the matrix from file into the 2D matrix
}

void read(char path[1000], int num){
    int *row, *col, *mat;
    if(num == 1){
     row = &rowA; col = &colA; mat = &matA;
    }
    else if(num == 2){
        row = &rowB; col = &colB; mat = &matB;
    }
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        printf("Can't open the file.\n");
        exit(-1);
    }
    fscanf(file, "%[^\n]", args); // scan the first line
    if(sscanf(args, "row=%d col=%d", row, col) != 2 ) // 2--> number of variables filled
    {
        printf("Invalid first line format");
        exit(1);
    }
    /// reading value from the file.
    scanFileToMat(file, mat, *row, *col);
    fclose(file);
}

/// A thread per matrix
void method1(){
    for( int i = 0 ; i < rowA ; i++ )
    {
        for( int j = 0 ; j < colB ; j++ )
        {
            for( int k = 0 ; k < colA ; k++ )
            {
                matC[i][j] += matA[i][k]*matB[k][j];
            }
        }
    }
    //pthread_exit(NULL);
}

/// A thread per row
void method2(void* row){
    for( int j = 0 ; j < colB ; j++ )
    {
        matC[(int) row][j] = 0;
        for( int k = 0 ; k < colA ; k++ )
        {
            matC[(int) row][j] += matA[(int) row][k]*matB[k][j];
        }
    }
    pthread_exit(NULL);
}

//thread method 2
void thread_method2(){
    int id = 0;
    pthread_t m2[rowA]; // thread for each row
    for(int i = 0; i < rowA; i++) {
        //creating thread for row
       id = pthread_create(&m2[i], NULL, method2, (void*)i); //i: arg is passed of the start routine "method2"
        if(id != 0) {
            printf("ERROR!!!\n");
            exit(-1);
        }
    }
    for(int i = 0; i < rowA; i++) {
        pthread_join(m2[i], NULL);
    }
}

void method3(void *thing){
    struct cell *value = (struct cell *) thing;
    int i = value->row;
    int j = value->col;
    matC[i][j] = 0;
    for(int l = 0;l < colA; l++){
        matC[i][j] += matA[i][l]*matB[l][j];
    }
    pthread_exit(NULL);
}
void thread_method3(){
    int id;
    pthread_t m3[rowA*colB];
    int k = 0;
    for(int i = 0; i < rowA; i++){
        for(int j = 0; j < colB; j++){
            struct cell *temp = malloc(sizeof(struct cell));
            temp->row = i;
            temp->col = j;
            id = pthread_create(&m3[k++], NULL ,method3 ,(void*)temp);
            if(id != 0){
                printf("ERRROR!\n");
                exit(-1);
            }
        }
    }
    for(int t = 0; t < rowA*colB ; t++)
    {
        pthread_join(m3[t], NULL);
    }
}

//A function to print Matrix
void printing(int mat[1000][1000], int row, int col){
    for(int i =0; i < row; i++)
    {
        for(int j = 0; j < col; j++ )
            printf("%d ", mat[i][j]);
        printf("\n");
    }
}

void saving(char *path){
    FILE *file;
    file = fopen(path, "w");
    if(file == NULL) {
        printf("failure in creating file.\n");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < rowA; i++){
        for(int j = 0; j < colB; j++){
            fprintf(file, "%d ", matC[i][j]);
        }
        fprintf(file, "\n");
    }
    fclose(file);
}

void excution1(){
    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time
    /// executing method 1
    method1();
    printf("Method: A thread per matrix\n");
    strcpy(globalfile, dest);
    saving(strcat(globalfile,"_per_matrix.txt"));
    printf("row=%d col=%d\n", rowA, colB);
    printing(matC, rowA, colB);
    gettimeofday(&stop, NULL); //end checking time
    printf("Number of Threads used: 1\n");
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    printf("-------------------------------------------------------------\n");
}

void excution2(){
    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time
    /// executing method 2
    thread_method2();
    printf("Method: A thread per row\n");
    strcpy(globalfile, dest);
    saving(strcat(globalfile,"_per_row.txt"));
    printf("row=%d col=%d\n", rowA, colB);
    printing(matC, rowA, colB);
    gettimeofday(&stop, NULL); //end checking time
    printf("Number of Threads used: %d\n",rowA);
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    printf("-------------------------------------------------------------\n");
}

void excution3(){
    struct timeval stop, start;
    gettimeofday(&start, NULL); //start checking time
    /// executing method
    thread_method3();
    printf("Method: A thread per element\n");
    strcpy(globalfile, dest);
    saving(strcat(globalfile,"_per_element.txt"));
    printf("row=%d col=%d\n", rowA, colB);
    printing(matC, rowA, colB);
    gettimeofday(&stop, NULL); //end checking time
    printf("Number of Threads used: %d\n",rowA*colB);
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
    printf("-------------------------------------------------------------\n");
}

excution(){
    if(colA != rowB)
    {
        printf("WARNING!!! not valid Dimensions\n");
        exit(3);
    }
    excution1();
    excution2();
    excution3();
}

setDefault(int argc, char *argv[]){
    //printing(matA, rowA, colA);

    if(argc == 1){
        strcpy(globalfile, ""); // clear file name
        strcat(globalfile, "a.txt");
        read(globalfile, 1);

        strcpy(globalfile, "");
        strcat(globalfile, "b.txt");
        read(globalfile, 2);

        strcpy(globalfile, "");
        strcat(globalfile, "c");
        strcpy(dest, "c");
    }
    else if(argc == 3){
        strcpy(globalfile, "");
        strcat(globalfile, argv[1]);
        strcat(globalfile, ".txt");
        read(globalfile, 1);

        strcpy(globalfile, "");
        strcat(globalfile, argv[2]);
        strcat(globalfile, ".txt");
        read(globalfile, 2);

        strcpy(globalfile, "");
        strcat(globalfile, "c");
        strcpy(dest, "c");
    }
    else if (argc == 4){
        strcpy(globalfile, "");
        strcat(globalfile, argv[1]);
        strcat(globalfile, ".txt");
        read(globalfile, 1);

        strcpy(globalfile, "");
        strcat(globalfile, argv[2]);
        strcat(globalfile, ".txt");
        read(globalfile, 2);

        strcpy(globalfile, "");
        strcat(globalfile, argv[3]);
        strcpy(dest, argv[3]);
    }
    else{
        printf("Invalid Arguments");
        exit(-1);
    }
}

int main(int argc, char *argv[]) {
    setDefault(argc, argv);
    excution();
    return 0;
}
