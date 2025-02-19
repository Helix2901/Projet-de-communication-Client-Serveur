#ifndef CLIENT_H
#define CLIENT_H


#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define MAX_SIZE 100 // Définir une taille maximale pour les matrices

// Structures
typedef struct {
  pid_t pid;
  int borne_sup;
  int taille_n, taille_m, taille_p;
} Requete;

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

#endif
