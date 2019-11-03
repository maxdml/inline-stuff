#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <iostream>
#include <stdint.h>
#include <unistd.h>

#include "benchmarks.hh"

void run_test_benchmark(MsrHandle* cpu_msr)
{
    if (cpu_msr == NULL)
    {
        printf("handle is NULL\n");
        return;
    }
    uint8_t arr[64] = {0};
    uint8_t c;
    uint64_t start_ref, end_ref, start_miss, end_miss; 

    cpu_msr->read(IA32_PMC0, &start_miss); 
    cpu_msr->read(IA32_PMC1, &start_ref); 
    printf("start miss: %lu\n", start_miss);
    printf("start ref: %lu\n", start_ref);
    
    //uint64_t store_start[2];
    //read_values(cpu_msr, store_start);
    for(int i = 0; i < 64; i++)
    {
        c = arr[i];
    }

    sleep(1);
    
    cpu_msr->read(IA32_PMC0, &end_miss); 
    cpu_msr->read(IA32_PMC1, &end_ref); 
    printf("end miss: %lu\n", end_miss);
    printf("end ref: %lu\n", end_ref);
    printf("total misses: %lu\n", end_miss - start_miss);
    printf("total references: %lu\n", end_ref - start_ref);
    
    //uint64_t store_end[2];
    //read_values(cpu_msr, store_end);
    //printf("%lu\n", store_end[0] - store_start[0]);
    //printf("%lu\n", store_end[1] - store_start[1]);

}

void cache_work(struct ThreadArgs &args) 
{
    args.iterations = 5;
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

void file_work(struct ThreadArgs args) {
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

