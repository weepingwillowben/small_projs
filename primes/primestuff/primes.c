#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct _threadinfo {
  int thread_id;
  int num_threads;
  int max;
  int result;
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

  int delta = 2*ti->num_threads; 
  int check = 3 + 2*ti->thread_id;
  int upto = ti->max;
  int count = 0;
  
  while (check < upto) {
    if (isprime(check)) {
      count++;
    }
    check += delta;
  }

  ti->result = count;

  return NULL;
}

int main(int argc, char **argv) {

  int p = atoi(argv[1]);
  int n = atoi(argv[2]);

  pthread_t workers[p];
  threadinfo_t info[p];

  pthread_mutex_init(&rl,NULL);

  for (int i=0; i<p; i++) {
    info[i].thread_id = i;
    info[i].num_threads = p;
    info[i].max = n;
    pthread_create(&(workers[i]),NULL,(void *(*)(void *))find_primes,(void *)&info[i]);
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
