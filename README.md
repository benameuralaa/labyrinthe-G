# 🎮 Jeu de Labyrinthe en C - Projet Pédagogique

Ce projet est un jeu de labyrinthe en console développé en langage C, spécialement conçu pour apprendre les concepts fondamentaux de la programmation.

## 📚 Objectifs Pédagogiques

Ce projet permet d'apprendre et de pratiquer :

1. **Gestion des fichiers** : `fopen`, `fgets`, `fscanf`, `fclose`
2. **Pointeurs** : manipulation et utilisation des pointeurs
3. **Tableaux 2D dynamiques** : allocation et libération de mémoire
4. **Listes chaînées simples** : structure et manipulation (historique)
5. **Listes doublement chaînées** : navigation bidirectionnelle (UNDO/REDO)

## 🎯 Règles du Jeu

- Le joueur (représenté par `P`) doit atteindre la sortie (représentée par `E`)
- Les murs sont représentés par `#`
- Les chemins sont des espaces ` `
- Le joueur ne peut pas traverser les murs

## 🎮 Commandes

- **Z** : Déplacer vers le haut
- **S** : Déplacer vers le bas
- **Q** : Déplacer vers la gauche
- **D** : Déplacer vers la droite
- **U** : UNDO (annuler le dernier mouvement)
- **R** : REDO (refaire le mouvement annulé)
- **X** : Quitter le jeu

## 📁 Structure du Code

### Structures de Données

1. **`Position`** : Structure simple pour stocker les coordonnées (x, y)

2. **`HistoriqueNoeud`** : Nœud de liste chaînée simple pour l'historique
   - Contient une position
   - Pointeur vers le nœud suivant

3. **`Historique`** : Liste chaînée simple pour l'historique complet
   - Pointeur vers la tête
   - Taille de la liste

4. **`UndoRedoNoeud`** : Nœud de liste doublement chaînée pour UNDO/REDO
   - Contient une position
   - Pointeur vers le nœud précédent
   - Pointeur vers le nœud suivant

5. **`UndoRedo`** : Liste doublement chaînée pour UNDO/REDO
   - Pointeur vers l'élément actuel
   - Capacité de la liste

6. **`Labyrinthe`** : Structure principale contenant toutes les données
   - Tableau 2D dynamique (grille)
   - Position du joueur et de la sortie
   - Historique et système UNDO/REDO
   - Compteur de coups

## 🔧 Compilation et Exécution

### Avec Makefile (recommandé)

```bash
# Compiler
make

# Exécuter
./labyrinthe labyrinthe.txt

# Nettoyer
make clean
```

### Compilation manuelle

```bash
gcc -Wall -Wextra -std=c99 -o labyrinthe labyrinthe.c
./labyrinthe labyrinthe.txt
```

### Windows

```bash
# Avec Makefile
make windows

# Ou manuellement
gcc -Wall -Wextra -std=c99 -o labyrinthe.exe labyrinthe.c
labyrinthe.exe labyrinthe.txt
```

## 📝 Format du Fichier Labyrinthe

Le labyrinthe est chargé depuis un fichier texte avec les caractères suivants :
- `#` : Mur
- ` ` (espace) : Chemin libre
- `P` : Position de départ du joueur
- `E` : Sortie

Exemple :
```
########
#      #
#P  #  E#
#   #   #
########
```

## 🧠 Explications Pédagogiques

### 1. Gestion des Fichiers

Le programme utilise :
- `fopen()` : Ouvre un fichier en mode lecture
- `fgets()` : Lit une ligne du fichier
- `fclose()` : Ferme le fichier

**Fonction clé** : `charger_labyrinthe()`

### 2. Pointeurs

Les pointeurs sont utilisés pour :
- Manipuler les tableaux 2D dynamiques (`char**`)
- Parcourir les listes chaînées
- Passer des structures par référence (pour éviter la copie)

### 3. Tableaux 2D Dynamiques

