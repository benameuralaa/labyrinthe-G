/*
 * ====================================================================
 * JEU DE LABYRINTHE - PROGRAMME PÉDAGOGIQUE EN C
 * ====================================================================
 * 
 * Objectifs pédagogiques :
 * - Utilisation des fichiers (fopen, fgets, fscanf, fclose)
 * - Manipulation des pointeurs
 * - Tableaux 2D dynamiques
 * - Listes chaînées simples (historique)
 * - Listes doublement chaînées (UNDO/REDO)
 * 
 * Contrôles :
 * - Z : haut
 * - S : bas
 * - Q : gauche
 * - D : droite
 * - U : UNDO (annuler le dernier mouvement)
 * - R : REDO (refaire le mouvement annulé)
 * - Q : quitter
 * 
 * ====================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ====================================================================
 * DÉFINITION DES STRUCTURES DE DONNÉES
 * ==================================================================== */

/*
 * Structure pour représenter une position dans le labyrinthe
 * Utilisation pédagogique : structure simple avec coordonnées (x, y)
 */
typedef struct {
    int x;  // Coordonnée X (colonne)
    int y;  // Coordonnée Y (ligne)
} Position;

/*
 * Structure pour un nœud de liste chaînée SIMPLE (historique)
 * Utilisation pédagogique : liste simple pour stocker l'historique
 * Chaque nœud contient une position et un pointeur vers le suivant
 */
typedef struct HistoriqueNoeud {
    Position pos;                        // Position stockée
    struct HistoriqueNoeud* suivant;     // Pointeur vers le nœud suivant
} HistoriqueNoeud;

/*
 * Structure pour une liste chaînée simple (historique)
 * Utilisation pédagogique : contient un pointeur vers la tête de la liste
 */
typedef struct {
    HistoriqueNoeud* tete;   // Pointeur vers le premier élément
    int taille;              // Nombre d'éléments dans l'historique
} Historique;

/*
 * Structure pour un nœud de liste DOUBLEMENT CHAÎNÉE (UNDO/REDO)
 * Utilisation pédagogique : liste doublement chaînée pour UNDO/REDO
 * Chaque nœud contient une position et deux pointeurs (précédent et suivant)
 */
typedef struct UndoRedoNoeud {
    Position pos;                          // Position stockée
    struct UndoRedoNoeud* precedent;       // Pointeur vers le nœud précédent
    struct UndoRedoNoeud* suivant;         // Pointeur vers le nœud suivant
} UndoRedoNoeud;

/*
 * Structure pour une liste doublement chaînée (UNDO/REDO)
 * Utilisation pédagogique : permet de naviguer dans les deux sens
 */
typedef struct {
    UndoRedoNoeud* actuel;   // Pointeur vers la position actuelle
    int capacite;            // Nombre total de positions sauvegardées
} UndoRedo;

/*
 * Structure principale du labyrinthe
 * Utilisation pédagogique : regroupe toutes les données nécessaires
 */
typedef struct {
    char** grille;           // Tableau 2D dynamique (pointeur de pointeurs)
    int largeur;             // Largeur du labyrinthe (colonnes)
    int hauteur;             // Hauteur du labyrinthe (lignes)
    Position joueur;         // Position actuelle du joueur
    Position sortie;         // Position de la sortie
    Historique historique;   // Historique des mouvements (liste simple)
    UndoRedo undoRedo;       // Système UNDO/REDO (liste doublement chaînée)
    int nbCoups;             // Compteur de coups
} Labyrinthe;

/* ====================================================================
 * DÉCLARATION DES FONCTIONS (PROTOTYPES)
 * ==================================================================== */

/* Fonctions pour l'historique (liste chaînée simple) */
HistoriqueNoeud* creer_noeud_historique(Position pos);
void ajouter_historique(Historique* hist, Position pos);
void liberer_historique(Historique* hist);

/* Fonctions pour UNDO/REDO (liste doublement chaînée) */
UndoRedoNoeud* creer_noeud_undo_redo(Position pos);
void ajouter_undo_redo(UndoRedo* ur, Position pos);
int undo(Labyrinthe* lab);
int redo(Labyrinthe* lab);
void liberer_undo_redo(UndoRedo* ur);

