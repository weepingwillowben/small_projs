#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sysexits.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct _threadinfo {
  int thread_id;
  int num_threads;
  int max;
  int result;
  int *logarray;
  int arraysize;
  int logfile;
} threadinfo_t;

pthread_mutex_t rl;

void report(char *str) {
  pthread_mutex_lock(&rl);
  printf("%s\n",str);
  pthread_mutex_unlock(&rl);
}

int ilog(int n) {
  int count = 0;
  while (n >= 2) {
    n = n / 2;
    count++;
  }
  return count;
}

int pseudo_sqrt(int n) {
  int l = ilog(n) / 2 - 1;
  int m = n;
  while (l > 0) {
    m = m / 2;
    l--;
  }
  return m;
}
    
int isprime(int n) {
  char buffer[100];

  int sq = pseudo_sqrt(n);
  int divisor = 3;
  while (divisor < n) {
    if (n % divisor == 0) {
      return 0;
    }
    divisor+=2;
  }

  return 1;
}

void *find_primes(threadinfo_t *ti) {
  char buffer[100];

  int *primes = ti->logarray;
  int size = ti->arraysize;

  int file = ti->logfile;

  int start = 3 + 2*ti->thread_id;
  int delta = 2*ti->num_threads; 
  int upto = ti->max;

  
  int count = 0;
  int check = start;
  int index = 0;
  while (check < upto) {
    if (index >= size) {
       write(file,primes,index*sizeof(int));
       index = 0;
    } 
    primes[index] = check;
    index++;
    if (isprime(check)) {
      count++;
    }
    check += delta;
  }
  write(file,primes,index*sizeof(int));
  close(file);
   
  ti->result = count;

  return NULL;
}

int main(int argc, char **argv) {

  if (argc < 4) errx(EX_USAGE,"bad number of arguments.");
  int p = atoi(argv[1]);
  if (p < 1) errx(EX_USAGE,"bad argument value.");
  int n = atoi(argv[2]);
  if (n < 2) errx(EX_USAGE,"bad argument value.");
  int w = atoi(argv[3]);

  pthread_t workers[p];
  threadinfo_t info[p];
  char filename[16];
  pthread_mutex_init(&rl,NULL);

  for (int i=0; i<p; i++) {
    info[i].thread_id = i;
    info[i].num_threads = p;
    info[i].max = n;
    info[i].arraysize = w;
    info[i].logarray = (int *)malloc(w*sizeof(int));
    sprintf(filename,"isprime_%03d.log",i);
    info[i].logfile = open(filename, O_TRUNC | O_WRONLY);

    printf("thread:%d file:%d\n",i,info[i].logfile);
    pthread_create(&(workers[i]),
		   NULL,
		   (void *(*)(void *))find_primes,
		   (void *)&info[i]);
  }

  int found = 0;
  char buffer[100];
  for (int i=0; i<p; i++) {
    void *junk;
    pthread_join(workers[i],&junk);
    found += info[i].result;
  }

  printf("There were %d primes less than %d found.\n",found,n);
}