Allocation en deux étapes :
1. Allocation du tableau de pointeurs : `char** grille = malloc(hauteur * sizeof(char*))`
2. Allocation de chaque ligne : `grille[i] = malloc(largeur * sizeof(char))`

Libération inverse :
1. Libérer chaque ligne
2. Libérer le tableau de pointeurs

### 4. Liste Chaînée Simple (Historique)

- Chaque nœud pointe vers le suivant
- Ajout en tête : le nouveau nœud devient la tête
- Parcours : suivre les pointeurs `suivant`

**Fonction clé** : `ajouter_historique()`

### 5. Liste Doublement Chaînée (UNDO/REDO)

- Chaque nœud pointe vers le précédent ET le suivant
- Navigation dans les deux sens
- UNDO : aller au nœud précédent
- REDO : aller au nœud suivant

**Fonctions clés** : `undo()` et `redo()`

## 🎓 Concepts Clés Expliqués

### Allocation Dynamique

```c
// Allocation d'une structure
Labyrinthe* lab = malloc(sizeof(Labyrinthe));

// Allocation d'un tableau 2D
char** grille = malloc(hauteur * sizeof(char*));
for (int i = 0; i < hauteur; i++) {
    grille[i] = malloc(largeur * sizeof(char));
}
```

### Libération de Mémoire

**IMPORTANT** : Toujours libérer toute la mémoire allouée pour éviter les fuites mémoire !

```c
// Libération d'un tableau 2D
for (int i = 0; i < hauteur; i++) {
    free(grille[i]);  // Libérer chaque ligne
}
free(grille);  // Libérer le tableau de pointeurs
```

### Manipulation de Pointeurs dans les Listes

```c
// Ajout en tête (liste simple)
nouveau->suivant = liste->tete;  // Le nouveau pointe vers l'ancienne tête
liste->tete = nouveau;            // Le nouveau devient la nouvelle tête
```

## 📊 Fonctionnalités Implémentées

✅ Lecture du labyrinthe depuis un fichier  
✅ Affichage du labyrinthe  
✅ Déplacement du joueur avec contrôle des collisions  
✅ Historique des mouvements (liste chaînée simple)  
✅ Système UNDO/REDO (liste doublement chaînée)  
✅ Compteur de coups  
✅ Détection de la victoire  
✅ Gestion correcte de la mémoire  

## 🐛 Dépannage

### Erreur "impossible d'ouvrir le fichier"
- Vérifiez que le fichier `labyrinthe.txt` existe
- Vérifiez le chemin du fichier

### Le programme se bloque
- Assurez-vous que le fichier contient bien un `P` (joueur) et un `E` (sortie)

### Fuites mémoire
- Le programme libère automatiquement toute la mémoire
- Vérifiez que `liberer_labyrinthe()` est bien appelé à la fin

## 📖 Pour Aller Plus Loin

Voici des idées d'amélioration pour approfondir :

1. **Génération automatique** : Créer un algorithme pour générer des labyrinthes
2. **Meilleur score** : Sauvegarder les meilleurs temps/coups
3. **Plusieurs niveaux** : Charger différents fichiers labyrinthe
4. **Affichage coloré** : Utiliser des couleurs ANSI pour améliorer l'affichage
5. **Résolution automatique** : Implémenter un algorithme de résolution (A*, Dijkstra)

## 📝 Notes

- Le code est volontairement commenté en français pour faciliter la compréhension
- Les noms de variables sont explicites
- Le code suit les bonnes pratiques du langage C
- Gestion d'erreurs complète pour tous les appels critiques

## 👨‍🎓 Pour l'Étudiant

Prenez le temps de :
1. Lire attentivement chaque fonction
2. Comprendre comment les pointeurs sont utilisés
3. Tracer mentalement l'exécution des listes chaînées
4. Expérimenter en modifiant le code
5. Créer vos propres labyrinthes

Bon apprentissage ! 🎓

