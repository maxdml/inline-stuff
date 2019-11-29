#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include <assert.h>

#include <iostream>
#include <atomic>

#include "benchmarks.hh"

//#define MULTITHREADED_HALF_ARRAY_READ 1

uint64_t n = (L2_SIZE / sizeof(uint64_t)) / 2;
volatile uint64_t *shared_array;
std::atomic<bool> sa_flag = false;

void bm_single_d_array_multithreaded(struct ThreadArgs &args)
{
    uint64_t *store_start = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR) * sizeof(uint64_t)));
    uint64_t *store_end = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR) * sizeof(uint64_t)));

    if (sa_flag == false) {

        printf("thread %d set up the shared array\n", args.id);
        sa_flag = true;
        std::cout << "Thread #" << args.id << " setting shared array" << std::endl;
        shared_array = static_cast<uint64_t*>(malloc(L2_SIZE));
        for (uint64_t i = 0; i < n; ++i)
            shared_array[i] = i;
    
    }
        clear_l1();
        clear_l2();

    unsigned int j, j_last;

    volatile uint64_t b;
    std::cout << "Thread #" << args.id << " accessing indices ";
    
    // j is initialized inside the loop now
    //std::cout << std::dec << j << " to " << std::dec << j_last << std::endl;
    for (unsigned int i = 0; i < args.iterations; ++i)
    {
#ifdef MULTITHREADED_HALF_ARRAY_READ
        if (args.id == 0)
        {
            j = 0;
            j_last = n/2;
        }
        else if (args.id == 1)
        {
            j = n/2 + 1;
            j_last = n;

        }
#else
        j = 0;
        j_last = n;
#endif

        //printf("running iteration %d\n", i);
        read_values(args.cpu_msr, store_start);
        for (uint64_t jj = j; jj < j_last; ++jj)
        {
            b = shared_array[jj];
            //__asm__ volatile("clflush (%0)" : : "r" ((volatile void *)& a[j]) : "memory");
        }
        read_values(args.cpu_msr, store_end);
        for (int c = 0; counter_tbl[c].name; ++c)
        {
            args.values(c, args.counts) = store_end[c] - store_start[c];
            //std::cout << counter_tbl[c].name << ": " << std::dec << args.values(c, args.counts) << std::endl;
        }
        fake_out_optimizations((uint64_t*)shared_array, L2_SIZE);
        args.counts++;
    }
    free(store_start);
    free(store_end);
}

void bm_cache_line_test(struct ThreadArgs &args)
{
    int n = L2_SIZE;
    uint8_t b;

    uint64_t *store_start = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR) * sizeof(uint64_t)));
    uint64_t *store_end = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR) * sizeof(uint64_t)));

    volatile uint8_t *arr = static_cast<uint8_t *>(malloc(L2_SIZE));
    for (int i = 0; i < n; i++) // we can fit L2_SIZE / sizeof(uint8_t) = L2_SIZE elements in L2
        arr[i] = i;

    clear_l2();

    for (unsigned int i = 0; i < args.iterations; ++i)
    {
        printf("running iteration %d\n", i);
        read_values(args.cpu_msr, store_start);

        for (int j = 0; j < 64; ++j)
        {
            b = arr[j];
            //__asm__ volatile("clflush (%0)" : : "r" ((volatile void *)& a[j]) : "memory");
        }
        read_values(args.cpu_msr, store_end);
        for (int c = 0; counter_tbl[c].name; ++c)
        {
            args.values(c, args.counts) = store_end[c] - store_start[c];
        }
        args.counts++;
        fake_out_optimizations((uint64_t *)arr, (long) n/8);
        for (int j = 64; j < n; j++)
            arr[j] = i*j;
        fake_out_optimizations((uint64_t *)arr, (long) n/8);
    }
    free(store_start);
    free(store_end);
}

void bm_single_d_array(struct ThreadArgs &args)
{
    printf("benchmark function for single dimensional array\n");
    uint64_t *store_start = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR) * sizeof(uint64_t)));
    uint64_t *store_end = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR) * sizeof(uint64_t)));

    std::cout << "Accessing 0x" << n << " elements" << std::endl;
    /* Fill the array */
    volatile uint64_t *a = static_cast<uint64_t *>(malloc(n * sizeof(uint64_t)));
    printf("Filling the array\n");
    for (uint64_t i = 0; i < n; ++i)
    {
        a[i] = i;
        //__asm__ volatile("clflush (%0)" : : "r" ((volatile void *)& a[i]) : "memory");
    }
    printf("array filled\n");
    clear_l2();

    volatile uint64_t b;

    /* Access it */
    for (unsigned int i = 0; i < args.iterations; ++i)
    {
        printf("running iteration %d\n", i);
        read_values(args.cpu_msr, store_start);

        for (uint64_t j = 0; j < n; ++j)
        {
            b = a[j];
            //__asm__ volatile("clflush (%0)" : : "r" ((volatile void *)& a[j]) : "memory");
        }
        read_values(args.cpu_msr, store_end);
        for (int c = 0; counter_tbl[c].name; ++c)
        {
            args.values(c, args.counts) = store_end[c] - store_start[c];
        }
        fake_out_optimizations((uint64_t*)a, sizeof(uint64_t));
        args.counts++;
    }

    printf("b = %lu\n", b);

    free(store_start);
    free(store_end);
}

