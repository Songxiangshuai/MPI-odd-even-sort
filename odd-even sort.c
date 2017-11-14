#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

const int MAX=16;
void Bubble_sort(int* a,int n);
int Compute_partner(int phase,int myid,int comm_sz);
void Merge_low(int *local_array, int*recv_array,int *temp_array,int local_n);
void Merge_high(int *local_array, int*recv_array,int *temp_array,int local_n);
int main(int argc, char **argv) {
    int n,myid,numprocs,i,j,phase;
	int partner;
	srand(time(NULL));	
	MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	if(myid==0) {
		printf("n:");
		scanf("%d",&n);
		
    }
	MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD);
	int local_array[n/numprocs];
	int recv_array[n/numprocs];
	int temp_array[n/numprocs];
	int global_array[n];
	
    for(i=0;i<n/numprocs;i++){   
	   local_array[i]=(rand()*(myid+1))%1000;	   
	}
	
	Bubble_sort(local_array,n/numprocs);
		
	MPI_Barrier(MPI_COMM_WORLD);
	for(j=0;j<numprocs;j++){
		if(myid==j){
			printf("process%d's local list:",myid);
			for(i=0;i<n/numprocs;i++){
				printf("%d ",local_array[i]);
			}
			printf("\n");
		}
		MPI_Barrier(MPI_COMM_WORLD);
	}
	for(phase=0;phase<numprocs;phase++){
	   partner=Compute_partner(phase,myid,numprocs);
       if(partner!=-1){
		 MPI_Sendrecv(local_array,n/numprocs,MPI_INT,partner,0,recv_array,n/numprocs,MPI_INT,partner,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
           		 
		 if(myid<partner)
			 Merge_low(local_array,recv_array,temp_array,n/numprocs);		     
	     else 
			 Merge_high(local_array,recv_array,temp_array,n/numprocs);
		
		 }
       MPI_Barrier(MPI_COMM_WORLD);	   
    }
	
	MPI_Gather(local_array,n/numprocs,MPI_INT,global_array,n/numprocs,MPI_INT,0,MPI_COMM_WORLD);
	if(myid==0)
	{  printf("Global list:");
	   for(i=0;i<n;i++){
				printf("%d ",global_array[i]);
			}
		printf("\n");
	}
	MPI_Finalize();
	return 0;
}

void Bubble_sort(int* a,int n){
   int length,i,temp;
   for(length=n; length>=2;length--){
	 
	  for(i=0;i<length-1;i++){
		  if(a[i]>a[i+1]){
	         temp=a[i];
			 a[i]=a[i+1];
			 a[i+1]=temp;
		  }
		
	  }
   }		  
}
int Compute_partner(int phase,int myid,int comm_sz){
	int partner;
    if(phase%2==0)
       if(myid%2!=0)
		   partner=myid-1;
	   else 
		   partner=myid+1;
	else 
		if(myid%2!=0)
			partner=myid+1;
		else
			partner=myid-1;
	if(partner==-1||partner==comm_sz)
       partner=-1;		
    return partner;
}
void Merge_low(int *local_array, int*recv_array,int*temp_array,int local_n){
	int m_i,r_i,t_i;    	
	m_i=r_i=t_i=0;
	
	while(t_i<local_n){
			if(local_array[m_i]<=recv_array[r_i]){			
				temp_array[t_i]=local_array[m_i];
				t_i++;
				m_i++;
			}
			else{
				temp_array[t_i]=recv_array[r_i];
				t_i++;
				r_i++;
            }				
    }
	for(m_i=0;m_i<local_n;m_i++)
			local_array[m_i]=temp_array[m_i];	
	
}
void Merge_high(int *local_array, int*recv_array,int*temp_array,int local_n){
	int m_i,r_i,t_i;    	
	m_i=r_i=t_i=local_n-1;
	
	
	while(t_i>=0){
			if(local_array[m_i]>recv_array[r_i]){			
				temp_array[t_i]=local_array[m_i];
				t_i--;
				m_i--;
			}
			else{
				temp_array[t_i]=recv_array[r_i];
				t_i--;
				r_i--;		
			}
	}
	for(m_i=0;m_i<local_n;m_i++)
	    local_array[m_i]=temp_array[m_i];	
	
}