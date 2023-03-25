//headers filename
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>
#include <math.h>

#define SIZE 16
#define MINIGRIDSIZE sqrt(SIZE)
#define UNASSIGNED 0
double start;

void print_grid(int grid[SIZE][SIZE]) {
   for (int row = 0; row < SIZE; row++) {
      for (int col = 0; col < SIZE; col++) {
         printf("%2d ", grid[row][col]);
      }
      printf("\n");
   }
}

int is_exist_row(int grid[SIZE][SIZE], int row, int num){
   for (int col = 0; col < SIZE; col++) {
      if (grid[row][col] == num) {
         return 1;
      }
   }
   return 0;
}

int is_exist_col(int grid[SIZE][SIZE], int col, int num) {
   for (int row = 0; row < SIZE; row++) {
      if (grid[row][col] == num) {
         return 1;
      }
   }
   return 0;
}

int is_exist_box(int grid[SIZE][SIZE], int startRow, int startCol, int num) {
   for (int row = 0; row < MINIGRIDSIZE; row++) {
      for (int col = 0; col < MINIGRIDSIZE; col++) {
         if (grid[row + startRow][col + startCol] == num) {
               return 1;
         } 
      }
   }
   return 0;
}

int is_safe_num(int grid[SIZE][SIZE], int row, int col, int num) {
   return !is_exist_row(grid, row, num) 
      && !is_exist_col(grid, col, num) 
      && !is_exist_box(grid, row - (row % (int)MINIGRIDSIZE), col - (col % (int)MINIGRIDSIZE), num);
}

int find_unassigned(int grid[SIZE][SIZE], int *row, int *col) {
   for (*row = 0; *row < SIZE; (*row)++) {
      for (*col = 0; *col < SIZE; (*col)++) {
         if (grid[*row][*col] == UNASSIGNED) {
               return 1;
         }
      }
   }
   return 0;
}


int solvePuzzle(int grid[SIZE][SIZE], int level) {
   
   int row = 0;
   int col = 0;
   if (!find_unassigned(grid, &row, &col)) return 1; 
   
   for (int num = 1; num <= SIZE; num++ ) {        
      if (is_safe_num(grid, row, col, num)) {            		
         #pragma omp task default(none) firstprivate(grid, row, col, num, level) shared(start) final(level>1)
         {			
            int copy_grid[SIZE][SIZE];			
            memcpy(copy_grid,grid,SIZE*SIZE*sizeof(int));
            copy_grid[row][col] = num;          
            if(solvePuzzle(copy_grid,level+1)) {
               print_grid(copy_grid);
               double end = omp_get_wtime();
               double time_spent = end - start;  
               printf("\nSUDOKU PUZZLE SOLVED SUCCESSFULLY !!!\n");       
               printf("\nTime Lapse: %f seconds\n",time_spent);  
               exit(0);                    
            }
         }                          
      }
   }
   #pragma omp taskwait
   return 0;
}

int main(int argc, char** argv) {
   //selection of threads 
   int nthreads;
   printf("Enter the number of threads:");
   scanf("%d",&nthreads);
   omp_set_num_threads(nthreads);
   
   //declaration of sudoku board
   int sudoku[SIZE][SIZE];
   for(int a=0;a<SIZE;a++) {
      for(int b=0;b<SIZE;b++) {
         sudoku[a][b]=0;
      }
   }
   
   //reading sudoku grid from .txt file
   char filename[100];
   FILE *fptr;
   printf("Enter the directory name:");
   scanf("%s",filename);
   fptr=fopen(filename,"r");
   if(fptr==NULL) {
      printf("<< ERROR !!! CANNOT OPEN FILE >>\n\n");
      exit(1);
   }
   for(int a=0;a<SIZE;a++) {
      for(int b=0;b<SIZE;b++) {
         fscanf(fptr,"%d",&sudoku[a][b]);
      }
   }
   fclose(fptr);
   
   //remaining implementation
   start = omp_get_wtime();
   printf("\nSudoku Grid Size: %d x %d", SIZE,SIZE);   
   printf("\n");
   printf("\nSolving Sudoku Puzzle: \n\n");
   printf("************************INPUT GRID***********************\n\n");
   print_grid(sudoku);
   printf("\n------------------------Processing------------------------\n\n"); 
   printf("************************OUTPUT GRID***********************\n\n");  	   
   
   #pragma omp parallel default(none) shared(sudoku)
   #pragma omp single nowait 
   {
      solvePuzzle(sudoku,1);   
   }
   return 0;  
}