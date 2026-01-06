#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ================= STRUCTURES ================= */

typedef struct {
    int x;
    int y;
} Position;

typedef struct HistoriqueNoeud {
    Position pos;
    struct HistoriqueNoeud* suivant;
} HistoriqueNoeud;

typedef struct {
    HistoriqueNoeud* tete;
    int taille; // Nb positions
} Historique;


typedef struct UndoRedoNoeud {
    Position pos;
    struct UndoRedoNoeud* precedent;
    struct UndoRedoNoeud* suivant;
} UndoRedoNoeud;

typedef struct {
    UndoRedoNoeud* actuel;
    int capacite;  // Nb coups stockés
} UndoRedo;

/* Structure principale du jeu */
typedef struct {
    char** grille;
    int largeur;
    int hauteur;
    Position joueur;
    Position sortie;
    Historique historique;
    UndoRedo undoRedo;
    int nbCoups;
} Labyrinthe;

/* ================= HISTORIQUE ================= */

HistoriqueNoeud* creer_noeud_historique(Position pos) {
    HistoriqueNoeud* nouveau = malloc(sizeof(HistoriqueNoeud));
    if (!nouveau) return NULL;
    nouveau->pos = pos;
    nouveau->suivant = NULL;
    return nouveau;
}

/* Ajoute une position à l'historique */
void ajouter_historique(Historique* hist, Position pos) {
    HistoriqueNoeud* nouveau = creer_noeud_historique(pos);
    if (!nouveau) return;
    nouveau->suivant = hist->tete;
    hist->tete = nouveau;
    hist->taille++;
}

/* Libère l'historique */
void liberer_historique(Historique* hist) {
    HistoriqueNoeud* courant = hist->tete;
    while (courant) {
        HistoriqueNoeud* tmp = courant;
        courant = courant->suivant;
        free(tmp);
    }
    hist->tete = NULL;
    hist->taille = 0;
}

/* ================= UNDO / REDO ================= */

UndoRedoNoeud* creer_noeud_undo_redo(Position pos) {
    UndoRedoNoeud* nouveau = malloc(sizeof(UndoRedoNoeud));
    if (!nouveau) return NULL;
    nouveau->pos = pos;
    nouveau->precedent = NULL;
    nouveau->suivant = NULL;
    return nouveau;
}

/* Ajoute une position Undo / Redo */
void ajouter_undo_redo(UndoRedo* ur, Position pos) {
    UndoRedoNoeud* nouveau = creer_noeud_undo_redo(pos);
    if (!nouveau) return;

    if (!ur->actuel) {
        ur->actuel = nouveau;
    } else {
        nouveau->precedent = ur->actuel;
        ur->actuel->suivant = nouveau;
        ur->actuel = nouveau;

        // Supprimer les coups suivants
        UndoRedoNoeud* s = nouveau->suivant;
        while (s) {
            UndoRedoNoeud* tmp = s;
            s = s->suivant;
            free(tmp);
        }
        nouveau->suivant = NULL;
    }
    ur->capacite++;
}

/* Annule le dernier coup */
int undo(Labyrinthe* lab) {
    UndoRedo* ur = &lab->undoRedo;
    if (!ur->actuel || !ur->actuel->precedent) return 0;
    ur->actuel = ur->actuel->precedent;
    lab->joueur = ur->actuel->pos;
    return 1;
}

/* Refait un coup annulé */
int redo(Labyrinthe* lab) {
    UndoRedo* ur = &lab->undoRedo;
    if (!ur->actuel || !ur->actuel->suivant) return 0;
    ur->actuel = ur->actuel->suivant;
    lab->joueur = ur->actuel->pos;
    return 1;
}

/* Libère Undo / Redo */
void liberer_undo_redo(UndoRedo* ur) {
    if (!ur->actuel) return;

    while (ur->actuel->precedent)
        ur->actuel = ur->actuel->precedent;

    while (ur->actuel) {
        UndoRedoNoeud* tmp = ur->actuel;
        ur->actuel = ur->actuel->suivant;
        free(tmp);
    }
    ur->capacite = 0;
}

/* ================= LABYRINTHE ================= */

/* Crée le labyrinthe */
Labyrinthe* creer_labyrinthe(void) {
    Labyrinthe* lab = malloc(sizeof(Labyrinthe));
    if (!lab) return NULL;

    lab->grille = NULL;
    lab->largeur = 0;
    lab->hauteur = 0;
    lab->historique.tete = NULL;
    lab->historique.taille = 0;
    lab->undoRedo.actuel = NULL;
    lab->undoRedo.capacite = 0;
    lab->nbCoups = 0;

    return lab;
}

