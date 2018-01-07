#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <stdint.h>
#include <string>
#include <thread>
#include <ctime>
using namespace std;
typedef vector<char> seive_type;

vector<int64_t> counts;
void seive_section(int64_t low_lim,int64_t limit,seive_type & is_prime,int t_num,int segment_size = L1D_CACHE_SIZE)
{
	
}

int64_t segmented_sieve(int64_t limit, int num_of_threads, int segment_size = L1D_CACHE_SIZE)
{
	if (limit < 2)
		return 0;
	int small_size = (int)sqrt((double)limit);
	int64_t count = (limit < 2) ? 0 : 1;

	// generate small primes <= sqrt
	seive_type is_prime(small_size + 1, 1);
	for (int i = 2; i * i < small_size; i++)
		if (is_prime[i])
			for (int j = i * i; j < small_size; j += i)
				is_prime[j] = 0;

	vector<thread> threads;
	counts.resize(num_of_threads);
	for (int64_t low = 2,n = 0; low <= limit;n++, low += limit / num_of_threads){
		int64_t high = min(low + limit / num_of_threads - 1, limit);
		threads.push_back(thread(seive_section,low, high, is_prime,n,segment_size));
	}
	for (thread & t : threads)
		t.join();
	for (int64_t c : counts)
		count += c;
	return count;
}
int main(int argc, char** argv)
{
	// generate the primes below this number
	int64_t limit = 200000000;
	cout << "threads|";
	for (int64_t c = 1LL << 16; c < (1LL << 19); c <<= 1){
		cout << c << "|";
	}
	cout << "\n";
	for (int t = 1; t < 8; t++){
		cout << "      " << t << "|";
		for (int64_t c = 1LL << 16; c < (1LL << 19); c <<= 1){
			int start = clock();
			int count = segmented_sieve(limit, t, c);
			int time = clock() - start;

			string spaces(" ",6 - int(log10(time + 1.0)));
			cout << spaces << time << "|";
		}
		cout << "\n";
	}
	cin.get();
	return 0;
}