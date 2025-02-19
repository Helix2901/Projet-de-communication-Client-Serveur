#include "serveur.h"
#include <stdlib.h>
#include <stdio.h>

// Fonction pour calculer un élément de la matrice C
void *compute_element(void *arg) {
    ThreadArgs *args = (ThreadArgs *) arg;
    int i = args->i;
    int j = args->j;
    Matrices *matrices = args->matrices;
    matrices->matrice_C[i][j] = 0;
    for (int k = 0; k < matrices->cols_a; k++) {
        matrices->matrice_C[i][j] += matrices->matrice_A[i][k] * matrices->matrice_B[k][j];
    }
    pthread_exit(0);
}

// Gestionnaire de signal pour nettoyer les processus zombies
void handle_sigchld(int sig) {
  int i =sig;
  i++;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main(void) {
    char request_pipe[] = "Srequest_pipe";

    // Création du tube nommé pour les requêtes
    if (access(request_pipe, F_OK) == -1) {
        if (mkfifo(request_pipe, S_IRUSR | S_IWUSR) == -1) {
            perror("Erreur lors de la création du tube nommé");
            return 1;
        }
    } else {
        printf("Le tube nommé existe déjà, utilisation du tube existant.\n");
    }

    // Création d'un sémaphore global pour limiter le nombre de clients
    sem_t *sem_clients = sem_open("/sem_clients", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, MAX_CLIENTS);
    if (sem_clients == SEM_FAILED) {
        perror("Erreur lors de la création du sémaphore global");
        exit(EXIT_FAILURE);
    }
    sem_unlink("/sem_clients");

    // Configuration du gestionnaire de signal pour éviter les processus zombies
    struct sigaction sa;
    sa.sa_handler = handle_sigchld;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("Erreur lors de la configuration du gestionnaire de signal");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Écouter les requêtes du client
        int fd = open(request_pipe, O_RDONLY);
        if (fd == -1) {
            perror("Erreur lors de l'ouverture du tube de requête");
            exit(EXIT_FAILURE);
        }

        Requete req;
        if (read(fd, &req, sizeof(req)) == -1) {
            perror("Erreur lors de la lecture de la requête");
            close(fd);
            continue;
        }
        close(fd);

        printf("Requête reçue du client avec PID %d...\n", req.pid);

        // Créer un processus enfant pour traiter la requête
        pid_t pid = fork();
        if (pid == -1) {
            perror("Erreur lors du fork");
            continue;
        }

        if (pid == 0) { // Processus enfant
            sem_wait(sem_clients);

            sem_t *sem_p = sem_open(SEM_A_READY, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
            if (sem_p == SEM_FAILED) {
                perror("Erreur lors de l'ouverture du sémaphore");
                exit(EXIT_FAILURE);
            }
            if (sem_unlink(SEM_A_READY) == -1) {
                perror("Erreur lors de la suppression du sémaphore");
                exit(EXIT_FAILURE);
            }

            // Création et gestion de la mémoire partagée
            int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
            if (shm_fd == -1) {
                perror("Erreur lors de la création de l'objet de mémoire partagée");
                exit(EXIT_FAILURE);
            }
            if (shm_unlink(SHM_NAME) == -1) {
                perror("Erreur lors de la suppression de l'objet de mémoire partagée");
                exit(EXIT_FAILURE);
            }

            size_t shm_size = sizeof(Matrices);
            if (ftruncate(shm_fd, shm_size) == -1) {
                perror("Erreur lors du redimensionnement de la mémoire partagée");
                exit(EXIT_FAILURE);
            }

            Matrices *matrices = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
            if (matrices == MAP_FAILED) {
                perror("Erreur lors du mapping de la mémoire partagée");
                exit(EXIT_FAILURE);
            }
            close(shm_fd);

            // Stocker les dimensions des matrices
            matrices->rows_a = req.taille_n;
            matrices->cols_a = req.taille_m;
            matrices->cols_b = req.taille_p;

            // Générer la matrice A
            for (int i = 0; i < matrices->rows_a; i++) {
                for (int j = 0; j < matrices->cols_a; j++) {
                    matrices->matrice_A[i][j] = rand() % req.borne_sup;
                }
            }
            sem_post(sem_p);

            // Fork pour Worker B
            if (fork() == 0) { // Worker B
                for (int i = 0; i < matrices->cols_a; i++) {
                    for (int j = 0; j < matrices->cols_b; j++) {
                        matrices->matrice_B[i][j] = rand() % req.borne_sup;
                    }
                }

                sem_wait(sem_p);

                pthread_t threads[matrices->rows_a][matrices->cols_b];
                ThreadArgs args[matrices->rows_a][matrices->cols_b];
                for (int i = 0; i < matrices->rows_a; i++) {
                    for (int j = 0; j < matrices->cols_b; j++) {
                        args[i][j].i = i;
                        args[i][j].j = j;
                        args[i][j].matrices = matrices;
                        pthread_create(&threads[i][j], NULL, compute_element, &args[i][j]);
                    }
                }

                for (int i = 0; i < matrices->rows_a; i++) {
                    for (int j = 0; j < matrices->cols_b; j++) {
                        pthread_join(threads[i][j], NULL);
                    }
                }

                exit(0);
            }

            wait(NULL);

            // Envoyer les matrices au client
            char response_pipe[256];
            sprintf(response_pipe, "response_pipe_%d", req.pid);
            int fd_response = open(response_pipe, O_WRONLY);
            if (fd_response == -1) {
                perror("Erreur lors de l'ouverture du tube de réponse");
                exit(EXIT_FAILURE);
            }

int array_a[(matrices->rows_a) * (matrices->cols_a)];
          int array_b[(matrices->cols_a) * (matrices->cols_b)];
          int array_c[(matrices->rows_a) * (matrices->cols_b)];
          int k = 0;
          for (int i = 0; i < matrices->rows_a; i++) {
            for (int j = 0; j < matrices->cols_a; j++) {
              array_a[k] = matrices->matrice_A[i][j];
              k++;
            }
          }
          k = 0;
          for (int i = 0; i < matrices->cols_a; i++) {
            for (int j = 0; j < matrices->cols_b; j++) {
              array_b[k] = matrices->matrice_B[i][j];
              k++;
            }
          }
          k = 0;
          for (int i = 0; i < matrices->rows_a; i++) {
            for (int j = 0; j < matrices->cols_b; j++) {
              array_c[k] = matrices->matrice_C[i][j];
              k++;
            }
          }
          write(fd_response, &(matrices->rows_a), sizeof(int));
          write(fd_response, &(matrices->cols_a), sizeof(int));
          write(fd_response, &(matrices->cols_b), sizeof(int));
          write(fd_response, array_a,
              sizeof(int) * matrices->rows_a * matrices->cols_a);
          write(fd_response, array_b,
              sizeof(int) * matrices->cols_a * matrices->cols_b);
          write(fd_response, array_c,
              sizeof(int) * matrices->rows_a * matrices->cols_b);
                     printf("Requête du client avec PID %d traitée avec succès.\n",
              req.pid);
          close(fd_response);
            munmap(matrices, shm_size);
            sem_post(sem_clients);
            exit(0);
        }
    }

    return 0;
}