/* Fonctions pour le labyrinthe */
Labyrinthe* creer_labyrinthe(void);
void liberer_labyrinthe(Labyrinthe* lab);
int charger_labyrinthe(Labyrinthe* lab, const char* nomFichier);
void afficher_labyrinthe(const Labyrinthe* lab);
int deplacer_joueur(Labyrinthe* lab, char direction);
int est_position_valide(const Labyrinthe* lab, int x, int y);
void initialiser_joueur(Labyrinthe* lab);
int est_fini(const Labyrinthe* lab);

/* ====================================================================
 * FONCTIONS POUR L'HISTORIQUE (LISTE CHAÎNÉE SIMPLE)
 * ==================================================================== */

/*
 * Fonction : creer_noeud_historique
 * Description : Crée un nouveau nœud pour l'historique
 * Pédagogie : Alloue dynamiquement la mémoire pour un nœud
 */
HistoriqueNoeud* creer_noeud_historique(Position pos) {
    HistoriqueNoeud* nouveau = (HistoriqueNoeud*)malloc(sizeof(HistoriqueNoeud));
    if (nouveau == NULL) {
        printf("Erreur : impossible d'allouer la mémoire pour l'historique\n");
        return NULL;
    }
    nouveau->pos = pos;
    nouveau->suivant = NULL;  // Le nouveau nœud est le dernier
    return nouveau;
}

/*
 * Fonction : ajouter_historique
 * Description : Ajoute une position à l'historique (ajout en tête)
 * Pédagogie : Manipulation de pointeurs pour ajouter en tête de liste
 */
void ajouter_historique(Historique* hist, Position pos) {
    HistoriqueNoeud* nouveau = creer_noeud_historique(pos);
    if (nouveau == NULL) return;
    
    // Ajout en tête : le nouveau nœud pointe vers l'ancienne tête
    nouveau->suivant = hist->tete;
    hist->tete = nouveau;  // Le nouveau nœud devient la nouvelle tête
    hist->taille++;
}

/*
 * Fonction : liberer_historique
 * Description : Libère toute la mémoire de l'historique
 * Pédagogie : Parcours de liste et libération de chaque nœud
 */
void liberer_historique(Historique* hist) {
    HistoriqueNoeud* courant = hist->tete;
    HistoriqueNoeud* suivant;
    
    // Parcours de la liste et libération de chaque nœud
    while (courant != NULL) {
        suivant = courant->suivant;  // Sauvegarde du pointeur suivant
        free(courant);                // Libération du nœud courant
        courant = suivant;            // Passage au nœud suivant
    }
    
    hist->tete = NULL;
    hist->taille = 0;
}

/* ====================================================================
 * FONCTIONS POUR UNDO/REDO (LISTE DOUBLEMENT CHAÎNÉE)
 * ==================================================================== */

/*
 * Fonction : creer_noeud_undo_redo
 * Description : Crée un nouveau nœud pour UNDO/REDO
 * Pédagogie : Initialisation des deux pointeurs (précédent et suivant)
 */
UndoRedoNoeud* creer_noeud_undo_redo(Position pos) {
    UndoRedoNoeud* nouveau = (UndoRedoNoeud*)malloc(sizeof(UndoRedoNoeud));
    if (nouveau == NULL) {
        printf("Erreur : impossible d'allouer la mémoire pour UNDO/REDO\n");
        return NULL;
    }
    nouveau->pos = pos;
    nouveau->precedent = NULL;  // Initialisation des pointeurs
    nouveau->suivant = NULL;
    return nouveau;
}

/*
 * Fonction : ajouter_undo_redo
 * Description : Ajoute une position à la liste UNDO/REDO
 * Pédagogie : Gestion d'une liste doublement chaînée avec insertion
 */
