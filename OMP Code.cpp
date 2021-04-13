#include <unistd.h>
#include <cstdlib>
#include <omp.h>
#include <cstdio>
#define N 33'554'432 // 2^25

int main() {
    #pragma omp parallel for schedule(dynamic) num_threads(64)
    for (unsigned int i = 0; i < N; i += 65536) {
        sleep(1);
        int proc = omp_get_thread_num();
        printf("Thread %d is ready to work within range [%d, %d).\n", proc, i, (i + 65536));
    }
    return 0;
}