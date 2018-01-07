The .cpp file is a c++ program that counts primes and times the execution time. 
More details are in comments in that file.

The excel files are recorded times on an I-5 3337-U cpu (2 hyperthreaded cores, L1=65kb,L2=512kb,L3=3Mb).

	The file name number is the number which it calculated primes up to.
	
	"gcc" indicates that it was compiled by cygwin's gcc version 4.9.3
	otherwise it was compiled by msvc++ 12.
	
	"bit" means that it used bit compaction via the standard library's vector<bool>
	otherwise it used vector<char> which is an ordinary dynamic array of bytes.
	
	"unseg" means that it did not use segmentation, instead it simply allocated the memory
	and workload between the threads (i.e. segment_size = cap_num / num_threads).