void ajouter_undo_redo(UndoRedo* ur, Position pos) {
    UndoRedoNoeud* nouveau = creer_noeud_undo_redo(pos);
    if (nouveau == NULL) return;
    
    if (ur->actuel == NULL) {
        // Premier élément : la liste est vide
        ur->actuel = nouveau;
    } else {
        // Insertion après l'élément actuel
        nouveau->precedent = ur->actuel;
        nouveau->suivant = ur->actuel->suivant;
        
        if (ur->actuel->suivant != NULL) {
            ur->actuel->suivant->precedent = nouveau;
        }
        
        ur->actuel->suivant = nouveau;
        ur->actuel = nouveau;  // Le nouveau nœud devient l'actuel
        
        // Supprimer tous les nœuds après l'actuel (on ne peut pas REDO après un nouveau mouvement)
        UndoRedoNoeud* suivant = ur->actuel->suivant;
        while (suivant != NULL) {
            UndoRedoNoeud* a_supprimer = suivant;
            suivant = suivant->suivant;
            free(a_supprimer);
        }
        ur->actuel->suivant = NULL;
    }
    
    ur->capacite++;
}

/*
 * Fonction : undo
 * Description : Annule le dernier mouvement (retour en arrière)
 * Pédagogie : Navigation vers le nœud précédent dans une liste doublement chaînée
 */
int undo(Labyrinthe* lab) {
    UndoRedo* ur = &(lab->undoRedo);
    
    // Vérifier qu'on peut faire UNDO (il doit y avoir un nœud précédent)
    if (ur->actuel == NULL || ur->actuel->precedent == NULL) {
        return 0;  // Impossible de faire UNDO
    }
    
    // Aller au nœud précédent
    ur->actuel = ur->actuel->precedent;
    lab->joueur = ur->actuel->pos;  // Restaurer la position
    
    return 1;  // UNDO réussi
}

/*
 * Fonction : redo
 * Description : Refait le mouvement annulé (avancer)
 * Pédagogie : Navigation vers le nœud suivant dans une liste doublement chaînée
 */
int redo(Labyrinthe* lab) {
    UndoRedo* ur = &(lab->undoRedo);
    
    // Vérifier qu'on peut faire REDO (il doit y avoir un nœud suivant)
    if (ur->actuel == NULL || ur->actuel->suivant == NULL) {
        return 0;  // Impossible de faire REDO
    }
    
    // Aller au nœud suivant
    ur->actuel = ur->actuel->suivant;
    lab->joueur = ur->actuel->pos;  // Restaurer la position
    
    return 1;  // REDO réussi
}

/*
 * Fonction : liberer_undo_redo
 * Description : Libère toute la mémoire de la liste UNDO/REDO
 * Pédagogie : Parcours et libération d'une liste doublement chaînée
 */
void liberer_undo_redo(UndoRedo* ur) {
    if (ur->actuel == NULL) return;
    
    // Aller au début de la liste
    UndoRedoNoeud* debut = ur->actuel;
    while (debut->precedent != NULL) {
        debut = debut->precedent;
    }
    
    // Parcours et libération de tous les nœuds
    UndoRedoNoeud* courant = debut;
    UndoRedoNoeud* suivant;
    
    while (courant != NULL) {
        suivant = courant->suivant;
        free(courant);
        courant = suivant;
    }
    
    ur->actuel = NULL;
    ur->capacite = 0;
}

/* ====================================================================
 * FONCTIONS POUR LE LABYRINTHE
 * ==================================================================== */

/*
 * Fonction : creer_labyrinthe
 * Description : Crée et initialise un nouveau labyrinthe
 * Pédagogie : Allocation dynamique et initialisation de la structure
 */
Labyrinthe* creer_labyrinthe(void) {
    Labyrinthe* lab = (Labyrinthe*)malloc(sizeof(Labyrinthe));
    if (lab == NULL) {
        printf("Erreur : impossible d'allouer la mémoire pour le labyrinthe\n");
        return NULL;
    }
    
    // Initialisation des valeurs par défaut
    lab->grille = NULL;
    lab->largeur = 0;
    lab->hauteur = 0;
    lab->joueur.x = 0;
    lab->joueur.y = 0;
    lab->sortie.x = 0;
    lab->sortie.y = 0;
    lab->historique.tete = NULL;
    lab->historique.taille = 0;
    lab->undoRedo.actuel = NULL;
    lab->undoRedo.capacite = 0;
    lab->nbCoups = 0;
    
    return lab;
}

