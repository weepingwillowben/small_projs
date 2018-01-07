#include <iostream>
#include <thread>
#include <vector>

static const int num_threads = 10;

//This function will be called from a thread
struct int_t{
	int *tid;
};
void call_from_thread(int_t tid) {
    std::cout << "Launched by thread " << tid.tid << std::endl;
}

int main() {
    std::vector<std::thread> t(num_threads);

    //Launch a group of threads
    for (int i = 0; i < num_threads; ++i) {
        t[i] = std::thread(call_from_thread, int_t{&i});
    }

    std::cout << "Launched from the main\n";

    //Join the threads with the main thread
    for (int i = 0; i < num_threads; ++i) {
        t[i].join();
    }

    return 0;
}