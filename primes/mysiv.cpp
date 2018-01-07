/*
Multithreaded sieve of Eratosthenes by Benjamin Black.

Adapted from the public domain segmented sieve of Eratosthenes written by 
Kim Walisch and posted at http://primesieve.org/segmented_sieve.html

Useage: enter max number and generate primes and get execution times

Different preprocessor definitions can produce the following behavior:
MAKE_FILE outputs a csv file instead of to the console
SEGMENTED: Outputs execution times of different sizes of segments and 
	different numbers of threads.
	If segmented is not defined, then it will output times of 
	different numbers of threads for an unsegmented sieve. Note that 
	this will crash given a high enough number.
UNROLLED: Use an unrolled inner loop.
SINGLE_BIT_SIEVE: sieve is stored in a bit by bit manner instead of a byte by byte.

MAX_SEGMENT_SIZE power of two of the highest size of segment
MIN_SEGMENT_SIZE power of two of the lowest size of segment

*/
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <stdint.h>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include <sstream>
using namespace std;

vector<int64_t> counts;
template < typename T > std::string to_string( const T& n )
{
	std::ostringstream stm ;
	stm << n ;
	return stm.str() ;
}
#define SEGMENTED
#define MAX_SEGMENT_SIZE 25
#define MIN_SEGMENT_SIZE 12
//#define UNROLLED
#define SINGLE_BIT_SIEVE
#define MAKE_FILE

