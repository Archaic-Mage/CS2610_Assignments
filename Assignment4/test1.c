#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <emmintrin.h>
 
const int till = 128;
const int reptill = 4;
 
int *p;
 
int temp;

void clcache() 
{
    for(int i = 0; i<till; i++) _mm_clflush(&p[i]);
}
 
int main() {

    p = (int*) malloc(till*sizeof(int));
    
    unsigned cycles_high, cycles_low, cycles_high1, cycles_low1;
    uint64_t start, end;

    unsigned int time[till];
    for(int i = 0; i <till; i++) time[i] = 0;

    clcache();

    for (int j = 0; j < reptill; j++)
    {
        for (int i = 0; i < till; i++)
        {

            asm volatile(
                "CPUID\n\t"
                "RDTSC\n\t"
                "mov %%edx, %0\n\t"
                "mov %%eax, %1\n\t"
                : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "rcx", "%rdx");

            temp = p[i];

            asm volatile(
                "RDTSCP\n\t"
                "mov %%edx, %0\n\t"
                "mov %%eax, %1\n\t"
                "CPUID\n\t"
                : "=r"(cycles_high1), "=r"(cycles_low1)::"%rax", "%rbx", "rcx", "%rdx");

            start = (((uint64_t)cycles_high << 32) | cycles_low);
            end = (((uint64_t)cycles_high1 << 32) | cycles_low1);

            // printf("for index %d: %ld\n", i, end - start);

            time[i] += (end - start);
        }

        clcache();
    }

    for(int i =0; i<till; i++) time[i]/=reptill;
 
    for(int i = 0; i<till; i++) printf("for the index %d : %u \n", i, time[i]);
 
    free(p);
 
    return 0;
}