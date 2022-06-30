#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <emmintrin.h>
 
const int till = 32*64*16;
 
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

    for (int i = 0; i < 32; i++)
        {
            temp = p[i*16*64];
        }

    // unsigned int time[32];
    // for(int i = 0; i <32; i++) time[i] = 0;

    clcache();

     for (int i = 0; i < 32; i++)
        {
            temp = p[i*16*64];

            for (int k = 0; k < i+1; k++)
            {
                asm volatile(
                    "CPUID\n\t"
                    "RDTSC\n\t"
                    "mov %%edx, %0\n\t"
                    "mov %%eax, %1\n\t"
                    : "=r"(cycles_high), "=r"(cycles_low)::"%rax", "%rbx", "rcx", "%rdx");

                temp = p[k*16*64];

                asm volatile(
                    "RDTSCP\n\t"
                    "mov %%edx, %0\n\t"
                    "mov %%eax, %1\n\t"
                    "CPUID\n\t"
                    : "=r"(cycles_high1), "=r"(cycles_low1)::"%rax", "%rbx", "rcx", "%rdx");

                start = (((uint64_t)cycles_high << 32) | cycles_low);
                end = (((uint64_t)cycles_high1 << 32) | cycles_low1);

                printf("for block %d: %ld\n", i, (end - start));

                // time[i] += (end - start);
            }
            printf("\n");
        }

    clcache();
 
    // for(int i = 0; i<32; i++) printf("for the index %d : %u \n", i, time[i]);
 
    free(p);
 
    return 0;
}