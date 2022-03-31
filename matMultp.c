#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <sys/time.h>


//////////////Global Variables//////////////////////////

int matrix1[1000][1000];
int matrix2[1000][1000];
int matrixOut[1000][1000];
int row1,row2,col1,col2;
struct rowCol{
    int row;
    int col;
};
char dims[1000];
char fileName[1000],outputFile[1000];

/////////////////Read Files//////////////////////////////////////////////////


void matrixElements(FILE *file, int matrix[1000][1000], int row, int col ) {//Read each element in the matrix
    for (int i = 0; i < row; i++){
        for (int j = 0; j < col; j++){
            fscanf(file, "%d", &matrix[i][j]);
        }
    }
}


void readFile( char* fName, int flag ){
    int *row, *col, *matrix;
    if( flag==0 ){//First matrix.
        row = &row1;
        col = &col1;
        matrix = matrix1;
    }else{//Second Matrix
        row = &row2;
        col = &col2;
        matrix = matrix2;
    }

    FILE *f = fopen( fName, "r" );//Read the file
    if (f==NULL){
        printf("File doesn't exist!!\n");
        exit(-1);
    }

    fscanf(f, "%[^\n]",dims);
    if(  sscanf(dims, "row=%d col=%d", row,col) != 2 ){//Check if the first line that contain the dims exists.
        exit(-1);
    }

    matrixElements(f,matrix,*row,*col);//Read the elements of this matrix.

    fclose(f);

}


//////////////////Save Output Matrices//////////////////////////////////////


void save1(){

    //Saving the output matrix using 1 thread per matrix in the required file.
    strcpy(outputFile, "");
    strcat(outputFile, fileName);
    strcat(outputFile, "_per_matrix.txt");

    FILE *fil = fopen(outputFile, "w");

    fprintf(fil,"Method: A thread per matrix\n");
    fprintf(fil,"row=%d col=%d\n",row1,col2);
    for(int i=0;i<row1;i++){
        for(int j=0;j<col2;j++){
            fprintf(fil,"%d ",matrixOut[i][j]);
        }
        fprintf(fil,"\n");
    }

}


void save2(){

    //Saving the output matrix using 1 thread per row in the required file.
    strcpy(outputFile, "");
    strcat(outputFile, fileName);
    strcat(outputFile, "_per_row.txt");

    FILE *fil =fopen(outputFile, "w");

    fprintf(fil,"Method: A thread per row\n");
    fprintf(fil,"row=%d col=%d\n",row1,col2);
    for(int i=0;i<row1;i++){
        for(int j=0;j<col2;j++){
            fprintf(fil,"%d ",matrixOut[i][j]);
        }
        fprintf(fil,"\n");
    }
}


void save3(){

    //Saving the output matrix using 1 thread per element in the required file.
    strcpy(outputFile, "");
    strcat(outputFile, fileName);
    strcat(outputFile, "_per_element.txt");

    FILE *fil =fopen(outputFile, "w");

    fprintf(fil,"Method: A thread per element\n");
    fprintf(fil,"row=%d col=%d\n",row1,col2);
    for(int i=0;i<row1;i++){
        for(int j=0;j<col2;j++){
            fprintf(fil,"%d ",matrixOut[i][j]);
        }
        fprintf(fil,"\n");
    }
}



/////////////////Matrix Multiplication Different Cases/////////////////////


//First Method: 1 Thread per matrix
void *matrixMultCase1(){
    for(int i=0;i<row1;i++){
        for(int j=0;j<col2;j++){
            matrixOut[i][j]=0;
            for(int k=0;k<row2;k++){
                matrixOut[i][j] += matrix1[i][k]*matrix2[k][j];
            }
        }
    }
}



//Second Method: 1 Thread per row.
void* matrixMultCase2(void* rowInd){

    long k=(long)rowInd;
    for(int i=0;i<col2;i++){
        matrixOut[k][i]=0;
        for(int j=0;j<row2;j++){
            matrixOut[k][i] += matrix1[k][j]*matrix2[j][i];
        }
    }
    pthread_exit(NULL);//Terminate calling thread
}


//Creating threads for the matrix.
void mat2(){

    int tid;
    pthread_t thread[row1];

    for(int i=0;i<row1;i++) {
        tid = pthread_create(&thread[i], NULL, matrixMultCase2, (void *) i);
        if (tid != 0) {//Error Happened.
            printf("ERROR!!\n");
            exit(-1);
        }
    }

    for(int i=0;i<row1;i++) {
        pthread_join(thread[i], NULL);//Waiting for each thread to terminate.
    }
}



//1 Thread per element
void* matrixMultCase3(void* index){

    struct rowCol *ind;
    ind = (struct rowCol *) index;
    int r=ind->row;
    int c=ind->col;

    matrixOut[r][c]=0;
    for(int i=0;i<row2;i++){
        matrixOut[r][c] += matrix1[r][i]*matrix2[i][c];
    }

    pthread_exit(NULL);//Terminate calling thread.
}