#if 1
uint64_t arr_2d[1024][16];
void bm_2d_array(struct ThreadArgs &args)
{
    printf("benchmark function for single dimensional array\n");
    uint64_t *store_start = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR) * sizeof(uint64_t)));
    uint64_t *store_end = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR) * sizeof(uint64_t)));

    /* Fill the array */
    printf("Filling the array\n");
    for (uint64_t i = 0; i < 1024; ++i) 
    {
        for(uint64_t j = 0; j < 16; j++)
        arr_2d[i][j] = i + j;
    }
    printf("array filled\n");

    volatile uint64_t b;

    /* Access it */
    for (unsigned int i = 0; i < args.iterations; ++i) 
    {
        printf("running iteration %d\n", i);
        read_values(args.cpu_msr, store_start);
        
        for (uint64_t i = 0; i < 1024; ++i) 
        {
            for(uint64_t j = 0; j < 16; j++)
            b = arr_2d[i][j];
            //__asm__ volatile("clflush (%0)" : : "r" ((volatile void *)& a[j]) : "memory");
        }
        read_values(args.cpu_msr, store_end);
        for (int c = 0; counter_tbl[c].name; ++c) 
        {
            args.values(c, args.counts) = store_end[c] - store_start[c];
        }
        args.counts++;
    }

    printf("b = %lu\n", b);
    free(store_start);
    free(store_end);
    
}
#endif

void bm_2d_array_non_cont(struct ThreadArgs &args)
{
    printf("benchmark function for single dimensional array\n");
    uint64_t *store_start = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR) * sizeof(uint64_t)));
    uint64_t *store_end = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR) * sizeof(uint64_t)));

    /* Fill the array */
    printf("Filling the array\n");
    for (uint64_t i = 0; i < 1024; ++i) 
    {
        for(uint64_t j = 0; j < 16; j++)
        arr_2d[i][j] = i + j;
    }
    printf("array filled\n");

    volatile uint64_t b;

    /* Access it */
    for (unsigned int i = 0; i < args.iterations; ++i) 
    {
        printf("running iteration %d\n", i);
        read_values(args.cpu_msr, store_start);
        
        for (uint64_t j = 0; j < 16; ++j) 
        {
            for(uint64_t i = 0; i < 1024; i++)
            b = arr_2d[i][j];
            //__asm__ volatile("clflush (%0)" : : "r" ((volatile void *)& a[j]) : "memory");
        }
        read_values(args.cpu_msr, store_end);
        for (int c = 0; counter_tbl[c].name; ++c) 
        {
            args.values(c, args.counts) = store_end[c] - store_start[c];
        }
        args.counts++;
    }

    printf("b = %lu\n", b);
    free(store_start);
    free(store_end);
}
/**
 * 32KB = 32768B => 4096 * 8 uint64_t
 */
void oned_arrays(struct ThreadArgs &args) {
    printf("Starting MB worker %d\n", args.id);
    uint64_t *store_start = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR + N_FIXED_CTR) * sizeof(uint64_t)));
    uint64_t *store_end = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR + N_FIXED_CTR) * sizeof(uint64_t)));

    /* Fill the array and the cache */
    volatile uint64_t *a = static_cast<uint64_t *>(malloc(L2_SIZE));
    for (int i = 0; i < L2_SIZE; ++i) {
        a[i] = xorshift128plus(mat_rng) >> 11;
        /* Flush the data we just loaded. We could do that every 8 i (clflush is for a line) */
        __asm__ volatile("mfence; clflush (%0)" : : "r" ((volatile void *)& a[i]) : "memory");
    }
    //__builtin___clear_cache((void *)a, (void *) (a+(n * sizeof(uint64_t))+1));

    int n = 1;
    //volatile uint64_t *b = static_cast<uint64_t *>(malloc(sizeof(uint64_t) * n));
    uint64_t c = 0;
    /* Access it */
    for (unsigned int i = 0; i < args.iterations; ++i) {
        srlz();
        read_values(args.cpu_msr, store_start);
        /* 2n loads */
        /*
        for (int j = 0; j < n; ++j) {
            //b[j] = a[j];
            c = a[j];
            //__asm__ volatile("clflush (%0)" : : "r" ((volatile void *)& c) : "memory");
            fake_out_optimizations(&c, sizeof(uint64_t));
        }
        */
        read_values(args.cpu_msr, store_end);
        srlz();
        for (int c = 0; counter_tbl[c].name; ++c) {
            args.values(c, args.counts) = store_end[c] - store_start[c];
        }
        args.counts++;
        //fake_out_optimizations((uint64_t *)b, sizeof(uint64_t) * n);
    }

    free(store_start);
    free(store_end);
}

