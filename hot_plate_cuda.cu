#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<sys/time.h>


#define MAXROW 1024
#define MAXCOL 1024

double when()
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return ((double) tp.tv_sec + (double) tp.tv_usec * 1e-6);
}
void initialize(float *oA, float *nA)
{
	int i,j;
	for(i=0; i<MAXROW; i++)
	{
		for(j=0;j<MAXCOL;j++)
		{
			if(i==0 || j==0 || j==(MAXCOL-1))
			{
				nA[i*MAXCOL + j] = 0.0;
				oA[i*MAXCOL + j] = 0.0;
			}
			else if (i==MAXROW-1)
			{
				nA[i*MAXCOL + j] = 100.0;
				oA[i*MAXCOL + j] = 100.0;
			}
			else if (i==400 && j<=330)
			{
				nA[i*MAXCOL + j] = 100.0;
				oA[i*MAXCOL + j] = 100.0;
			}
			else if (i==200 && j ==500)
			{
				nA[i*MAXCOL + j] = 100.0;
				oA[i*MAXCOL + j] = 100.0;
			}
			else
			{
				nA[i*MAXCOL + j] = 50.0;
				oA[i*MAXCOL + j] = 50.0;
			}
		}
	}
}

__global__ void calculate_new_values(float *nA, float *oA)
{
	if(blockIdx.x == 0 || threadIdx.x ==0 ||blockIdx.x == MAXCOL-1 || threadIdx.x == MAXCOL-1 
		|| (blockIdx.x==400 && threadIdx.x<=330) || (blockIdx.x==200 && threadIdx.x==500)  )
	{}
	else
	{
		nA[blockIdx.x * MAXCOL + threadIdx.x] 
			=(oA[(blockIdx.x+1) * MAXCOL + threadIdx.x] + 
			oA[(blockIdx.x-1) * MAXCOL + threadIdx.x] + 
			oA[blockIdx.x * MAXCOL + threadIdx.x+1] + 
			oA[blockIdx.x * MAXCOL + threadIdx.x-1] + 
			(4 * oA[blockIdx.x * MAXCOL + threadIdx.x]))/8.0;
	}
}

int main(void)
{
	double start_time = when();
	float *nA, *oA;
	float *d_nA, *d_oA;
	float *tmp;
	
	int iter=0;
	float convergence;
	int converged = 0;
	
	int size = MAXROW * MAXCOL * sizeof(float);

	nA = (float*)malloc(size);
	oA = (float*)malloc(size);

	cudaError_t err = cudaMalloc((void**)&d_nA,size);
	cudaError_t err1 = cudaMalloc((void**)&d_oA,size);
	
	initialize(oA, nA);

	cudaMemcpy(d_oA, oA, size, cudaMemcpyHostToDevice);
	cudaMemcpy(d_nA, nA, size, cudaMemcpyHostToDevice);
	
	while(!converged)
	{
		calculate_new_values<<<1024,1024>>>(d_nA , d_oA);
		cudaMemcpy(nA , d_nA, size, cudaMemcpyDeviceToHost);
	
		converged = 1;
		for(int i=1;i<MAXROW-1;i++)
		{
			for(int j=1;j<MAXCOL-1;j++)
			{

				if( (i==400 && j<=330) || (i==200 && j ==500))
				{
								//skip
				}
				else
				{
					convergence = nA[i*MAXCOL + j]- ((nA[(i+1)*MAXCOL + j] + nA[(i-1)*MAXCOL + j] 
						+ nA[i*MAXCOL + j+1] + nA[i*MAXCOL + j-1])/4.0 );

					if(fabs(convergence) > 0.1)
					{
						converged = 0;
						break;
					}
				}
			}
			if(converged == 0)
				break;
		}
		iter++;

		tmp = d_nA;
		d_nA = d_oA;
		d_oA = tmp;
	}
	printf("iter = %d and execution time = %f\n",iter, when() - start_time);
			
	cudaMemcpy(nA, d_nA, size, cudaMemcpyDeviceToHost);
	
	cudaFree(d_nA);
	cudaFree(d_oA);
	free(nA);
	free(oA);	
	
	return 1;
		
}
