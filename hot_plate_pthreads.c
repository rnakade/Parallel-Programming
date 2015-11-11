#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<sys/time.h>
#include<pthread.h>

#define MAXROW 1024
#define MAXCOL 1024
#define PROCS 16

float array1[MAXROW][MAXCOL];
float array2[MAXROW][MAXCOL];
pthread_t t[PROCS];

int num_of_iter = 0;
int not_converged = 1;
int count =0;

typedef struct barrier_node {
        pthread_mutex_t count_lock;
        pthread_cond_t ok_to_proceed;
        //int count;
} mylib_linear_barrier_t;

mylib_linear_barrier_t barrier1;
mylib_linear_barrier_t barrier2;
mylib_linear_barrier_t barrier3;

int check_for_convergence(int * thread_num);
double when();
void mylib_linear_barrier (mylib_linear_barrier_t * b);
void mylib_init_linear_barrier(mylib_linear_barrier_t * b);
void mylib_destroy_linear_barrier(mylib_linear_barrier_t * b);

int main(int argc, char* argv[])
{
	double start_time;
    double end_time;
	
	start_time = when();
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

	/*Initialisation of arrays complete */
	
	/* initialise barrier*/
	mylib_init_linear_barrier(&barrier1);
	mylib_init_linear_barrier(&barrier2);
	mylib_init_linear_barrier(&barrier3);

	/*Initialise Threads*/
	int n; int* arg;
	for(int i=0;i<PROCS;i++)
	{
		arg = malloc (sizeof(*arg));
		*arg = i;
		if(pthread_create(&t[i], NULL,(void*) &check_for_convergence,arg)!=0)
			printf("Thread creation failed\n");
	}
	/*Initialisation of threads complete */
	
	/* Wait for all threads to complete */
	for(int i=0;i<PROCS;i++)
	{
		pthread_join(t[i],NULL);
	}
	
	/*destroy barrier*/
	mylib_destroy_linear_barrier(&barrier1);
	mylib_destroy_linear_barrier(&barrier2);
	mylib_destroy_linear_barrier(&barrier3);
	
	end_time=when();
	
	printf("No of iterations = %d\n",num_of_iter);
	printf("Total execution time = %f\n", end_time-start_time);
	
	return 1;
}

int check_for_convergence(int * thread_num)
{
	float convergence;
	float temp;
	int current_thread = *thread_num;
	
	while(not_converged)
	{
		/*calculate new values*/
		for(int i = current_thread ; i< MAXROW; i+=PROCS)
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
						array2[i][j] = (array1[i+1][j] + array1[i-1][j] + array1[i][j+1] 
							+ array1[i][j-1] + (4 * array1[i][j]))/8.0;
				}
			}
		}
		
		mylib_linear_barrier(&barrier1);
		
		/*swap arrays*/	
		for(int i = current_thread ; i< MAXROW; i+=PROCS)
		{
			for(int j=0 ; j<MAXCOL ; j++)
			{
				temp = array1[i][j];
				array1[i][j] = array2[i][j];
				array2[i][j] = temp;
			}
		}
		/* swapping complete*/
		
		mylib_linear_barrier(&barrier2);
		
		/*check for convergence*/
		 if(current_thread == 0)
		 {
			not_converged = 0;
			for(int i = 0 ; i< MAXROW; i+=PROCS)
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
						convergence = array1[i][j]- ((array1[i+1][j] + array1[i-1][j] 
							+ array1[i][j+1] + array1[i][j-1])/4.0 );
						if(fabs(convergence) > 0.1)
							not_converged = 1;				
					}
				}
			 }
			 num_of_iter++;
		 }
		mylib_linear_barrier(&barrier3);
	}
	pthread_exit(NULL);
}

double when()
{
        struct timeval tp;
        gettimeofday(&tp, NULL);
        return ((double) tp.tv_sec + (double) tp.tv_usec * 1e-6);
}

void mylib_init_linear_barrier(mylib_linear_barrier_t * b)
{
        pthread_mutex_init(&(b->count_lock),NULL);
        pthread_cond_init(&(b->ok_to_proceed),NULL);
}
void mylib_linear_barrier (mylib_linear_barrier_t * b)
{
	pthread_mutex_lock(&(b->count_lock));
	count++;
	if(count!=PROCS)
		pthread_cond_wait(&(b->ok_to_proceed),&(b->count_lock));
	else
		count = 0;
	pthread_mutex_unlock(&(b->count_lock));
	pthread_cond_broadcast(&(b->ok_to_proceed));

}
void mylib_destroy_linear_barrier(mylib_linear_barrier_t * b)
{
        pthread_mutex_destroy(&b->count_lock);
        pthread_cond_destroy(&b->ok_to_proceed);
}
