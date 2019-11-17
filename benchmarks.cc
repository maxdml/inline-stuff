#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <iostream>
#include <stdint.h>
#include <unistd.h>

#include "benchmarks.hh"

void bm_cache_line_test(struct ThreadArgs &args)
{
    uint8_t arr[32];
    uint8_t b;

    uint64_t *store_start = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR) * sizeof(uint64_t)));
    uint64_t *store_end = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR) * sizeof(uint64_t)));

    for(int i = 0; i < 32; i++)
        arr[i] = i;

    for (unsigned int i = 0; i < args.iterations; ++i) 
    {
        printf("running iteration %d\n", i);
        read_values(args.cpu_msr, store_start);
        
        for (int j = 0; j < 32; ++j) 
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
    }
    free(store_start);
    free(store_end);
}

void bm_single_d_array(struct ThreadArgs &args)
{
    printf("benchmark function for single dimensional array\n");
    uint64_t *store_start = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR) * sizeof(uint64_t)));
    uint64_t *store_end = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR) * sizeof(uint64_t)));

    uint64_t n = 16384; // Moe than size of L2 which is 256K

    /* Fill the array */
    volatile uint64_t *a = static_cast<uint64_t *>(malloc(sizeof(uint64_t) * n));
    printf("Filling the array\n");
    for (uint64_t i = 0; i < n; ++i) 
    {
        a[i] = i;
    }
    printf("array filled\n");

    volatile uint64_t b;

    /* Access it */
    for (unsigned int i = 0; i < args.iterations; ++i) 
    {
        printf("running iteration %d\n", i);
        read_values(args.cpu_msr, store_start);
        
        for (int j = 0; j < n; ++j) 
        {
            b = a[j];
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

#if 1
uint64_t arr_2d[1024][64];
void bm_2d_array(struct ThreadArgs &args)
{
    printf("benchmark function for single dimensional array\n");
    uint64_t *store_start = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR) * sizeof(uint64_t)));
    uint64_t *store_end = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR) * sizeof(uint64_t)));

    /* Fill the array */
    printf("Filling the array\n");
    for (uint64_t i = 0; i < 1024; ++i) 
    {
        for(uint64_t j = 0; j < 64; j++)
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
            for(uint64_t j = 0; j < 64; j++)
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
        for(uint64_t j = 0; j < 64; j++)
        arr_2d[i][j] = i + j;
    }
    printf("array filled\n");

    volatile uint64_t b;

    /* Access it */
    for (unsigned int i = 0; i < args.iterations; ++i) 
    {
        printf("running iteration %d\n", i);
        read_values(args.cpu_msr, store_start);
        
        for (uint64_t j = 0; j < 64; ++j) 
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
void oned_arrays(struct ThreadArgs &args) {
    printf("Starting MB worker %d\n", args.id);
    uint64_t *store_start = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR + N_FIXED_CTR) * sizeof(uint64_t)));
    uint64_t *store_end = static_cast<uint64_t *>(malloc((N_CUSTOM_CTR + N_FIXED_CTR) * sizeof(uint64_t)));

    int n = 4096; // 512 * 8

    /* Fill the array */
    volatile uint64_t *a = static_cast<uint64_t *>(malloc(sizeof(uint64_t) * n));
    for (int i = 0; i < n; ++i) {
        a[i] = xorshift128plus(mat_rng) >> 11;
    }

    volatile uint64_t *b = static_cast<uint64_t *>(malloc(sizeof(uint64_t) * n));
    /* Access it */
    for (unsigned int i = 0; i < args.iterations; ++i) {
        read_values(args.cpu_msr, store_start);
        for (int j = 0; j < n; ++j) {
            b[j] = a[j];
            //__asm__ volatile("clflush (%0)" : : "r" ((volatile void *)& a[j]) : "memory");
        }
        read_values(args.cpu_msr, store_end);
        for (int c = 0; counter_tbl[c].name; ++c) {
            args.values(c, args.counts) = store_end[c] - store_start[c];
        }
        args.counts++;
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