/*
 * Fonction : liberer_labyrinthe
 * Description : Libère toute la mémoire allouée pour le labyrinthe
 * Pédagogie : Libération d'un tableau 2D dynamique et des listes chaînées
 */
void liberer_labyrinthe(Labyrinthe* lab) {
    if (lab == NULL) return;
    
    // Libération du tableau 2D
    if (lab->grille != NULL) {
        for (int i = 0; i < lab->hauteur; i++) {
            free(lab->grille[i]);  // Libérer chaque ligne
        }
        free(lab->grille);  // Libérer le tableau de pointeurs
    }
    
    // Libération des listes chaînées
    liberer_historique(&(lab->historique));
    liberer_undo_redo(&(lab->undoRedo));
    
    // Libération de la structure principale
    free(lab);
}

/*
 * Fonction : charger_labyrinthe
 * Description : Charge un labyrinthe depuis un fichier texte
 * Pédagogie : Utilisation de fopen, fgets, fclose pour lire un fichier
 *             Allocation dynamique d'un tableau 2D
 */
int charger_labyrinthe(Labyrinthe* lab, const char* nomFichier) {
    FILE* fichier;
    char ligne[1024];  // Buffer pour lire chaque ligne
    int ligne_num = 0;
    
    // Ouverture du fichier en mode lecture
    fichier = fopen(nomFichier, "r");
    if (fichier == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier '%s'\n", nomFichier);
        return 0;
    }
    
    // Première passe : compter les lignes et trouver la largeur maximale
    int max_largeur = 0;
    int nb_lignes = 0;
    
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        int longueur = strlen(ligne);
        // Enlever le caractère de fin de ligne s'il existe
        if (longueur > 0 && ligne[longueur - 1] == '\n') {
            ligne[longueur - 1] = '\0';
            longueur--;
        }
        if (longueur > max_largeur) {
            max_largeur = longueur;
        }
        nb_lignes++;
    }
    
    // Réinitialiser le fichier au début
    rewind(fichier);
    
    // Allocation du tableau 2D dynamique
    // Allocation du tableau de pointeurs (lignes)
    lab->grille = (char**)malloc(nb_lignes * sizeof(char*));
    if (lab->grille == NULL) {
        printf("Erreur : impossible d'allouer la mémoire pour la grille\n");
        fclose(fichier);
        return 0;
    }
    
    // Allocation de chaque ligne
    for (int i = 0; i < nb_lignes; i++) {
        lab->grille[i] = (char*)malloc((max_largeur + 1) * sizeof(char));
        if (lab->grille[i] == NULL) {
            printf("Erreur : impossible d'allouer la mémoire pour la ligne %d\n", i);
            // Libérer ce qui a déjà été alloué
            for (int j = 0; j < i; j++) {
                free(lab->grille[j]);
            }
            free(lab->grille);
            fclose(fichier);
            return 0;
        }
    }
    
    // Deuxième passe : lire le contenu du fichier
    ligne_num = 0;
    while (fgets(ligne, sizeof(ligne), fichier) != NULL && ligne_num < nb_lignes) {
        int longueur = strlen(ligne);
        if (longueur > 0 && ligne[longueur - 1] == '\n') {
            ligne[longueur - 1] = '\0';
            longueur--;
        }
        
        // Copier la ligne dans la grille et remplir avec des espaces si nécessaire
        strcpy(lab->grille[ligne_num], ligne);
        for (int i = longueur; i < max_largeur; i++) {
            lab->grille[ligne_num][i] = ' ';  // Remplir avec des espaces
        }
        lab->grille[ligne_num][max_largeur] = '\0';  // Terminateur de chaîne
        
        ligne_num++;
    }
    
    // Fermeture du fichier
    fclose(fichier);
    
    // Enregistrement des dimensions
    lab->hauteur = nb_lignes;
    lab->largeur = max_largeur;
    
    // Initialiser la position du joueur
    initialiser_joueur(lab);
    
    return 1;  // Succès
}

/*
 * Fonction : initialiser_joueur
 * Description : Trouve la position initiale du joueur 'P' et de la sortie 'E' dans le labyrinthe
 * Pédagogie : Parcours d'un tableau 2D pour trouver des caractères
 */