//Creating threads for the matrix.
void mat3(){

    int tid;
    pthread_t thread[row1*col2];

    int s=0;
    for(int i=0;i<row1;i++) {
        for (int j=0;j<col2; j++) {

            struct rowCol *indices=malloc(sizeof(struct rowCol));
            indices->row=i;
            indices->col=j;

            tid = pthread_create(&thread[s], NULL, matrixMultCase3,(void *)indices);
            s++;

            if (tid != 0) {//Error Happened.
                printf("ERROR!!\n");
                exit(-1);
            }
        }
    }

    for(int i=0;i<row1*col2;i++) {
        pthread_join(thread[i],NULL);//Waiting for each thread to terminate.

    }

}



////////////////Printing Matrix in the console//////////////////////


void printMatrix(int matrixOutput[1000][1000],int rowOut,int colOut){//Printing the Output Matrix.

    printf("row=%d col=%d\n",rowOut,colOut);
    for(int i=0;i<rowOut;i++){
        for(int j=0;j<colOut;j++){
            printf("%d ",matrixOutput[i][j]);
        }
        printf("\n");
    }
}



///////////////Runtime and Threads number Count//////////////////


void runtime1(){//Run the first method: 1 thread per matrix.

    struct timeval stop, start;

    gettimeofday(&start, NULL); //start checking time
    matrixMultCase1();
    gettimeofday(&stop, NULL); //end checking time

    save1();//Saving the output matrix.

    printMatrix(matrixOut,row1,col2);//Printing the output matrix in the console.

    printf("Number of Threads Created: 1 Thread\n");
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
}


void runtime2(){//Run the second method: 1 thread per row.

    struct timeval stop, start;

    gettimeofday(&start, NULL); //start checking time
    mat2();
    gettimeofday(&stop, NULL); //end checking time

    save2();//Saving the output matrix.

    printMatrix(matrixOut,row1,col2);//Printing the output matrix in the console.

    printf("Number of Threads Created: %d Threads\n",row1);
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
}


void runtime3(){//Run the third method: 1 thread per element.

    struct timeval stop, start;

    gettimeofday(&start, NULL); //start checking time
    mat3();
    gettimeofday(&stop, NULL); //end checking time

    save3();//Saving the output matrix.

    printMatrix(matrixOut,row1,col2);//Printing the output matrix in the console.

    printf("Number of Threads Created: %d Threads\n",row1*col2);
    printf("Seconds taken %lu\n", stop.tv_sec - start.tv_sec);
    printf("Microseconds taken: %lu\n", stop.tv_usec - start.tv_usec);
}




/////////////////main Function////////////////////


int main(int argc, char *argv[]) {
    
    
    if(argc==4){//entered specified files names for matrices 1, 2 and the output matrix.

        strcpy(fileName, "");
        strcat(fileName, argv[1]);
        strcat(fileName, ".txt");
        readFile(fileName, 0);

        strcpy(fileName, "");
        strcat(fileName, argv[2]);
        strcat(fileName, ".txt");
        readFile(fileName, 1);

        strcpy(fileName, "");
        strcat(fileName, argv[3]);

    }else if(argc==3){//entered specified files names for matrices 1 and 2.

        strcpy(fileName, "");
        strcat(fileName, argv[1]);
        strcat(fileName, ".txt");
        readFile(fileName, 0);

        strcpy(fileName, "");
        strcat(fileName, argv[2]);
        strcat(fileName, ".txt");
        readFile(fileName, 1);

        strcpy(fileName, "");
        strcat(fileName, "c");

    }else if(argc==2){//entered specified file name for matrix 1.

        strcpy(fileName, "");
        strcat(fileName, argv[1]);
        strcat(fileName, ".txt");
        readFile(fileName, 0);

        strcpy(fileName, "");
        strcat(fileName, "b.txt");
        readFile(fileName, 1);

        strcpy(fileName, "");
        strcat(fileName, "c");

    }else if(argc==1){//default names for input and output files.

        strcpy(fileName, "");
        strcat(fileName, "a.txt");
        readFile(fileName, 0);

        strcpy(fileName, "");
        strcat(fileName, "b.txt");
        readFile(fileName, 1);

        strcpy(fileName, "");
        strcat(fileName, "c");

    }else{

        printf("Invalid Args!!\n");
        exit(-1);
    }


    if(col1!=row2){//The 2 matrices entered can't be multiplied because they have invalid dimensions.
        printf("Invalid Dimensions!!\n");
        exit(-1);
    }

    //Running the first method: 1 thread per matrix.
    printf("\n--------------->FIRST METHOD: 1 THREAD PER MATRIX<---------------\n");
    runtime1();
    printf("\n//////////////////////////////////////////////////////\n");

    //Running the second method: 1 thread per row.
    printf("\n--------------->SECOND METHOD: 1 THREAD PER ROW<---------------\n");
    runtime2();
    printf("\n//////////////////////////////////////////////////////\n");

    //Running the third method: 1 thread per element.
    printf("\n--------------->THIRD METHOD: 1 THREAD PER ELEMENT<---------------\n");
    runtime3();
    printf("\n//////////////////////////////////////////////////////\n");

    return 0;
}
