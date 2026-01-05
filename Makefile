# Makefile pour le jeu de labyrinthe
# Usage : make (compile) ou make clean (nettoie)

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = labyrinthe
SOURCE = labyrinthe.c

# Compilation par défaut
all: $(TARGET)

# Règle de compilation
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCE)

# Nettoyage des fichiers compilés
clean:
	rm -f $(TARGET) $(TARGET).exe

# Pour Windows, on peut aussi utiliser :
windows:
	$(CC) $(CFLAGS) -o $(TARGET).exe $(SOURCE)

.PHONY: all clean windows

