#include <stdio.h>
#include <string.h>

#define MAX_SALLES 100
#define MAX_RESERVATIONS 1000

typedef struct {
    char nom[50];
    int capacite;
    float tarif_horaire;
    char equipements[200];
} Salle;

typedef struct {
    int id;
    char nom_client[50];
    int index_salle;
    char date[11];       // "DD-MM-YYYY"
    int heure_debut;
    int heure_fin;
    int nb_personnes;
    float tarif;
    int statut;          // 0=en attente, 1=confirmee, 2=annulee
} Reservation;

Salle salles[MAX_SALLES];
int nbSalles = 0;

Reservation reservations[MAX_RESERVATIONS];
int nbReservations = 0;

void sauvegarderSalles() {
    FILE *f = fopen("salles.txt", "w");
    int i;

    for (i = 0; i < nbSalles; i++) {
        fprintf(f, "%s;%d;%.2f;%s\n",
                salles[i].nom,
                salles[i].capacite,
                salles[i].tarif_horaire,
                salles[i].equipements);
    }
    fclose(f);
}

void chargerSalles() {
    FILE *f = fopen("salles.txt", "r");

    nbSalles = 0;
    while (nbSalles < MAX_SALLES &&
           fscanf(f, " %49[^;];%d;%f;%199[^\n]",
                  salles[nbSalles].nom,
                  &salles[nbSalles].capacite,
                  &salles[nbSalles].tarif_horaire,
                  salles[nbSalles].equipements) == 4) {
        nbSalles++;
    }
    fclose(f);
}

void sauvegarderReservations() {
    FILE *f = fopen("reservations.txt", "w");
    int i;

    for (i = 0; i < nbReservations; i++) {
        fprintf(f, "%d;%s;%d;%s;%d;%d;%d;%.2f;%d\n",
                reservations[i].id,
                reservations[i].nom_client,
                reservations[i].index_salle,
                reservations[i].date,
                reservations[i].heure_debut,
                reservations[i].heure_fin,
                reservations[i].nb_personnes,
                reservations[i].tarif,
                reservations[i].statut);
    }
    fclose(f);
}

void chargerReservations() {
    FILE *f = fopen("reservations.txt", "r");

    nbReservations = 0;
    while (nbReservations < MAX_RESERVATIONS &&
           fscanf(f, "%d;%49[^;];%d;%10[^;];%d;%d;%d;%f;%d",
                  &reservations[nbReservations].id,
                  reservations[nbReservations].nom_client,
                  &reservations[nbReservations].index_salle,
                  reservations[nbReservations].date,
                  &reservations[nbReservations].heure_debut,
                  &reservations[nbReservations].heure_fin,
                  &reservations[nbReservations].nb_personnes,
                  &reservations[nbReservations].tarif,
                  &reservations[nbReservations].statut) == 9) {
        nbReservations++;
    }
    fclose(f);
}

void chargerDepuisFichiers() {
    chargerSalles();
    chargerReservations();
}

void sauvegarderDansFichiers() {
    sauvegarderSalles();
    sauvegarderReservations();
}

int intervallesSeChevauchent(int start1, int end1, int start2, int end2) {
    return (start1 < end2) && (start2 < end1);
}

int reservationEnConflit(int index_salle, const char *date,
                         int heure_debut, int heure_fin) {
    int i;
    for (i = 0; i < nbReservations; i++) {
        if (reservations[i].index_salle == index_salle &&
            strcmp(reservations[i].date, date) == 0 &&
            reservations[i].statut != 2) {
            if (intervallesSeChevauchent(heure_debut, heure_fin,
                                         reservations[i].heure_debut,
                                         reservations[i].heure_fin)) {
                return 1;
            }
        }
    }
    return 0;
}

void ajouterSalle() {
    Salle s;

    if (nbSalles >= MAX_SALLES) {
        printf("Nombre maximal de salles atteint.\n");
        return;
    }

    printf("Nom de la salle : ");
    scanf(" %49s", s.nom);

    printf("Capacite : ");
    scanf("%d", &s.capacite);

    printf("Tarif horaire : ");
    scanf("%f", &s.tarif_horaire);

    printf("Equipements : ");
    scanf(" %199[^\n]", s.equipements);

    salles[nbSalles++] = s;
    printf("Salle ajoutee.\n");
}

void afficherSalles() {
    int i;
    printf("=== Liste des salles ===\n");
    if (!f)
    {
        printf("aucune salle à afficher");
    }        
    else
    {
        for (i = 0; i < nbSalles; i++) {
                printf("%d) %s, capacite=%d, tarif=%.2f, equipements=%s\n",
               i+1, salles[i].nom, salles[i].capacite,
               salles[i].tarif_horaire, salles[i].equipements);
        }
    }
    
}

