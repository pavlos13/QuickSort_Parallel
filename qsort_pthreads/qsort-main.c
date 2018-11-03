/**********************************************************************
 *
 * qsort.c -- Sequential implementation of QuickSort
 *
 * Nikos Pitsianis <nikos.pitsianis@eng.auth.gr>
 * Dimitris Floros <fcdimitr@auth.gr>
 * Time-stamp: <2018-10-10>
 *
 **********************************************************************/
 
  /* AVGOUSTINAKIS PAVLOS - 8111**
  **************PTHREADS**************/
  

#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include "qsort-sequential.h"
#include <assert.h>
#include <pthread.h>

/* local function declarations */
int  test( int *a, int n);
void init( int *a, int n);
void print(int *a, int n);
void *wrapper (void *arg) ;      //Jump function to qsort_seq called by pthread_create

//compare function for qsort built-in
int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

//Global variables
int activeThreads, maxThreads;
pthread_mutex_t mux;

//Struct of values passed to qsort_seq by pthread_create
typedef struct{
	int *arg_v;
    int arg_n;
} arg_struct;
	

/* --- Entry POINT --- */
int main(int argc, char **argv) {

  /* parse input */
  if (argc != 3) {
    printf("Usage: %s q p\n  where n=2^q is problem size and th=2^p is thread number (power of two)\n", 
	   argv[0]);
    exit(1);
  }

  /* variables to hold execution time */
  struct timeval startwtime, endwtime;
  double seq_time, par_time , time_ratio;
  
  
  pthread_t thread;   //new thread
  
  arg_struct arg;
  
 
  /* initiate vector of random integerts */
  int n  = 1<<atoi(argv[1]);
  
  activeThreads = 0;
  maxThreads=1<<atoi(argv[2]); 
  int *a = (int *) malloc(n * sizeof(int));
  
  int *a_p = (int *) malloc(n * sizeof(int));	// a_p, a_s are used for copies of initial unsorted vector
  int *a_s = (int *) malloc(n * sizeof(int));
 printf("Max Threads= %d\n",maxThreads);
    /* initialize vector */
  init(a, n);
  
  //Copy initial vector
  for (int i=0; i<n; i++){
	  a_p[i]=a[i];
	  a_s[i]=a[i];
  }
	
  //PARALLEL
  
  //arg contains function arguments
  arg.arg_v=a_p;
  arg.arg_n=n;
  
  /* print vector */
  //print(a, n);
  
  //Initialise mux
  pthread_mutex_init (&mux, NULL);
  /* sort elements in original order */
  gettimeofday (&startwtime, NULL);
  pthread_create(&thread, NULL, wrapper, (void *)&arg); //call thread
  pthread_join (thread, NULL);
  //qsort_seq(a_p, n);
  gettimeofday (&endwtime, NULL);

  /* get time in seconds */
  par_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
                      + endwtime.tv_sec - startwtime.tv_sec);

  /* validate result */
  int pass = test(a_p, n);
  printf("TEST %s\n",(pass) ? "PASSed" : "FAILed");
  assert( pass != 0 );
  
  
    //SEQUENTIAL
	
/* initialize vector */

  arg.arg_v=a_s;
  arg.arg_n=n;
  // print vector 

  /* sort elements in original order */
  gettimeofday (&startwtime, NULL);
  qsort(a_s, n, sizeof(int), cmpfunc);
  gettimeofday (&endwtime, NULL);

   /*get time in seconds */
   seq_time = (double)((endwtime.tv_usec - startwtime.tv_usec)/1.0e6
                      + endwtime.tv_sec - startwtime.tv_sec);

  /* validate result */
  int pass2 = test(a_s, n);
  printf("TEST %s\n",(pass2) ? "PASSed" : "FAILed");
  assert( pass2 != 0 );  
  
  
    /* print sorted vector */
  //print(a, n);  */
  
  /* print execution time */
  printf("Sequential wall clock time: %f sec\n", seq_time);
  printf("Parallel wall clock time: %f sec\n", par_time);
  time_ratio= seq_time/par_time;
  printf("Parallel algorithm was  %f times faster\n", time_ratio);

  /* exit */
  return 0;
  
}

/** -------------- SUB-PROCEDURES  ----------------- **/ 

/** procedure test() : verify sort results **/
int test(int *a, int n) {

  int pass = 1; 

  for (int i=0; i<n; i++){

    if (a[i-1] > a[i]){
      pass = 0;
    }

  }

  return pass;
  
  
}

/** procedure init() : initialize array "a" with data **/
void init(int *a, int n) {
  int i;
  for (i = 0; i < n; i++) {
    a[i] = rand() % n; // (N - i);
  }
}

/** procedure  print() : print array elements **/
void print(int *a, int n) {
  int i;
  for (i = 0; i < n; i++) {
    printf("%d ", a[i]);
  }
  printf("\n");
}

void *wrapper (void *arg) {
	int *a;
	int n;
	//printf("I am inside!\n");
	a = ((arg_struct*) arg )->arg_v;
	n = ((arg_struct*) arg )->arg_n;
	qsort_seq(a, n);
	
}