void initialiser_joueur(Labyrinthe* lab) {
    int joueur_trouve = 0;
    int sortie_trouvee = 0;
    
    // Parcourir tout le labyrinthe pour trouver 'P' et 'E'
    for (int y = 0; y < lab->hauteur; y++) {
        for (int x = 0; x < lab->largeur; x++) {
            // Chercher la position du joueur 'P'
            if (!joueur_trouve && lab->grille[y][x] == 'P') {
                lab->joueur.x = x;
                lab->joueur.y = y;
                joueur_trouve = 1;
                // Remplacer 'P' par un espace dans la grille (le joueur sera affiché séparément)
                lab->grille[y][x] = ' ';
            }
            // Chercher la position de la sortie 'E'
            if (!sortie_trouvee && lab->grille[y][x] == 'E') {
                lab->sortie.x = x;
                lab->sortie.y = y;
                sortie_trouvee = 1;
            }
        }
    }
    
    // Vérifier que le joueur et la sortie ont été trouvés
    if (!joueur_trouve) {
        printf("Attention : Position du joueur 'P' non trouvee dans le labyrinthe !\n");
    }
    if (!sortie_trouvee) {
        printf("Attention : Sortie 'E' non trouvee dans le labyrinthe !\n");
    }
    
    // Ajouter la position initiale à UNDO/REDO
    if (joueur_trouve) {
        ajouter_undo_redo(&(lab->undoRedo), lab->joueur);
    }
}

/*
 * Fonction : afficher_labyrinthe
 * Description : Affiche le labyrinthe dans la console
 * Pédagogie : Parcours d'un tableau 2D et affichage
 */
void afficher_labyrinthe(const Labyrinthe* lab) {
    // Effacer l'écran (portable)
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
    
    printf("\n=== LABYRINTHE ===\n\n");
    
    // Parcours de chaque ligne
    for (int y = 0; y < lab->hauteur; y++) {
        // Parcours de chaque colonne
        for (int x = 0; x < lab->largeur; x++) {
            // Si c'est la position du joueur, afficher 'P'
            if (x == lab->joueur.x && y == lab->joueur.y) {
                // Si le joueur est sur la sortie, afficher 'P' (on a déjà vérifié la victoire)
                printf("P");
            } else {
                // Sinon, afficher le caractère de la grille
                printf("%c", lab->grille[y][x]);
            }
        }
        printf("\n");  // Nouvelle ligne après chaque ligne du labyrinthe
    }
    
    printf("\nCoups : %d\n", lab->nbCoups);
    printf("Position : (%d, %d)\n", lab->joueur.x, lab->joueur.y);
    printf("\nCommandes : Z(haut) S(bas) Q(gauche) D(droite) U(undo) R(redo) X(quitter)\n");
}

/*
 * Fonction : est_position_valide
 * Description : Vérifie si une position est valide (pas un mur, dans les limites)
 * Pédagogie : Vérification des limites d'un tableau et des valeurs
 */
int est_position_valide(const Labyrinthe* lab, int x, int y) {
    // Vérifier que la position est dans les limites du labyrinthe
    if (x < 0 || x >= lab->largeur || y < 0 || y >= lab->hauteur) {
        return 0;  // Hors limites
    }
    
    // Vérifier que ce n'est pas un mur
    // Les espaces ' ' et la sortie 'E' sont des positions valides
    if (lab->grille[y][x] == '#') {
        return 0;  // C'est un mur
    }
    
    return 1;  // Position valide (espace ou sortie 'E')
}

/*
 * Fonction : deplacer_joueur
 * Description : Déplace le joueur dans une direction donnée
 * Pédagogie : Manipulation de pointeurs, vérification de conditions
 */
