/*
 * lab1_parallel.c
 *
 *  Created on: Jan 13, 2014
 *      Author: rnakade
 */


#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<sys/time.h>

#define MAXROW 1024
#define MAXCOL 1024

float array1[MAXROW][MAXCOL];
float array2[MAXROW][MAXCOL];



void initialize_array(float arr[][MAXCOL]);
void calculate_new_values(float arr1[][MAXCOL],float arr2[][MAXCOL]);
int check_for_convergence(float arr[][MAXCOL]);
double when();

int main()
{

	int iter = 0;
	int status1=1;
	int status2=1;
	int hot_cells=0;
	double start_time;
	double end_time;

	start_time = when();

	initialize_array(array1);
	initialize_array(array2);

	while(1){

		calculate_new_values(array2,array1);
		status1 = check_for_convergence(array2);
		iter++;
		if(status1 == 0) break;

		calculate_new_values(array1,array2);
		status2 = check_for_convergence(array1);
		iter++;
		if(status2 == 0) break;
	}


	if(status1 == 0)
	{
		for(int i=0;i<MAXROW;i++)
		{
			for(int j=0; j<MAXCOL; j++)
			{
				if(array2[i][j]>50) hot_cells++;
			}
		}
	}
	else
	{
		for(int i=0;i<MAXROW;i++)
		{
			for(int j=0; j<MAXCOL; j++)
			{
				if(array1[i][j]>50) hot_cells++;
			}
		}
	}

	end_time = when();
	printf("no of iterations = %d\n",iter);
	printf("No. of cells with temperature greater than 50 is %d\n",hot_cells);
	printf("Total execution time = %f\n", end_time-start_time);

	return 1;
}

void initialize_array(float arr[][MAXCOL])
{
	int i,j;
	for(i=0;i<MAXROW;i++)
		{
			for(j=0;j<MAXCOL;j++)
			{
				if(i==0 || j==0 || j==(MAXCOL-1))
				{
					arr[i][j] = 0.0;
				}
				else if (i==MAXROW-1)
				{
					arr[i][j] = 100.0;
				}
				else if (i==400 && j<=330)
				{
					arr[i][j] = 100.0;
				}
				else if (i==200 && j ==500)
				{
					arr[i][j] = 100.0;
				}
				else
				{
					arr[i][j] = 50.0;
				}
			}
		}
}

void calculate_new_values(float arr2[][MAXCOL],float arr1[][MAXCOL])
{
	for (int i=0; i<MAXROW; i++)
	{
		for(int j=0; j< MAXCOL; j++)
		{
			if( i==0 || (i==MAXROW-1) || j==0 || j== (MAXCOL-1) || (i==400 && j<=330) || (i==200 && j ==500))
			{
				//skip
			}
			else
			{
				arr2[i][j] = (arr1[i+1][j] + arr1[i-1][j] + arr1[i][j+1] + arr1[i][j-1] + (4 * arr1[i][j]))/8.0;
			}
		}
	}

}

int check_for_convergence(float arr[][MAXCOL])
{
	float convergence;
	int status=0;
	for(int i=0;i<MAXROW;i++)
	{
		for(int j=0;j<MAXCOL;j++)
		{

			if( i==0 || (i==MAXROW-1) || j==0 || j== (MAXCOL-1) || (i==400 && j<=330) || (i==200 && j ==500))
			{
							//skip
			}
			else
			{
				convergence = arr[i][j]- ((arr[i+1][j] + arr[i-1][j] + arr[i][j+1] + arr[i][j-1])/4.0 );

				if(fabs(convergence) > 0.1)
				{
					status = 1;
					return status;
				}
			}
		}
	}
	return status;
}
double when()
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	return ((double) tp.tv_sec + (double) tp.tv_usec * 1e-6);
}
