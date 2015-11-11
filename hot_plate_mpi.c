#include <mpi.h>
#include <stdio.h>
#include<stdlib.h>
#include <sys/time.h>
#include <malloc.h>
#include <math.h>

#define MAXROW 1024
#define MAXCOL 1024

double When();

int main(int argc, char *argv[])
{
	float **nA;
	float **oA;
	float **tmp;
	
	int i,j;
	int done,reallydone;
	int iter;
	
    int nproc, iproc;
    MPI_Status status1, status2;
    MPI_Request request1, request2, request3, request4;

    MPI_Init(&argc, &argv);
    double starttime = When();

    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    MPI_Comm_rank(MPI_COMM_WORLD, &iproc);
    //fprintf(stderr,"%d: Hello from %d of %d\n", iproc, iproc, nproc);
    
 	/* Determine how much I should be doing and allocate the arrays*/   
 	int theSize = MAXROW/ nproc;
 	
    nA = (float **)malloc((theSize + 2) * sizeof(float*));
    oA = (float **)malloc((theSize + 2) * sizeof(float*));
    
    for (i=0;i<theSize + 2;i++)
    {
    	nA[i]= (float*)malloc(MAXCOL * sizeof(float));
    	oA[i]= (float*)malloc(MAXCOL * sizeof(float));
    }
    
    int start = 1;
    int end = theSize + 1;
    
    /* Initialize the cells */
    for (i = 0; i < theSize+2; i++)
    {
    	for(j = 0;j<MAXCOL;j++ )
    	{
        	nA[i][j] = oA[i][j] = 50.0;
        	if(iproc == 0)
        	{
        		start = 2;
        		oA[1][j] = nA[1][j] = 0.0;
        	}
        	if(iproc == nproc-1)
        	{
        		end = theSize;
        		oA[theSize][j] = nA[theSize][j] = 100.0;	
        	}
        }
        nA[i][0] = oA[i][0] = 0.0;
        nA[i][MAXCOL-1] = oA[i][MAXCOL-1] = 0.0;
    }
    /*Initialize other fixed cells*/
    done = 1;
    reallydone = 1;
    for(i=1 ; i<=nproc; i++)
    {
    	if(i*theSize >= 200)
    	{
    		if(iproc == (i-1))
    		{
    			//fprintf(stderr,"iproc is %d\n",iproc);
    			nA[200 - (iproc * theSize)][500] = oA[200 - (iproc * theSize)][500] = 100.0;
    			done = 0;
    		}
    		 MPI_Allreduce(&done, &reallydone, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
    		 if(reallydone ==0)
    		 	break;
    	}
    }
    done = 1;
    reallydone = 1;
    for(i=1 ; i<=nproc; i++)
    {
    	if(i*theSize >= 400)
    	{
    		if(iproc == (i-1))
    		{
    			for(j=0; j <= 330 ; j++)
    				nA[400 - (iproc * theSize)][j] = 100.0;
    				oA[400 - (iproc * theSize)][j] = 100.0;
    			done =0;
    		}
    		MPI_Allreduce(&done, &reallydone, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
    		if(reallydone ==0)
    			break;
    	}
    }

    /* Print values*/
    /*
   	for (i = 0; i < theSize+2; i++)
    {
    	for(j = 0;j<MAXCOL;j++ )
    	{
        	fprintf(stderr,"%d: value of oA[%d][%d] is %f\t value of 
        		nA[%d][%d] is %f\n",iproc,oA[i][j],i,j,nA[i][j]);
        }
    }
	*/
	
	/* Now compute values and check for convergence*/
	reallydone = 1; iter = 0;
	int done1, reallydone1;
	float convergence;
	while(reallydone)
	{
		done =0;
		/* First, I must get my neighbors boundary values */
        if (iproc != 0)
        {
            MPI_Isend(oA[1], MAXCOL, MPI_FLOAT, iproc - 1, 0, MPI_COMM_WORLD, &request1);
            MPI_Irecv(oA[0], MAXCOL, MPI_FLOAT, iproc - 1, 0, MPI_COMM_WORLD, &request2);
        }
        if (iproc != nproc - 1)
        {
            MPI_Isend(oA[theSize], MAXCOL, MPI_FLOAT, iproc + 1, 0, MPI_COMM_WORLD, &request3);
            MPI_Irecv(oA[theSize + 1], MAXCOL, MPI_FLOAT, iproc + 1, 0, MPI_COMM_WORLD, &request4);
        }
        /* Do the calculations */
        
        for (i = start+1; i < end-1; i++)
        {
        	for(j=1;j<MAXCOL-1;j++)
            	nA[i][j] = (oA[i+1][j] + oA[i-1][j] + oA[i][j+1] 
            		+ oA[i][j-1] + (4 * oA[i][j]))/8.0;
        }
        
		if(iproc !=0)
        	MPI_Wait(&request2,&status1);
        for(j=1;j<MAXCOL-1;j++)
            nA[start][j] = (oA[start+1][j] + oA[start-1][j] + oA[start][j+1] 
            	+ oA[start][j-1] + (4 * oA[start][j]))/8.0;
  	
        if(iproc != nproc-1)
			MPI_Wait(&request4,&status2);
		for(j=1;j<MAXCOL-1;j++)
            nA[end-1][j] = (oA[end][j] + oA[end-2][j] + oA[end-1][j+1] 
            	+ oA[end-1][j-1] + (4 * oA[end-1][j]))/8.0;
        
        /*
        MPI_Barrier(MPI_COMM_WORLD);
        done1 = 1;
    	reallydone1 = 1;
        for(i=1 ; i<=nproc; i++)
		{
			if(i*theSize >= 200)
			{
				if(iproc == (i-1))
				{
					nA[200 - (iproc * theSize)][500] = 100.0;
					done1=0;
				}
				MPI_Allreduce(&done1, &reallydone1, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
    		 	if(reallydone1 ==0)
    		 		break;
			}
		}
		MPI_Barrier(MPI_COMM_WORLD);
		done1 = 1;
    	reallydone1 = 1;
		for(i=1 ; i<=nproc; i++)
		{
			if(i*theSize >= 400)
			{
				if(iproc == (i-1))
				{
					for(j=1; j <= 330 ; j++)
						nA[400 - (iproc * theSize)][j] = 100.0;
					done1=0;
				}
			MPI_Allreduce(&done1, &reallydone1, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
    		if(reallydone1 ==0)
    			break;
			}
		}
		*/
        /* Check to see if converged */
        for (i = start; i < end; i++)
        {
        	for(j=1;j<MAXCOL-1;j++)
			{
				convergence = nA[i][j]- ((nA[i+1][j] + nA[i-1][j] 
					+ nA[i][j+1] + nA[i][j-1])/4.0 );
				if (fabs(convergence) > 0.1)
				{
					done = 1;
					break;
				}
            }
            if(done == 1)
				break;
        }
        MPI_Barrier(MPI_COMM_WORLD);
        /* Do a reduce to see if everybody is done */
        MPI_Allreduce(&done, &reallydone, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

        /* Swap the pointers */
        tmp = nA;
        nA = oA;
        oA = tmp;
        if(iproc == 0)
        	iter++;
	}
	if(iproc ==0)
		fprintf(stderr, "It took %d iterations and %f seconds to converge for %d nodes\n",
			iter, When() - starttime, nproc);
	
    free(nA);
    free(oA);
    
    MPI_Finalize();
    return 1;
}

/* Return the correct time in seconds, using a double precision number.       */
double When()
{
    struct timeval tp;
    gettimeofday(&tp, NULL);
    return ((double) tp.tv_sec + (double) tp.tv_usec * 1e-6);
}
