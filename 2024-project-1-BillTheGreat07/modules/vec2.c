#include <math.h>

#include "vec2.h"


Vector2 vec2_add(Vector2 vec1, Vector2 vec2) {
	return (Vector2){
		vec1.x + vec2.x,
		vec1.y + vec2.y,
	};
}

Vector2 vec2_scale(Vector2 vec, double scalar) {
	return (Vector2){
		vec.x * scalar,
		vec.y * scalar,
	};
}

Vector2 vec2_rotate(Vector2 vec, double angle) {
	return (Vector2){
		vec.x * cos(angle) - vec.y * sin(angle),
		vec.x * sin(angle) + vec.y * cos(angle),
	};
}

double vec2_distance(Vector2 vec1, Vector2 vec2) {
	double xd = vec1.x - vec2.x;
	double yd = vec1.y - vec2.y;
	return sqrt(xd*xd + yd*yd);
}

Vector2 vec2_from_polar(double length, double angle) {
	return vec2_scale( vec2_rotate((Vector2){0, 1}, angle), length);
}