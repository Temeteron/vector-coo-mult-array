#include "libraries.h"
/*Greasidis Dimitrios AEM 1624
 *Kontogiannis Dimitrios AEM 1565
 *
 *Serial implementation of sparse matrix-vector multiplication using the COO format.I/O of matrix
 *is done by the Matrix-Market library mmio.h and generation of b vector is done randomly by the program.
 *The matrix read in the input is the transpose of the corresponding .mtx files in order to have the 
 *appropriate index sorted.The function COOSpMV takes as arguements the i and j indices of the matrix's 
 *non-zero elements as well as the multiplication vector,the dimension of the matrix and the number of 
 *non-zero elements.The function produces the resulting y vector and its performance is timed.
 *
 *Compile : make
 *Run : make run ARGS= "matrixfilename.mtx"
 */

//Function Performing the multiplication
void COOSpMV(int *I_index ,int *J_index,double *A_values,double *b_vector,double *y_vector,int N,int NZ){
	int i;
	
	for (i=0; i<N; i++){
		y_vector[i]=0;
	}
	for (i=0; i<NZ; i++){
		y_vector[I_index[i]]=y_vector[I_index[i]] + A_values[i]*b_vector[J_index[i]];
	}
	
}

int main(int argc, char *argv[]){
    int ret_code;
    MM_typecode matcode;
    FILE *f;
    int M, N, nz;   
    int i, *I_index, *J_index;
    double *A_values,*b_vector,*y_vector, time_spent;

    clock_t begin, end;
    
    // begin = clock();

    if (argc < 2){
		fprintf(stderr, "Usage: %s [martix-market-filename]\n", argv[0]);
		exit(1);
	} else {
        if ((f = fopen(argv[1], "r")) == NULL){
            exit(1);
        }
    }

    if (mm_read_banner(f, &matcode) != 0){
        printf("Could not process Matrix Market banner.\n");
        exit(1);
    }


    /*  This is how one can screen matrix types if their application */
    /*  only supports a subset of the Matrix Market data types.      */

    if (mm_is_complex(matcode) && mm_is_matrix(matcode) && 
            mm_is_sparse(matcode) ){
        printf("Sorry, this application does not support ");
        printf("Market Market type: [%s]\n", mm_typecode_to_str(matcode));
        exit(1);
    }

    /* find out size of sparse matrix .... */

    if ((ret_code = mm_read_mtx_crd_size(f, &M, &N, &nz)) !=0)
        exit(1);


    /* reseve memory for matrices */
	//i_index array
    I_index = (int *) malloc(nz * sizeof(int));
    J_index = (int *) malloc(nz * sizeof(int));
    A_values = (double *) malloc(nz * sizeof(double));
	
	b_vector = (double *) malloc(N * sizeof(double));
	y_vector = (double *) malloc(N * sizeof(double));

    /* NOTE: when reading in doubles, ANSI C requires the use of the "l"  */
    /*   specifier as in "%lg", "%lf", "%le", otherwise errors will occur */
    /*  (ANSI C X3.159-1989, Sec. 4.9.6.2, p. 136 lines 13-15)            */

    for (i=0; i<nz; i++){
        fscanf(f, "%d %d %lg\n", &J_index[i], &I_index[i], &A_values[i]);
        I_index[i]--;  /* adjust from 1-based to 0-based */
        J_index[i]--;
    }

    if (f !=stdin) fclose(f);

    /************************/
    /* now write out matrix */
    /************************/

    mm_write_banner(stdout, matcode);
    mm_write_mtx_crd_size(stdout, M, N, nz);
   // for (i=0; i<nz; i++) {
   //     fprintf(stdout, "%d %d %20.19g\n", I_index[i]+1, J_index[i]+1, A_values[i]);
	//}
	//Fill b_vector with random numbers
	for (i=0; i<N; i++){
        b_vector[i]=(double)rand();
    }

	begin=clock();
	COOSpMV(I_index,J_index,A_values,b_vector,y_vector,N,nz);
	end=clock();
	//Timing the multiplication
	time_spent=(double)(end-begin) / CLOCKS_PER_SEC;
	printf("Time spent in serial Sparse Matrix-Vector Multiplication using COO : %lf \n",time_spent);
	/*Debug
		for(i=0;i<N;i++) {	
		printf("Position: %d   %lf \n",i,y_vector[i]); }
	*/

	return 0;
}

