#ifndef SERVEUR_H
#define SERVEUR_H

#include <fcntl.h>       // Pour shm_open
#include <sys/mman.h>    // Pour mmap, munmap
#include <sys/stat.h>    // Pour les permissions (mode_t)
#include <pthread.h>
#include <unistd.h>      // Pour ftruncate, close
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <wait.h>
#include <semaphore.h>   // Pour les sémaphores

#define SHM_NAME "/shared_matrices"  // Nom de l'objet de mémoire partagée
#define SEM_A_READY "/sem_a_ready"  // Sémaphore pour la matrice A
#define MAX_CLIENTS 10
#define MAX_SIZE 1000

typedef struct {
  int matrice_A[MAX_SIZE][MAX_SIZE];
  int matrice_B[MAX_SIZE][MAX_SIZE];
  int matrice_C[MAX_SIZE][MAX_SIZE];
  int rows_a, cols_a;
  int cols_b;
} Matrices;

typedef struct {
  int i, j;
  Matrices *matrices;
} ThreadArgs;

#include <stdio.h>
#include <stdlib.h>

// Structures
typedef struct {
  pid_t pid;
  int borne_sup;
  int taille_n, taille_m, taille_p;
} Requete;
#endif