int deplacer_joueur(Labyrinthe* lab, char direction) {
    int nouvelle_x = lab->joueur.x;
    int nouvelle_y = lab->joueur.y;
    
    // Calculer la nouvelle position selon la direction
    switch (direction) {
        case 'Z': case 'z':  // Haut
            nouvelle_y--;
            break;
        case 'S': case 's':  // Bas
            nouvelle_y++;
            break;
        case 'Q': case 'q':  // Gauche
            nouvelle_x--;
            break;
        case 'D': case 'd':  // Droite
            nouvelle_x++;
            break;
        default:
            return 0;  // Direction invalide
    }
    
    // Vérifier que la nouvelle position est valide
    if (!est_position_valide(lab, nouvelle_x, nouvelle_y)) {
        return 0;  // Mouvement impossible
    }
    
    // Sauvegarder l'ancienne position pour l'historique
    Position ancienne_pos = lab->joueur;
    
    // Déplacer le joueur
    lab->joueur.x = nouvelle_x;
    lab->joueur.y = nouvelle_y;
    
    // Ajouter à l'historique (liste simple)
    ajouter_historique(&(lab->historique), ancienne_pos);
    
    // Ajouter à UNDO/REDO (liste doublement chaînée)
    ajouter_undo_redo(&(lab->undoRedo), lab->joueur);
    
    // Incrémenter le compteur de coups
    lab->nbCoups++;
    
    return 1;  // Déplacement réussi
}

/*
 * Fonction : est_fini
 * Description : Vérifie si le joueur a atteint la sortie
 * Pédagogie : Comparaison simple de positions
 */
int est_fini(const Labyrinthe* lab) {
    return (lab->joueur.x == lab->sortie.x && lab->joueur.y == lab->sortie.y);
}

/* ====================================================================
 * FONCTION PRINCIPALE (MAIN)
 * ==================================================================== */

int main(int argc, char* argv[]) {
    Labyrinthe* lab;
    char commande;
    const char* nomFichier;
    
    // Vérifier si un nom de fichier est fourni en argument
    if (argc > 1) {
        nomFichier = argv[1];
    } else {
        // Sinon, utiliser le fichier par défaut
        nomFichier = "labyrinthe.txt";
    }
    
    // Créer le labyrinthe
    lab = creer_labyrinthe();
    if (lab == NULL) {
        return 1;
    }
    
    // Charger le labyrinthe depuis le fichier
    printf("Chargement du labyrinthe depuis '%s'...\n", nomFichier);
    if (!charger_labyrinthe(lab, nomFichier)) {
        printf("Erreur lors du chargement du labyrinthe.\n");
        liberer_labyrinthe(lab);
        return 1;
    }
    
    printf("Labyrinthe charge avec succes !\n");
    printf("Appuyez sur une touche pour commencer...\n");
    getchar();
    
    // Boucle principale du jeu
    do {
        afficher_labyrinthe(lab);
        
        // Vérifier si le joueur a gagné
        if (est_fini(lab)) {
            printf("\n=== FELICITATIONS ! VOUS AVEZ TROUVE LA SORTIE ! ===\n");
            printf("Nombre de coups : %d\n", lab->nbCoups);
            break;
        }
        
        // Lire la commande du joueur
        printf("\nCommande : ");
        commande = getchar();
        // Consommer le caractère de nouvelle ligne restant
        while (getchar() != '\n');
        
        // Traiter la commande
        switch (commande) {
            case 'Z': case 'z':
            case 'S': case 's':
            case 'Q': case 'q':
            case 'D': case 'd':
                if (!deplacer_joueur(lab, commande)) {
                    printf("Mouvement impossible !\n");
                    getchar();  // Attendre que l'utilisateur appuie sur Entrée
                }
                break;
            case 'U': case 'u':
                if (undo(lab)) {
                    lab->nbCoups++;
                    printf("UNDO effectue !\n");
                } else {
                    printf("Impossible de faire UNDO !\n");
                }
                getchar();
                break;
            case 'R': case 'r':
                if (redo(lab)) {
                    lab->nbCoups++;
                    printf("REDO effectue !\n");
                } else {
                    printf("Impossible de faire REDO !\n");
                }
                getchar();
                break;
            case 'X': case 'x':
                printf("Au revoir !\n");
                break;
            default:
                printf("Commande invalide !\n");
                getchar();
                break;
        }
        
    } while (commande != 'X' && commande != 'x' && !est_fini(lab));
    
    // Libération de la mémoire
    liberer_labyrinthe(lab);
    
    return 0;
}