void ajouterReservation() {
    if (nbReservations >= MAX_RESERVATIONS) {
        printf("Nombre maximal de reservations atteint.\n");
        return;
    }
    if (nbSalles == 0) {
        printf("Aucune salle definie.\n");
        return;
    }

    Reservation r;
    int duree;

    printf("=== Nouvelle Reservation ===\n");
    r.id = nbReservations + 1;

    printf("Nom du client : ");
    scanf(" %49s", r.nom_client);

    afficherSalles();
    printf("Indice de la salle (1 a %d) : ", nbSalles);
    scanf("%d", &r.index_salle);
    r.index_salle--;
    if (r.index_salle < 0 || r.index_salle >= nbSalles) {
        printf("Indice de salle invalide.\n");
        return;
    }

    printf("Date (JJ-MM-AAAA) : ");
    scanf(" %10s", r.date);

    printf("Heure de debut : ");
    scanf("%d", &r.heure_debut);

    printf("Heure de fin : ");
    scanf("%d", &r.heure_fin);

    if (r.heure_fin <= r.heure_debut) {
        printf("Heure de fin doit etre > heure de debut.\n");
        return;
    }

    printf("Nombre de personnes : ");
    scanf("%d", &r.nb_personnes);

    if (r.nb_personnes > salles[r.index_salle].capacite) {
        printf("Erreur: nombre de personnes > capacite (%d).\n",
               salles[r.index_salle].capacite);
        return;
    }

    if (reservationEnConflit(r.index_salle, r.date,
                             r.heure_debut, r.heure_fin)) {
        printf("Erreur: reservation qui se chevauche deja.\n");
        return;
    }

    duree = r.heure_fin - r.heure_debut;
    r.tarif = salles[r.index_salle].tarif_horaire * duree;
    r.statut = 1;

    reservations[nbReservations++] = r;
    printf("Reservation creee. Tarif = %.2f\n", r.tarif);

    sauvegarderDansFichiers();
}

int extraireMois(const char *date) {
    int mois = (date[3] - '0') * 10 + (date[4] - '0');
    return mois;
}

void afficherChiffreAffairesParSalle() {
    int i, j;
    printf("=== Chiffre d'affaires par salle ===\n");
    for (i = 0; i < nbSalles; i++) {
        float total = 0.0f;
        for (j = 0; j < nbReservations; j++) {
            if (reservations[j].index_salle == i &&
                reservations[j].statut != 2) {
                total += reservations[j].tarif;
            }
        }
        printf("Salle %s : %.2f\n", salles[i].nom, total);
    }
}

void afficherReservationsParMois() {
    int i;
    int compteur[13] = {0};

    for (i = 0; i < nbReservations; i++) {
        if (reservations[i].statut != 2) {
            int mois = extraireMois(reservations[i].date);
            if (mois >= 1 && mois <= 12) {
                compteur[mois]++;
            }
        }
    }

    printf("=== Nombre de reservations par mois ===\n");
    for (i = 1; i <= 12; i++) {
        printf("Mois %02d : %d reservations\n", i, compteur[i]);
    }
}

void afficherSallesLesPlusPopulaires() {
    int i;
    int counts[MAX_SALLES] = {0};
    int max = 0;

    for (i = 0; i < nbReservations; i++) {
        if (reservations[i].statut != 2) {
            int idx = reservations[i].index_salle;
            if (idx >= 0 && idx < nbSalles) {
                counts[idx]++;
                if (counts[idx] > max) {
                    max = counts[idx];
                }
            }
        }
    }

    printf("=== Salles les plus populaires ===\n");
    if (max == 0) {
        printf("Aucune reservation.\n");
        return;
    }

    for (i = 0; i < nbSalles; i++) {
        if (counts[i] == max) {
            printf("Salle %s avec %d reservations\n",
                   salles[i].nom, counts[i]);
        }
    }
}

void afficherReservations() {
    int i;
    printf("=== Liste des reservations ===\n");
    for (i = 0; i < nbReservations; i++) {
        printf("ID=%d, client=%s, salle=%d, date=%s, %dh-%dh, nb=%d, tarif=%.2f, statut=%d\n",
               reservations[i].id,
               reservations[i].nom_client,
               reservations[i].index_salle,
               reservations[i].date,
               reservations[i].heure_debut,
               reservations[i].heure_fin,
               reservations[i].nb_personnes,
               reservations[i].tarif,
               reservations[i].statut);
    }
}

void afficherMenu() {
    printf("\n=== MENU ===\n");
    printf("1. Ajouter une salle\n");
    printf("2. Afficher les salles\n");
    printf("3. Ajouter une reservation\n");
    printf("4. Afficher les reservations\n");
    printf("5. Chiffre d'affaires par salle\n");
    printf("6. Reservations par mois\n");
    printf("7. Salles les plus populaires\n");
    printf("0. Quitter\n");
    printf("Choix : ");
}

int main() {
    int choix;
    chargerDepuisFichiers();

    do {
        afficherMenu();
        if (scanf("%d", &choix) != 1) {
            printf("Entree invalide. Fin du programme.\n");
            break;
        }

        switch (choix) {
        case 1:
            ajouterSalle();
            sauvegarderDansFichiers();
            break;
        case 2:
            afficherSalles();
            break;
        case 3:
            ajouterReservation();
            break;
        case 4:
            afficherReservations();
            break;
        case 5:
            afficherChiffreAffairesParSalle();
            break;
        case 6:
            afficherReservationsParMois();
            break;
        case 7:
            afficherSallesLesPlusPopulaires();
            break;
        case 0:
            printf("Au revoir.\n");
            break;
        default:
            printf("Choix invalide.\n");
        }
    } while (choix != 0);

    return 0;
}
