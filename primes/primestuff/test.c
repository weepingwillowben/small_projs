#include <math.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
int main(int argc, char **argv){
	int n = atoi(argv[1]);
	int prime_count = 1;
	int start = clock();
	for(int k = 3; k < n; k++){
		int is_prime = 1;
		for(int j = 3; j <= (int)(sqrt(k) + 0.001); j += 2){
			if (k % j == 0){
				is_prime = 0;
				break;
			}
		}	
		if (k % 2 == 1 && is_prime)
			prime_count++;
	}
	int time = clock() - start;
	
	printf("%d\n",prime_count);
	printf("in% dms\n",time);
}