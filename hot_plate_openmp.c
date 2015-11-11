#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<sys/time.h>
#include<omp.h>

#define MAXROW 1024
#define MAXCOL 1024

double when();

float array1[MAXROW][MAXCOL];
float array2[MAXROW][MAXCOL];

int main()
{
	int iter = 0;
	double start_time;
	double end_time;
	int procs;
	int not_converged = 1;
	float temp;
	float convergence;

        start_time = when();
        procs = omp_get_num_procs();
        omp_set_num_threads(procs);

	/*Intialize arrays*/

		for(int i=0;i<MAXROW;i++)
		{
			for(int j=0;j<MAXCOL;j++)
			{
				if(i==0 || j==0 || j==(MAXCOL-1))
				{
					array1[i][j] = 0.0;
					array2[i][j] = 0.0;
				}
				else if (i==MAXROW-1)
				{
					array1[i][j] = 100.0;
					array2[i][j] = 100.0;
				}
				else if (i==400 && j<=330)
				{
					array1[i][j] = 100.0;
					array2[i][j] = 100.0;
				}
				else if (i==200 && j ==500)
				{
					array1[i][j] = 100.0;
					array2[i][j] = 100.0;
				}
				else
				{
					array1[i][j] = 50.0;
					array2[i][j] = 50.0;
				}
			}
		}
		
	/*Initialisation complete */
	int i,j;
	#pragma omp parallel private (temp) shared(array1, array2) reduction(+:iter)
	{	
	while(not_converged)
	{
		/*calculate new values*/
		#pragma omp for
		for (int i=0; i<MAXROW; i++)
		{
			for(int j=0; j< MAXCOL; j++)
			{
				if( i==0 || (i==MAXROW-1) || j==0 || j== (MAXCOL-1) || 
					(i==400 && j<=330) || (i==200 && j ==500))
				{
					//skip
				}
				else
				{
					array2[i][j] = (array1[i+1][j] + array1[i-1][j] + array1[i][j+1] + 
						array1[i][j-1] + (4 * array1[i][j]))/8.0;
				}
			}
		}
		
		#pragma omp barrier	
			/*swap arrays*/
			#pragma omp for
			for(int i=0;i<MAXROW;i++)
			{
				for(int j=0 ; j<MAXCOL ; j++)
				{
					temp = array1[i][j];
					array1[i][j] = array2[i][j];
					array2[i][j] = temp;
				}
			}
		/* swapping complete*/
		#pragma omp barrier

		/*check for convergence*/
		not_converged = 0;
		#pragma omp for
		for(int i=0;i<MAXROW;i++)
			{
				for(int j=0;j<MAXCOL;j++)
				{

					if( i==0 || (i==MAXROW-1) || j==0 || j== (MAXCOL-1) || 
						(i==400 && j<=330) || (i==200 && j ==500))
					{
									//skip
					}
					else
					{
						convergence = array1[i][j]- ((array1[i+1][j] + array1[i-1][j] + 
							array1[i][j+1] + array1[i][j-1])/4.0 );

						if(fabs(convergence) > 0.1)
						{
							not_converged = 1;
						}
					}
				}

			}
		#pragma omp single
		iter++;
	}
	}
	end_time = when();
        printf("No. of processors = %d\n",procs);
	printf("Total execution time = %f\n", end_time-start_time);
	printf("no of iterations = %d\n",iter);
	
	return 1;
}
double when()
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return ((double) tp.tv_sec + (double) tp.tv_usec * 1e-6);
}
