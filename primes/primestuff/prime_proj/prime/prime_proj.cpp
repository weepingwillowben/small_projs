
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <stdint.h>
using namespace std;
/// Set your CPU's L1 data cache size (in bytes) here
const int L1D_CACHE_SIZE = 32768;
typedef vector<char> seive_type;

/// Generate primes using the segmented sieve of Eratosthenes.
/// This algorithm uses O(n log log n) operations and O(sqrt(n)) space.
/// @param limit         Sieve primes <= limit.
/// @param segment_size  Size of the sieve array in bytes.
///
/*
struct thread_info{
	thread_info(vector<int> * smalls){
		small_primes = smalls;
		count = 0;
	}
	int64_t count;
	vector<int> * small_primes;
	seive_type sieve;
};*/
/*
void seive_thread(int64_t low,int64_t high,thread_info & info){
	seive_type & sieve = info.sieve;
	vector<int> & small_primes = *info.small_primes;
    fill(sieve.begin(), sieve.end(), 1);

	int num_of_primes = small_primes.size();
	int high_sqrt = ceil(sqrt(high));
	for(int p = 1; p < num_of_primes; p++){
		int prime = small_primes[p];
		if (prime > high_sqrt)
			break;

		int64_t start = (low / prime) * prime;
		if (start < low)
			start += prime;

		for (int j = start - low; j < L1D_CACHE_SIZE; j += prime){
			sieve[j] = 0;
		}
	}
	for (int n = low % 2; n <= high - low; n += 2)
		info.count += sieve[n];
}

int64_t segmented_sieve(int64_t limit, int segment_size = L1D_CACHE_SIZE){
	int small_max = ceil(sqrt(limit));
	seive_type is_prime(small_max + 1, 1);

	for (int i = 2; i * i <= small_max; i++)
		if (is_prime[i])
			for (int j = i * i; j <= small_max; j += i)
				is_prime[j] = 0;

	vector<int> primes;
	int n = 0;
	for (char is_p : is_prime){
		if (is_p)
			primes.push_back(n);
		n++;
	}

	thread_info Inf(&primes);
	for (int64_t low = 0; low <= limit; low += segment_size){
		int64_t high = max(limit,low + segment_size);
		seive_thread(low,high,Inf);
	}
	return Inf.count;
}
*/
int64_t seive_section(int64_t low_lim,int64_t limit,seive_type & is_prime, int segment_size = L1D_CACHE_SIZE)
{
	// vector used for sieving
	vector<char> sieve(segment_size);

	vector<int> primes;
	vector<int> next;

	int64_t front_sqrt = 2;
	int64_t count = 0;
	for (; front_sqrt * front_sqrt <= low_lim; front_sqrt++)
	{
		if (is_prime[front_sqrt])
		{
			primes.push_back(front_sqrt);

			int start_num = low_lim - (low_lim % front_sqrt);
			if (start_num < 0)
				start_num += front_sqrt;

			next.push_back(start_num);
		}
	}
	for (int64_t low = low_lim; low <= limit; low += segment_size)
	{
		fill(sieve.begin(), sieve.end(), 1);

		// current segment = interval [low, high]
		int64_t high = min(low + segment_size - 1, limit);

		// store small primes needed to cross off multiples
		for ( ;front_sqrt * front_sqrt <= high; front_sqrt++)
		{
			if (is_prime[front_sqrt])
			{
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
			for (; j < segment_size; j += k)
				sieve[j] = 0;
			next[i] = j - segment_size;
		}

		for (int64_t n = 3; n <= high; n += 2)
			if (sieve[n - low]) // n is a prime
				count++;
	}
	return count;
}

int64_t segmented_sieve(int64_t limit, int segment_size = L1D_CACHE_SIZE)
{
	if (limit < 2)
		return 0;
	int small_size = (int) sqrt((double) limit);
	int64_t count = (limit < 2) ? 0 : 1;

	// generate small primes <= sqrt
	vector<char> is_prime(small_size + 1, 1);
	for (int i = 2; i * i <= small_size; i++)
		if (is_prime[i])
			for (int j = i * i; j <= small_size; j += i)
				is_prime[j] = 0;
	count += seive_section(0,limit / 2,is_prime,segment_size);
	count += seive_section(limit / 2,limit,is_prime,segment_size);
	return count;
}

/// Usage: ./segmented_sieve n size
/// @param n     Sieve the primes up to n.
/// @param size  Size of the sieve array in bytes.
///
#include <ctime>
int main(int argc, char** argv)
{
	// generate the primes below this number
	int64_t limit = 1000000000;
	cin >> limit;
	int start = clock();
	if (argc >= 2)
		limit = atol(argv[1]);

	int size = L1D_CACHE_SIZE;
	if (argc >= 3)
		size = atoi(argv[2]);

	int count = segmented_sieve(limit, size);
	int end = clock();
	cout << end - start << "ms";
	cout << count << " primes found." << endl;
	return 0;
}