#ifdef SINGLE_BIT_SIEVE
typedef vector<bool> sieve_type;
#else
typedef vector<char> sieve_type;
#endif
int get_next(int64_t prime, int64_t threshold){
	//this is not optimal!!!
	int64_t sqr = prime * prime;
	if (sqr > threshold)
		return sqr - threshold;

	int64_t k = prime * 2;
	int64_t j = sqr + ((threshold - sqr) / k) * k;
	sqr = j - threshold;
	if (sqr < 0)
		sqr += k;
	return sqr;
}
double get_time(){
	using namespace chrono;
	return duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count() / 1000000000.0;
}
#ifdef UNROLLED
int64_t elim_composites(sieve_type & sieve, int64_t start, int64_t end,int64_t incr){
	int64_t unrolled_end = end - 8 * incr;
	int64_t j = start;
	for (; j < unrolled_end;){
		sieve[j] = 0;
		j += incr;
		sieve[j] = 0;
		j += incr;
		sieve[j] = 0;
		j += incr;
		sieve[j] = 0;
		j += incr;
		sieve[j] = 0;
		j += incr;
		sieve[j] = 0;
		j += incr;
		sieve[j] = 0;
		j += incr;
		sieve[j] = 0;
		j += incr;
	}
	for (; j < end; j += incr){
		sieve[j] = 0;
	}
	return j;
}
#else
int64_t elim_composites(sieve_type & sieve, int64_t start, int64_t end,int64_t incr){
	int64_t j = start;
	for (; j < end; j += incr){
		sieve[j] = 0;
	}
	return j;
}
#endif
#ifdef SEGMENTED
vector<vector<int64_t>> thread_primes;
void sieve_section(int64_t low_lim,int64_t limit,sieve_type * is_prime_p,int t_num,int segment_size)
{
	sieve_type & is_prime = *is_prime_p;
	sieve_type sieve(segment_size);
	
	vector<int> primes;
	vector<int> next;
	
	int64_t front_sqrt = 2;
	for (; front_sqrt * front_sqrt <= low_lim; front_sqrt++)
	{
		if (is_prime[front_sqrt])
		{
			primes.push_back(front_sqrt);
			
			
			next.push_back(get_next(front_sqrt,low_lim));
		}
	}
	int64_t n = max(int64_t(3),low_lim % 2 ? low_lim : low_lim + 1);
	int64_t count = 0;
	for (int64_t low = low_lim; low <= limit; low += segment_size)
	{
		fill(sieve.begin(), sieve.end(), 1);
 
		// current segment = interval [low, high]
		int64_t high = min(low + segment_size - 1, limit);
 
		// store small primes needed to cross off multiples
		for ( ;front_sqrt * front_sqrt <= high; front_sqrt++){
			if (is_prime[front_sqrt]){
				primes.push_back(front_sqrt);
				next.push_back(front_sqrt * front_sqrt - low);
			}
		}
		// sieve the current segment
		int size = primes.size();
		for (size_t i = 1; i < size; i++)
		{
			int j = next[i];
			int k = primes[i] * 2;
			
			j = elim_composites(sieve,j,segment_size,k);
			
			next[i] = j - segment_size;
		}
		for (; n <= high; n += 2)
			if (sieve[n - low]){ // n is a prime
				count++;
				//thread_primes[t_num].push_back(n);
			}
	}
	counts[t_num] = count;
}
#else
void sieve_section(int64_t low_lim,int64_t limit,sieve_type * is_prime_p,int t_num,int segment_size)
{
	sieve_type & is_prime = *is_prime_p;
	sieve_type sieve(limit - low_lim+1,1);
 
	vector<int> primes;
	
	int64_t front_sqrt = 2;
	for (; front_sqrt * front_sqrt <= limit; front_sqrt++)
	{
		if (is_prime[front_sqrt])
		{
			primes.push_back(front_sqrt);
		}
	}
	// sieve the current segment
	int size = primes.size();
	for (size_t i = 1; i < size; i++)
	{
		int prime = primes[i];
		int k = prime * 2;
		int64_t j = get_next(prime,low_lim);
		
		elim_composites(sieve,j,limit - low_lim,k);
	}
	int64_t count = 0;
	for (int64_t n = max(int64_t(3), low_lim % 2 ? low_lim : low_lim + 1); n <= limit; n += 2){
		if (sieve[n - low_lim]) // n is a prime
			count++;
	}

	counts[t_num] = count;
}
#endif
/*vector<int64_t> reg_sieve(int64_t limit){
	vector<int64_t> primes;
	sieve_type is_prime(limit+1, 1);
	for (int64_t i = 2; i <= limit; i++){
		if (is_prime[i]){
			primes.push_back(i);
			for (int64_t j = i * i; j <= limit; j += i){
				is_prime[j] = 0;
			}
		}
	}
	
	return primes;
}*/
int64_t segmented_sieve(int64_t limit, int num_of_threads, int segment_size)
{
	if (limit < 2)
		return 0;
	int small_size = (int)(sqrt(limit)) + 1;
	int64_t count = (limit < 2) ? 0 : 1;

	// generate small primes <= sqrt
	sieve_type is_prime(small_size, 1);
	for (int i = 2; i * i < small_size; i++)
		if (is_prime[i])
			for (int j = i * i; j < small_size; j += i)
				is_prime[j] = 0;

	counts.assign(num_of_threads,0);
	vector<thread> threads;

	int64_t low = 2;
	for (int n = 0; n < num_of_threads;n++){
		int64_t high = min(low + limit / num_of_threads + 1, limit);
		threads.push_back(thread(sieve_section,low, high, &is_prime,n,segment_size));
		low = high+1;
	}
	for (thread & t : threads)
		t.join();
	for (int64_t c : counts)
		count += c;

	return count;
}
string spaces(int num){
	return string(num, ' ');
}
int main(int argc, char** argv)
{
	// generate the primes below this number
	int64_t limit = 800000000;
	while (limit){
		cin >> limit;
#ifdef MAKE_FILE
		ofstream cout(to_string(limit) + ".csv");
#endif
	
		int seg_size = (int64_t(sqrt(limit)) >> 1) << 1;
		int count = segmented_sieve(limit, 4, seg_size);
		cout << "\ncount = " << count;
		
#ifdef SEGMENTED
		cout << "\nthreads,";
		for (int64_t c = 1LL << MIN_SEGMENT_SIZE; c < (1LL << MAX_SEGMENT_SIZE); c <<= 1){
			cout << c << ",";
		}
		cout << "\n";
		for (int t = 1; t < 8; t++){
			cout << "      " << t << ",";
			for (int64_t c = 1LL << MIN_SEGMENT_SIZE; c < (1LL << MAX_SEGMENT_SIZE); c <<= 1){
				double start = get_time();
				int count = segmented_sieve(limit, t, c);
				double time = get_time() - start;

				cout << spaces(6 - int(log10(time + 1.0))) << time << ",";
			}
			cout << "\n";
		}
#else
		cout << "unsegmented sieve:\n";
		for (int t = 1; t < 8; t++){
			cout << t << ",";
			double start = get_time();
			count = segmented_sieve(limit, t, -1);
			double time = get_time() - start;
			cout << time << "\n";
		}
#endif
	}
	return 0;
}