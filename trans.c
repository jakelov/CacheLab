/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
 
// student id : lovingoodja1, witheesc
// name :  Jacob Lovingood, Spencer Withee    
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);

// The sizeof(int) == 4 
// S=5, E=1, B=5. We have 32 byte blocks, so we can store 8 integers, due to size of ints being 4 bytes.
char trans_desc32[] = "Transpose of a 32x32 Matrix";
void trans_32(int M, int N, int A[N][M], int B[M][N]) {
int k=0, i=0, j=0;
int a1,a2,a3,a4,a5,a6,a7,a8;
if (M==32) {
		for (i=0; i < N; i+= 8) { // row
			for (j=0; j < M; j+= 8) { // col
				for (k=0; k < 8; k++) { // block
					a1 = A[i+k][j+0];
					a2 = A[i+k][j+1];
					a3 = A[i+k][j+2];
					a4 = A[i+k][j+3];
					a5 = A[i+k][j+4];
					a6 = A[i+k][j+5];
					a7 = A[i+k][j+6];
					a8 = A[i+k][j+7];
					B[j+0][i+k] = a1;
					B[j+1][i+k] = a2;
					B[j+2][i+k] = a3;
					B[j+3][i+k] = a4;
					B[j+4][i+k] = a5;
					B[j+5][i+k] = a6;
					B[j+6][i+k] = a7;
					B[j+7][i+k] = a8;
					
				}
			}
		}
	}
}
/** The sizeof(int) == 4 
 * * S=5, E=1, B=6. We have 64 byte blocks, so we can store 4 integers, due to size of ints being 4 bytes.
*/
char trans_desc64[] = "Transpose of a 64x64 Matrix";
void trans_64(int M, int N, int A[N][M], int B[M][N]) {
int k=0, i=0, j=0;
int a1,a2,a3,a4,a5,a6,a7,a8;
if (M==64) {
		for (i=0; i < N; i+= 8) {
			for (j=0; j < M; j+= 8) {
				for (k=0; k < 8; k++) {
					a1 = A[i+k][j+0];
					a2 = A[i+k][j+1];
					a3 = A[i+k][j+2];
					a4 = A[i+k][j+3];
					B[j+0][i+k] = a1;
					B[j+1][i+k] = a2;
					B[j+2][i+k] = a3;
					B[j+3][i+k] = a4;
				}
				for (k=0; k < 8; k++) {
					a5 = A[i+k][j+4];
					a6 = A[i+k][j+5];
					a7 = A[i+k][j+6];
					a8 = A[i+k][j+7];
					B[j+4][i+k] = a5;
					B[j+5][i+k] = a6;
					B[j+6][i+k] = a7;
					B[j+7][i+k] = a8;
				}
			}
		}	
	}
}
/**
 * helper function to do the matrix transposing of a 61x67 matrix, broken up into smaller problems due to it's uneven size.
*/
char trans_desc61[] = "Transpose of a 61x67 matrix";
void trans_61(int M, int N, int A[N][M], int B[M][N]) {
int k=0, i=0, j=0;
int a1,a2,a3,a4,a5,a6,a7,a8;
if (M == 61) {
for (i=0; i < 64; i+=8) {
			for (j=0; j < 56; j+=8) {
				for (k=0; k < 8; k++) {
					a1 = A[i+k][j+0];
					a2 = A[i+k][j+1];
					a3 = A[i+k][j+2];
					a4 = A[i+k][j+3];
					a5 = A[i+k][j+4];
					a6 = A[i+k][j+5];
					a7 = A[i+k][j+6];
					a8 = A[i+k][j+7];
					B[j+0][i+k] = a1;
					B[j+1][i+k] = a2;
					B[j+2][i+k] = a3;
					B[j+3][i+k] = a4;
					B[j+4][i+k] = a5;
					B[j+5][i+k] = a6;
					B[j+6][i+k] = a7;
					B[j+7][i+k] = a8;
				}
			}
		}
		// 67x5 case for smaller subproblem
		for (i=0; i < 67; i++){
			a1 = A[i][56+0];
			a2 = A[i][56+1];
			a3 = A[i][56+2];
			a4 = A[i][56+3];
			a5 = A[i][56+4];
			B[56][i] = a1;
			B[57][i] = a2;
			B[58][i] = a3;
			B[59][i] = a4;
			B[60][i] = a5;
			
		}
		// 3x56 case for smaller subproblem
		for (j=0; j < 56; j+=8) {
			for (k=0; k < 3; k++) {
				a1 = A[64+k][j+0];
				a2 = A[64+k][j+1];
				a3 = A[64+k][j+2];
				a4 = A[64+k][j+3];
				a5 = A[64+k][j+4];
				a6 = A[64+k][j+5];
				a7 = A[64+k][j+6];
				a8 = A[64+k][j+7];
				B[j+0][64+k] = a1;
				B[j+1][64+k] = a2;
				B[j+2][64+k] = a3;
				B[j+3][64+k] = a4;
				B[j+4][64+k] = a5;
				B[j+5][64+k] = a6;
				B[j+6][64+k] = a7;
				B[j+7][64+k] = a8;
			}
		}
	}
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
	switch (M) 
	{
		// Calls specific trans function for specific size.
		case 32: trans_32(M,N,A,B); 
	  	break;
		case 64: trans_64(M,N,A,B);
		break;
		case 61: trans_61(M,N,A,B);
		break;
	}	
}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}
