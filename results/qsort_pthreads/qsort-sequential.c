/**********************************************************************
 *
 * qsort-sequential implementation of QuickSort
 *
 * Nikos Pitsianis <nikos.pitsianis@eng.auth.gr>
 * Dimitris Floros <fcdimitr@auth.gr>
 * Time-stamp: <2018-10-10>
 *
 **********************************************************************/
#include <stdio.h>
#include <pthread.h>
extern int activeThreads, maxThreads;
extern pthread_mutex_t mux;
typedef struct{
	int *arg_v;
    int arg_n;
} arg_struct;


/* swap -- swap elements k and l of vector v */
void swap(int *v, int k, int l) {
  int temp = v[k];
  v[k] = v[l];
  v[l] = temp;
}


/* partition -- in-place update of elements */
int partition(int *v, int n) {
  int pivot = v[n-1];
  int i = 0;
  
  for (int j = 0; j < n - 1; j++) 
    if (v[j] < pivot) 
      swap(v,i++,j);

  swap(v, i, n - 1);
  return (i);
}

void *wrapper1 (void *arg) {
	
	int *a;
	int n;
	//printf("I am inside!\n");
	a = ((arg_struct*) arg )->arg_v;
	n = ((arg_struct*) arg )->arg_n;
	qsort_seq(a, n);
	
}

/* qsortseq -- Entry point for QuickSort */
void qsort_seq(int *v, int n) {
  if (n > 1) {
	  
	
 int p = partition(v, n);
    /*   qsort_seq(v,p);
    qsort_seq(&v[p+1],n-p-1);
	*/
	arg_struct arg1,arg2;
	arg1.arg_v=v;
    arg1.arg_n=p;
	arg2.arg_v=&v[p+1];
    arg2.arg_n=n-p-1;
	
	pthread_t thread1, thread2;

	
	 int parallel = 0;

 
    if (activeThreads < maxThreads-1 && n>550) {
      pthread_mutex_lock (&mux);
      activeThreads += 1;
      pthread_mutex_unlock (&mux);
      pthread_create(&thread1, NULL, wrapper1, (void *)&arg1);
	  //pthread_create(&thread2, NULL, wrapper1, (void *)&arg2);
	  qsort_seq(&v[p+1],n-p-1);
      parallel = 1;
    }

	
	 if (parallel) {

     // printf("Threads %d\n",activeThreads);
      pthread_join (thread1, NULL);
	  //pthread_join (thread2, NULL);
      pthread_mutex_lock (&mux);
      activeThreads -= 1;
      pthread_mutex_unlock (&mux);
     
    }
    else {
      qsort_seq(v,p);
      qsort_seq(&v[p+1],n-p-1);
    }
  }
}