void cache_work(struct ThreadArgs &args) {
    uint64_t store_start[N_CUSTOM_CTR + N_FIXED_CTR];
    uint64_t store_end[N_CUSTOM_CTR + N_FIXED_CTR];
    uint32_t i, j, k;
    uint32_t nrows = 50;
    uint32_t ncols = 10;
    uint64_t a[nrows][ncols];
    /* Init matrix */
    for (i = 0; i < nrows; ++i) {
        for (j = 0; j < ncols; ++j) {
            a[i][j] = xorshift128plus(mat_rng) >> 11;
            __asm__ volatile("clflush (%0)" : : "r" ((volatile void *)& a[i][j]) : "memory");
        }
    }
    /* Access it */
    for (k = 0; k < args.iterations; ++k) {
        read_values(args.cpu_msr, store_start);
    /*
        for (j = 0; j < nrows; ++j) {
            for (i = 0; i < ncols; ++i) {
                a[i][j] = 2 * a[i][j];
            }
        }
    */
        for (i = 0; i < ncols; ++i) {
            for (j = 0; j < nrows; ++j) {
                a[i][j] = 2 * a[i][j];
            }
        }
        read_values(args.cpu_msr, store_end);
        for (int c = 0; counter_tbl[c].name; ++c) {
            args.values(c, args.counts) = store_end[c] - store_start[c];
        }
        args.counts++;
    }
}
// Just less than L2:
// 1023K /media/memfs/wiki/fr/h/e/x/Utilisateur~Hexasoft_Statistiques_Geckonidé_41ae.html
// 1018K /media/memfs/wiki/fr/c/l/n/Utilisateur~Cln-id_monobook.js_9900.html

// Just above L2:
// 1.2M /media/memfs/wiki/fr/b/u/l/Wikipédia~Bulletin_des_administrateurs_Février_2007_8e0a.html
// 1.2M /media/memfs/wiki/fr/s/o/b/Utilisateur~Sobreira_Carnivora_a3cd.html

// less than 2 L3 ways:
// 2.2M /media/memfs/wiki/fr/i/t/a/Projet~Italie_Catégorisation_f5a3.html
// 2.1M /media/memfs/wiki/fr/z/x/8/Utilisateur~ZX81-bot_Journaux_2007042701_125f.html

void file_work(struct ThreadArgs &args) {
    std::string f1 = "/media/memfs/wiki/fr/h/e/x/Utilisateur~Hexasoft_Statistiques_Geckonidé_41ae.html";
    std::string f2 = "/media/memfs/wiki/fr/c/l/n/Utilisateur~Cln-id_monobook.js_9900.html";
    //std::string f1 = "/media/memfs/wiki/fr/b/u/l/Wikipédia~Bulletin_des_administrateurs_Février_2007_8e0a.html";
    //std::string f2 = "/media/memfs/wiki/fr/s/o/b/Utilisateur~Sobreira_Carnivora_a3cd.html";
    //std::string f1 = "/media/memfs/wiki/fr/i/t/a/Projet~Italie_Catégorisation_f5a3.html";
    //std::string f2 = "/media/memfs/wiki/fr/z/x/8/Utilisateur~ZX81-bot_Journaux_2007042701_125f.html";

    for (uint32_t i = 0; i < args.iterations; ++i) {
        FILE *file = NULL;
        if (i%2 == 0) {
            file = fopen(f1.c_str(), "rb");
        } else {
            file = fopen(f2.c_str(), "rb");
        }
        if (fseek(file, 0, SEEK_END) == -1) {
            std::cout << "fseek error: " << strerror(errno) << std::endl;
        }
        int size = ftell(file);
        if (size == -1) {
            std::cout << "Error telling file size: " << strerror(errno) << std::endl;
        }
        if (fseek(file, 0, SEEK_SET) == -1) {
            std::cout << "fseek error: " << strerror(errno) << std::endl;
        }
        char *body = static_cast<char *>(malloc(size+1));
        size_t char_read = fread(body, sizeof(char), size, file);
        if (char_read < (unsigned) size) {
            std::cout << "fread() read less bytes than file's size." << std::endl;
        }
        if (strstr(body, "somedummystring") != NULL) {
            std::cout << "Dummy string match. Stunning." << std::endl;
        }
        free(body);
        body = NULL;
        fclose(file);
    }
    pthread_exit(NULL);
}