/* Libère le labyrinthe */
void liberer_labyrinthe(Labyrinthe* lab) {
    if (!lab) return;

    for (int i = 0; i < lab->hauteur; i++)
        free(lab->grille[i]);
    free(lab->grille);

    liberer_historique(&lab->historique);
    liberer_undo_redo(&lab->undoRedo);
    free(lab);
}

/* Charge le labyrinthe depuis un fichier */
int charger_labyrinthe(Labyrinthe* lab, const char* nomFichier) {
    FILE* fichier = fopen(nomFichier, "r");
    if (!fichier) return 0;

    char ligne[1024];
    int largeur = 0, hauteur = 0;

    while (fgets(ligne, sizeof(ligne), fichier)) {
        int len = strlen(ligne);
        if (len && ligne[len - 1] == '\n') len--;
        if (len > largeur) largeur = len;
        hauteur++;
    }
    rewind(fichier);

    lab->grille = malloc(hauteur * sizeof(char*));
    for (int i = 0; i < hauteur; i++)
        lab->grille[i] = malloc(largeur + 1);

    for (int y = 0; y < hauteur; y++) {
        fgets(ligne, sizeof(ligne), fichier);
        int len = strlen(ligne);
        if (len && ligne[len - 1] == '\n') len--;
        strcpy(lab->grille[y], ligne);
        for (int i = len; i < largeur; i++)
            lab->grille[y][i] = ' ';
        lab->grille[y][largeur] = '\0';
    }

    fclose(fichier);
    lab->largeur = largeur;
    lab->hauteur = hauteur;
    initialiser_joueur(lab);
    return 1;
}

/* Trouve le joueur et la sortie */
void initialiser_joueur(Labyrinthe* lab) {
    for (int y = 0; y < lab->hauteur; y++) {
        for (int x = 0; x < lab->largeur; x++) {
            if (lab->grille[y][x] == 'P') {
                lab->joueur.x = x;
                lab->joueur.y = y;
                lab->grille[y][x] = ' ';
            }
            if (lab->grille[y][x] == 'E') {
                lab->sortie.x = x;
                lab->sortie.y = y;
            }
        }
    }
    ajouter_undo_redo(&lab->undoRedo, lab->joueur);
}

/* Affiche le labyrinthe */
void afficher_labyrinthe(const Labyrinthe* lab) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    for (int y = 0; y < lab->hauteur; y++) {
        for (int x = 0; x < lab->largeur; x++) {
            if (x == lab->joueur.x && y == lab->joueur.y)
                printf("P");
            else
                printf("%c", lab->grille[y][x]);
        }
        printf("\n");
    }
    printf("\nCoups : %d\n", lab->nbCoups);
}

/* Vérifie si une position est valide */
int est_position_valide(const Labyrinthe* lab, int x, int y) {
    if (x < 0 || y < 0 || x >= lab->largeur || y >= lab->hauteur)
        return 0;
    return lab->grille[y][x] != '#';
}

/* Déplace le joueur */
int deplacer_joueur(Labyrinthe* lab, char d) {
    int x = lab->joueur.x;
    int y = lab->joueur.y;

    if (d == 'z' || d == 'Z') y--;
    if (d == 's' || d == 'S') y++;
    if (d == 'q' || d == 'Q') x--;
    if (d == 'd' || d == 'D') x++;

    if (!est_position_valide(lab, x, y)) return 0;

    Position ancienne = lab->joueur;
    lab->joueur.x = x;
    lab->joueur.y = y;

    ajouter_historique(&lab->historique, ancienne);
    ajouter_undo_redo(&lab->undoRedo, lab->joueur);
    lab->nbCoups++;

    return 1;
}

/* Vérifie si le joueur a gagné */
int est_fini(const Labyrinthe* lab) {
    return lab->joueur.x == lab->sortie.x &&
           lab->joueur.y == lab->sortie.y;
}

/* ================= MAIN ================= */

int main(int argc, char* argv[]) {
    const char* fichier = (argc > 1) ? argv[1] : "labyrinthe.txt";
    Labyrinthe* lab = creer_labyrinthe();
    if (!lab || !charger_labyrinthe(lab, fichier)) return 1;

    char c;
    do {
        afficher_labyrinthe(lab);
        c = getchar();
        while (getchar() != '\n');

        if (c == 'u' || c == 'U') undo(lab);
        else if (c == 'r' || c == 'R') redo(lab);
        else if (c == 'x' || c == 'X') break;
        else deplacer_joueur(lab, c);

    } while (!est_fini(lab));

    liberer_labyrinthe(lab);
    return 0;
}

