#include "client.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc != 5) {
    fprintf(stderr, "Usage: %s <borne_sup> <taille_n> <taille_m> <taille_p>\n",
        argv[0]);
    exit(EXIT_FAILURE);
  }
  Requete req;
  req.pid = getpid();
  // Lire les paramètres depuis argv
  req.borne_sup = atoi(argv[1]);
  req.taille_n = atoi(argv[2]);
  req.taille_m = atoi(argv[3]);
  req.taille_p = atoi(argv[4]);
  // Vérifier si les valeurs sont valides
  if (req.borne_sup <= 0 || req.taille_n <= 0 || req.taille_m <= 0
      || req.taille_p <= 0) {
    fprintf(stderr,
        "Erreur : Les dimensions et la borne supérieure doivent être des\
         entiers positifs.\n");
    exit(EXIT_FAILURE);
  }
  // Vérifier si les tailles des matrices ne dépassent pas MAX_SIZE
  if (req.taille_n > MAX_SIZE || req.taille_m > MAX_SIZE
      || req.taille_p > MAX_SIZE) {
    fprintf(stderr,
        "Erreur : La taille des matrices dépasse la limite de %d.\n",
        MAX_SIZE);
    exit(EXIT_FAILURE);
  }
  // Créer un tube nommé unique pour lire la réponse du serveur
  char response_pipe[256];
  sprintf(response_pipe, "response_pipe_%d", req.pid);
  if (mkfifo(response_pipe, S_IRUSR | S_IWUSR) == -1) {
    perror("Erreur lors de la création du tube de réponse");
    exit(EXIT_FAILURE);
  }
  // Envoyer la requête au serveur via un tube nommé existant
  char request_pipe[] = "Srequest_pipe"; // Correspond au pipe utilisé par le
                                         // serveur
  int fd;
  if ((fd = open(request_pipe, O_WRONLY)) == -1) {
    perror("Erreur lors de l'ouverture du tube de requête");
    exit(EXIT_FAILURE);
  }
  if (write(fd, &req, sizeof(Requete)) == -1) {
    perror("Erreur lors de l'écriture dans le tube de requête");
    close(fd);
    exit(EXIT_FAILURE);
  }
  close(fd);
  // Ouvrir le tube de réponse pour recevoir les matrices A, B et C
  int fd_response = open(response_pipe, O_RDONLY);
  if (fd_response == -1) {
    perror("Erreur lors de l'ouverture du tube de réponse");
    exit(EXIT_FAILURE);
  }
  // Lire les dimensions des matrices
  int rows_a, cols_a, cols_b;
  //read(fd_response, matrice_A, sizeof(int) * rows_a * cols_a);
  read(fd_response, &rows_a, sizeof(int));
  read(fd_response, &cols_a, sizeof(int));
  read(fd_response, &cols_b, sizeof(int));
  // Allouer dynamiquement les matrices en fonction des dimensions lues
  int matrice_A[rows_a * cols_a];
  int matrice_B[cols_a * cols_b];
  int matrice_C[rows_a * cols_b];
  // Lire les données des matrices
  read(fd_response, matrice_A, sizeof(int) * rows_a * cols_a);
  read(fd_response, matrice_B, sizeof(int) * cols_a * cols_b);
  read(fd_response, matrice_C, sizeof(int) * rows_a * cols_b);
  close(fd_response);
  // Afficher la matrice A
  int k = 0;
  printf("Client: %d\nMatrice A:\n", req.pid);
  for (int i = 0; i < cols_a; i++) {
    for (int j = 0; j < rows_a; j++) {
      printf("%d ", matrice_A[k]);
      k++;
    }
    printf("\n");
  }
  k = 0;
  // Afficher la matrice B
  printf("Matrice B:\n");
  for (int i = 0; i < cols_a; i++) {
    for (int j = 0; j < cols_b; j++) {
      printf("%d ", matrice_B[k]);
      k++;
    }
    printf("\n");
  }
  k = 0;
  // Afficher la matrice résultat C
  printf("Matrice résultat C:\n");
  for (int i = 0; i < rows_a; i++) {
    for (int j = 0; j < cols_b; j++) {
      printf("%d ", matrice_C[k]);
      k++;
    }
    printf("\n");
  }
  // Nettoyer le tube de réponse
  //unlink(response_pipe);
  return 0;
}
