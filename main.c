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
    char date[11];
    int heure_debut;
    int heure_fin;
    int nb_personnes;
    float tarif;
    int statut;
} Reservation;

Salle salles[MAX_SALLES];
int nbSalles = 0;

Reservation reservations[MAX_RESERVATIONS];
int nbReservations = 0;

int lireEntier() {
    int valeur;
    int resultat;
    while (1) {
        resultat = scanf("%d", &valeur);
        if (resultat != 1) {
            printf("Erreur : veuillez entrer un nombre entier valide.\n");
            while (getchar() != '\n');
            continue;
        }
        return valeur;
    }
}

float lireReel() {
    float valeur;
    int resultat;
    while (1) {
        resultat = scanf("%f", &valeur);
        if (resultat != 1) {
            printf("Erreur : veuillez entrer un nombre reel valide.\n");
            while (getchar() != '\n');
            continue;
        }
        return valeur;
    }
}

void sauvegarderSalles() {
    FILE *f = fopen("assets/salles.txt", "w");
    int i;
    if (!f) {
        printf("Erreur : impossible d'ouvrir assets/salles.txt en ecriture.\n");
        return;
    }
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
    FILE *f = fopen("assets/salles.txt", "r");
    if (!f) {
        nbSalles = 0;
        return;
    }
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

void sauvegarderTarifs() {
    FILE *f = fopen("assets/tarifs.txt", "w");
    if (!f) {
        printf("Erreur ouverture fichier tarifs.\n");
        return;
    }
    for (int i = 0; i < nbSalles; i++) {
        fprintf(f, "%s;%.2f\n", salles[i].nom, salles[i].tarif_horaire);
    }
    fclose(f);
}

void chargerTarifs() {
    FILE *f = fopen("assets/tarifs.txt", "r");
    if (!f) return;
    char nom[50];
    float tarif;
    while (fscanf(f, "%49[^;];%f\n", nom, &tarif) == 2) {
        for (int i = 0; i < nbSalles; i++) {
            if (strcmp(salles[i].nom, nom) == 0) {
                salles[i].tarif_horaire = tarif;
            }
        }
    }
    fclose(f);
}

void sauvegarderReservations() {
    FILE *f = fopen("assets/reservations.txt", "w");
    int i;
    if (!f) {
        printf("Erreur : impossible d'ouvrir assets/reservations.txt en ecriture.\n");
        return;
    }
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
    FILE *f = fopen("assets/reservations.txt", "r");
    if (!f) {
        nbReservations = 0;
        return;
    }
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
    chargerTarifs();
}

void sauvegarderDansFichiers() {
    sauvegarderSalles();
    sauvegarderReservations();
    sauvegarderTarifs();
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

// SECTION SALLES

void afficherSalles() {
    int i;
    printf("=== Liste des salles ===\n");
    if (nbSalles == 0) {
        printf("Aucune salle a afficher.\n");
        return;
    }
    for (i = 0; i < nbSalles; i++) {
        printf("%d) %s, capacite=%d, tarif=%.2f, equipements=%s\n",
               i + 1, salles[i].nom, salles[i].capacite,
               salles[i].tarif_horaire, salles[i].equipements);
    }
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
    s.capacite = lireEntier();
    printf("Tarif horaire : ");
    s.tarif_horaire = lireReel();
    printf("Equipements : ");
    scanf(" %199[^\n]", s.equipements);
    salles[nbSalles++] = s;
    printf("Salle ajoutee.\n");
    sauvegarderDansFichiers();
}

void modifierSalle() {
    if (nbSalles == 0) {
        printf("Aucune salle a modifier.\n");
        return;
    }
    afficherSalles();
    printf("Indice de la salle a modifier (1 a %d) : ", nbSalles);
    int index = lireEntier();
    index--;
    if (index < 0 || index >= nbSalles) {
        printf("Indice invalide.\n");
        return;
    }
    printf("\n=== Modification de la salle %s ===\n", salles[index].nom);
    printf("1. Modifier le nom\n");
    printf("2. Modifier la capacite\n");
    printf("3. Modifier le tarif horaire\n");
    printf("4. Modifier les equipements\n");
    printf("0. Annuler\n");
    printf("Choix : ");
    int choix = lireEntier();
    switch (choix) {
    case 1:
        printf("Nouveau nom : ");
        scanf(" %49s", salles[index].nom);
        printf("Nom modifie.\n");
        break;
    case 2: {
        printf("Nouvelle capacite : ");
        int nouvelle_capacite = lireEntier();
        for (int i = 0; i < nbReservations; i++) {
            if (reservations[i].index_salle == index &&
                reservations[i].statut != 2 &&
                reservations[i].nb_personnes > nouvelle_capacite) {
                printf("Erreur : la nouvelle capacite est inferieure au nombre de personnes dans une reservation (%d personnes).\n",
                       reservations[i].nb_personnes);
                return;
            }
        }
        salles[index].capacite = nouvelle_capacite;
        printf("Capacite modifiee.\n");
        break;
    }
    case 3:
        printf("Nouveau tarif horaire : ");
        salles[index].tarif_horaire = lireReel();
        for (int i = 0; i < nbReservations; i++) {
            if (reservations[i].index_salle == index &&
                reservations[i].statut != 2) {
                int duree = reservations[i].heure_fin - reservations[i].heure_debut;
                reservations[i].tarif = salles[index].tarif_horaire * duree;
            }
        }
        printf("Tarif horaire modifie et tarifs des reservations recalcules.\n");
        break;
    case 4:
        printf("Nouveaux equipements : ");
        scanf(" %199[^\n]", salles[index].equipements);
        printf("Equipements modifies.\n");
        break;
    case 0:
        printf("Modification annulee.\n");
        return;
    default:
        printf("Choix invalide.\n");
        return;
    }
    sauvegarderDansFichiers();
}

void supprimerSalle() {
    if (nbSalles == 0) {
        printf("Aucune salle a supprimer.\n");
        return;
    }
    afficherSalles();
    printf("Indice de la salle a supprimer (1 a %d) : ", nbSalles);
    int index = lireEntier();
    index--;
    if (index < 0 || index >= nbSalles) {
        printf("Indice invalide.\n");
        return;
    }
    for (int i = 0; i < nbReservations; i++) {
        if (reservations[i].index_salle == index) {
            reservations[i].statut = 2;
        }
    }
    for (int i = index; i < nbSalles - 1; i++) {
        salles[i] = salles[i + 1];
    }
    nbSalles--;
    for (int i = 0; i < nbReservations; i++) {
        if (reservations[i].index_salle > index) {
            reservations[i].index_salle--;
        }
    }
    printf("Salle supprimee.\n");
    sauvegarderDansFichiers();
}

// SECTION RESERVATIONS ET FACTURES

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
    r.index_salle = lireEntier();
    r.index_salle--;
    if (r.index_salle < 0 || r.index_salle >= nbSalles) {
        printf("Indice de salle invalide.\n");
        return;
    }
    printf("Date (JJ-MM-AAAA) : ");
    scanf(" %10s", r.date);
    printf("Heure de debut : ");
    r.heure_debut = lireEntier();
    printf("Heure de fin : ");
    r.heure_fin = lireEntier();
    if (r.heure_fin <= r.heure_debut) {
        printf("Heure de fin doit etre > heure de debut.\n");
        return;
    }
    printf("Nombre de personnes : ");
    r.nb_personnes = lireEntier();
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

void modifierReservation() {
    if (nbReservations == 0) {
        printf("Aucune reservation a modifier.\n");
        return;
    }
    afficherReservations();
    printf("ID de la reservation a modifier : ");
    int id = lireEntier();
    int index = -1;
    for (int i = 0; i < nbReservations; i++) {
        if (reservations[i].id == id) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        printf("Reservation non trouvee.\n");
        return;
    }
    if (reservations[index].statut == 2) {
        printf("Erreur : impossible de modifier une reservation annulee.\n");
        return;
    }
    printf("\n=== Modification de la reservation %d ===\n", id);
    printf("1. Modifier le nom du client\n");
    printf("2. Modifier la date\n");
    printf("3. Modifier les heures\n");
    printf("4. Modifier le nombre de personnes\n");
    printf("5. Modifier la salle\n");
    printf("0. Annuler\n");
    printf("Choix : ");
    int choix = lireEntier();
    switch (choix) {
    case 1:
        printf("Nouveau nom du client : ");
        scanf(" %49s", reservations[index].nom_client);
        printf("Nom modifie.\n");
        break;
    case 2:
        printf("Nouvelle date (JJ-MM-AAAA) : ");
        scanf(" %10s", reservations[index].date);
        if (reservationEnConflit(reservations[index].index_salle,
                                 reservations[index].date,
                                 reservations[index].heure_debut,
                                 reservations[index].heure_fin)) {
            printf("Erreur : une reservation existe deja pour cette date et ces heures.\n");
            return;
        }
        printf("Date modifiee.\n");
        break;
    case 3: {
        printf("Nouvelle heure de debut (0-23) : ");
        int heure_debut = lireEntier();
        printf("Nouvelle heure de fin (0-24) : ");
        int heure_fin = lireEntier();
        if (heure_fin <= heure_debut) {
            printf("Erreur : heure de fin doit etre > heure de debut.\n");
            return;
        }
        if (reservationEnConflit(reservations[index].index_salle,
                                 reservations[index].date,
                                 heure_debut, heure_fin)) {
            printf("Erreur : une reservation existe deja pour ces heures.\n");
            return;
        }
        reservations[index].heure_debut = heure_debut;
        reservations[index].heure_fin = heure_fin;
        int duree = heure_fin - heure_debut;
        reservations[index].tarif = salles[reservations[index].index_salle].tarif_horaire * duree;
        printf("Heures modifiees et tarif recalcule.\n");
        break;
    }
    case 4: {
        printf("Nouveau nombre de personnes : ");
        int nb_personnes = lireEntier();
        if (nb_personnes > salles[reservations[index].index_salle].capacite) {
            printf("Erreur : nombre de personnes > capacite (%d).\n",
                   salles[reservations[index].index_salle].capacite);
            return;
        }
        reservations[index].nb_personnes = nb_personnes;
        printf("Nombre de personnes modifie.\n");
        break;
    }
    case 5: {
        afficherSalles();
        printf("Nouvelle salle (1 a %d) : ", nbSalles);
        int nouvelle_salle = lireEntier();
        nouvelle_salle--;
        if (nouvelle_salle < 0 || nouvelle_salle >= nbSalles) {
            printf("Indice de salle invalide.\n");
            return;
        }
        if (reservationEnConflit(nouvelle_salle,
                                 reservations[index].date,
                                 reservations[index].heure_debut,
                                 reservations[index].heure_fin)) {
            printf("Erreur : une reservation existe deja pour cette salle a cette date et heure.\n");
            return;
        }
        if (reservations[index].nb_personnes > salles[nouvelle_salle].capacite) {
            printf("Erreur : nombre de personnes > capacite (%d).\n",
                   salles[nouvelle_salle].capacite);
            return;
        }
        reservations[index].index_salle = nouvelle_salle;
        int duree = reservations[index].heure_fin - reservations[index].heure_debut;
        reservations[index].tarif = salles[nouvelle_salle].tarif_horaire * duree;
        printf("Salle modifiee et tarif recalcule.\n");
        break;
    }
    case 0:
        printf("Modification annulee.\n");
        return;
    default:
        printf("Choix invalide.\n");
        return;
    }
    sauvegarderDansFichiers();
}

void supprimerReservation() {
    if (nbReservations == 0) {
        printf("Aucune reservation a supprimer.\n");
        return;
    }
    afficherReservations();
    printf("ID de la reservation a supprimer : ");
    int id = lireEntier();
    int index = -1;
    for (int i = 0; i < nbReservations; i++) {
        if (reservations[i].id == id) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        printf("Reservation non trouvee.\n");
        return;
    }
    for (int i = index; i < nbReservations - 1; i++) {
        reservations[i] = reservations[i + 1];
    }
    nbReservations--;
    printf("Reservation supprimee.\n");
    sauvegarderDansFichiers();
}

void genererFacture(int idReservation) {
    Reservation *r = NULL;
    for (int i = 0; i < nbReservations; i++) {
        if (reservations[i].id == idReservation) {
            r = &reservations[i];
            break;
        }
    }
    if (!r) {
        printf("Reservation introuvable.\n");
        return;
    }
    char nomFichier[150];
    sprintf(nomFichier, "factures/facture_%d.txt", r->id);
    FILE *f = fopen(nomFichier, "w");
    if (!f) {
        printf("Erreur creation facture.\n");
        return;
    }
    int duree = r->heure_fin - r->heure_debut;
    fprintf(f,
            "====== FACTURE ======\n"
            "Client : %s\n"
            "Salle  : %s\n"
            "Date   : %s\n"
            "Heure  : %dh - %dh\n"
            "Duree  : %d heures\n"
            "Montant : %.2f DT\n",
            r->nom_client,
            salles[r->index_salle].nom,
            r->date,
            r->heure_debut,
            r->heure_fin,
            duree,
            r->tarif);
    printf(
            "====== FACTURE ======\n"
            "Client : %s\n"
            "Salle  : %s\n"
            "Date   : %s\n"
            "Heure  : %dh - %dh\n"
            "Duree  : %d heures\n"
            "Montant : %.2f DT\n",
            r->nom_client,
            salles[r->index_salle].nom,
            r->date,
            r->heure_debut,
            r->heure_fin,
            duree,
            r->tarif);
    fclose(f);
}

// SECTION STATISTIQUES

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

// MENU

void afficherMenu() {
    printf("\n========== MENU PRINCIPAL ==========\n");
    printf("\n--- RESERVATIONS ET FACTURES ---\n");
    printf("1. Ajouter une reservation\n");
    printf("2. Afficher les reservations\n");
    printf("3. Modifier une reservation\n");
    printf("4. Supprimer une reservation\n");
    printf("5. Generer une facture\n");
    printf("\n--- GESTION DES SALLES ---\n");
    printf("6. Ajouter une salle\n");
    printf("7. Afficher les salles\n");
    printf("8. Modifier une salle\n");
    printf("9. Supprimer une salle\n");
    printf("\n--- STATISTIQUES ---\n");
    printf("10. Chiffre d'affaires par salle\n");
    printf("11. Reservations par mois\n");
    printf("12. Salles les plus populaires\n");
    printf("\n0. Quitter\n");
    printf("=====================================\n");
    printf("Choix : ");
}

// PROGRAMME PRINCIPAL
int main() {
    int choix;
    chargerDepuisFichiers();

    do {
        afficherMenu();
        choix = lireEntier();

        switch (choix) {
        case 1:
            ajouterReservation();
            break;
        case 2:
            afficherReservations();
            break;
        case 3:
            modifierReservation();
            break;
        case 4:
            supprimerReservation();
            break;
        case 5:
            afficherReservations();
            printf("ID de la reservation pour generer la facture : ");
            int id = lireEntier();
            genererFacture(id);
            break;
        case 6:
            ajouterSalle();
            break;
        case 7:
            afficherSalles();
            break;
        case 8:
            modifierSalle();
            break;
        case 9:
            supprimerSalle();
            break;
        case 10:
            afficherChiffreAffairesParSalle();
            break;
        case 11:
            afficherReservationsParMois();
            break;
        case 12:
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

