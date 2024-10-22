///////////////////////////////////////////////////////////////////
//
// vec2
//
// Module για επεξεργασία 2d διανυσμάτων. 
//
///////////////////////////////////////////////////////////////////


#pragma once

#include "raylib.h"		// Ο τύπος Vector2 ορίζεται στη raylib



// Επιστρέφει το άθροισμα δύο διανυσμάτων

Vector2 vec2_add(Vector2 vec1, Vector2 vec2);

// Πολλαπλασιάζει ένα διάνυσμα vec με έναν πραγματικό αριθμό scalar

Vector2 vec2_scale(Vector2 vec, double scalar);

// Περιστρέφει το διάνυσμα vec κατά angle radians γύρω από την αρχή των αξόνων

Vector2 vec2_rotate(Vector2 vec, double angle);

// Επιστρέφει την Ευκλείδεια απόσταση ανάμεσα σε δύο διανύσματα

double vec2_distance(Vector2 vec1, Vector2 vec2);

// Επιστρέφει ένα διάνυσμα με μήκος length και γωνία angle.

Vector2 vec2_from_polar(double length, double angle);
