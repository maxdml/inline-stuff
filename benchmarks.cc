#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <iostream>

#include "benchmarks.hh"

void cache_work(struct ThreadArgs &args) {
    uint64_t store_start[N_CUSTOM_CTR + N_FIXED_CTR];
    uint64_t store_end[N_CUSTOM_CTR + N_FIXED_CTR];
    uint32_t i, j, k;
    /* Init matrix */
    for (i = 0; i < 100; ++i) {
        for (j = 0; j < 100; ++j) {
            args.a[i][j] = xorshift128plus(mat_rng) >> 11;
            __asm__ volatile("clflush (%0)" : : "r" ((volatile void *)& args.a[i][j]) : "memory");
        }
    }

    /* Access it */
    for (k = 0; k < args.iterations; ++k) {
        read_values(args.cpu_msr, store_start);
        for (j = 0; j < 100; ++j) {
            for (i = 0; i < 100; ++i) {
                args.a[i][j] = 2 * args.a[i][j];
            }
        }
    /*
        for (i = 0; i < 5000; ++i) {
            for (j = 0; j < 100; ++j) {
                args.a[i][j] = 2 * args.a[i][j];
            }
        }
    */
        read_values(args.cpu_msr, store_end);
        for (int i = 0; counter_tbl[i].name; ++i) {
            args.values(i, args.counts) = store_end[i] - store_start[i];
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
    args.f1 = "/media/memfs/wiki/fr/h/e/x/Utilisateur~Hexasoft_Statistiques_Geckonidé_41ae.html";
    args.f2 = "/media/memfs/wiki/fr/c/l/n/Utilisateur~Cln-id_monobook.js_9900.html";
    //std::string f1 = "/media/memfs/wiki/fr/b/u/l/Wikipédia~Bulletin_des_administrateurs_Février_2007_8e0a.html";
    //std::string f2 = "/media/memfs/wiki/fr/s/o/b/Utilisateur~Sobreira_Carnivora_a3cd.html";
    //std::string f1 = "/media/memfs/wiki/fr/i/t/a/Projet~Italie_Catégorisation_f5a3.html";
    //std::string f2 = "/media/memfs/wiki/fr/z/x/8/Utilisateur~ZX81-bot_Journaux_2007042701_125f.html";

    for (uint32_t i = 0; i < args.iterations; ++i) {
        FILE *file = NULL;
        if (i%2 == 0) {
            file = fopen(args.f1.c_str(), "rb");
        } else {
            file = fopen(args.f2.c_str(), "rb");
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
