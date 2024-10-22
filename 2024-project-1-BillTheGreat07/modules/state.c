
#include <stdlib.h>

#include "ADTVector.h"
#include "ADTList.h"
#include "state.h"
#include "vec2.h"


// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	Vector objects;			// περιέχει στοιχεία Object (αστεροειδείς, σφαίρες)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	int next_bullet;		// Αριθμός frames μέχρι να επιτραπεί ξανά σφαίρα
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
};


// Δημιουργεί και επιστρέφει ένα αντικείμενο

static Object create_object(ObjectType type, Vector2 position, Vector2 speed, Vector2 orientation, double size) {
	Object obj = malloc(sizeof(*obj));
	obj->type = type;
	obj->position = position;
	obj->speed = speed;
	obj->orientation = orientation;
	obj->size = size;
	return obj;
}

// Επιστρέφει έναν τυχαίο πραγματικό αριθμό στο διάστημα [min,max]

static double randf(double min, double max) {
	return min + (double)rand() / RAND_MAX * (max - min);
}

// Προσθέτει num αστεροειδείς στην πίστα (η οποία μπορεί να περιέχει ήδη αντικείμενα).
//
// ΠΡΟΣΟΧΗ: όλα τα αντικείμενα έχουν συντεταγμένες x,y σε ένα καρτεσιανό επίπεδο.
// - Η αρχή των αξόνων είναι η θέση του διαστημόπλοιου στην αρχή του παιχνιδιού
// - Στο άξονα x οι συντεταγμένες μεγαλώνουν προς τα δεξιά.
// - Στον άξονα y οι συντεταγμένες μεγαλώνουν προς τα πάνω.

static void add_asteroids(State state, int num) {
	for (int i = 0; i < num; i++) {
		// Τυχαία θέση σε απόσταση [ASTEROID_MIN_DIST, ASTEROID_MAX_DIST]
		// από το διστημόπλοιο.
		//
		Vector2 position = vec2_add(
			state->info.spaceship->position,
			vec2_from_polar(
				randf(ASTEROID_MIN_DIST, ASTEROID_MAX_DIST),	// απόσταση
				randf(0, 2*PI)									// κατεύθυνση
			)
		);

		// Τυχαία ταχύτητα στο διάστημα [ASTEROID_MIN_SPEED, ASTEROID_MAX_SPEED]
		// με τυχαία κατεύθυνση.
		//
		Vector2 speed = vec2_from_polar(
			randf(ASTEROID_MIN_SPEED, ASTEROID_MAX_SPEED) * state->speed_factor,
			randf(0, 2*PI)
		);

		Object asteroid = create_object(
			ASTEROID,
			position,
			speed,
			(Vector2){0, 0},								// δεν χρησιμοποιείται για αστεροειδείς
			randf(ASTEROID_MIN_SIZE, ASTEROID_MAX_SIZE)		// τυχαίο μέγεθος
		);
		vector_insert_last(state->objects, asteroid);
	}
}

// Συνάρτηση για την αφαίρεση ενός στοιχείου από οποιαδήποτε θέση στο vector

bool vector_remove_at(Vector vec, int pos) {
    int size = vector_size(vec);
    if(pos < 0 || pos >= size) {
        return false; // Εκτός ορίων δεν γίνεται η αφαίρεση
    }

    if(pos == size - 1) {
        // Αν το στοιχείο για αφαίρεση είναι το τελευταίο, απλά αφαιρείται
        vector_remove_last(vec);
    } else {
        // Αντικατάστησε το στοιχείο στη θέση pos με το τελευταίο στοιχείο
        Pointer last_item = vector_get_at(vec, size - 1);
        vector_set_at(vec, pos, last_item);
        vector_remove_last(vec); // Αφαιρείται το τελευταίο στοιχείο
    }

    return true;
}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.paused = false;				// Το παιχνίδι ξεκινάει χωρίς να είναι paused.
	state->speed_factor = 1;				// Κανονική ταχύτητα
	state->next_bullet = 0;					// Σφαίρα επιτρέπεται αμέσως
	state->info.score = 0;				// Αρχικό σκορ 0

	// Δημιουργούμε το vector των αντικειμένων, και προσθέτουμε αντικείμενα
	state->objects = vector_create(0, NULL);

	// Δημιουργούμε το διαστημόπλοιο
	state->info.spaceship = create_object(
		SPACESHIP,
		(Vector2){0, 0},			// αρχική θέση στην αρχή των αξόνων
		(Vector2){0, 0},			// μηδενική αρχική ταχύτητα
		(Vector2){0, 1},			// κοιτάει προς τα πάνω
		SPACESHIP_SIZE				// μέγεθος
	);

	// Προσθήκη αρχικών αστεροειδών
	add_asteroids(state, ASTEROID_NUM);

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {
    if(state == NULL) return NULL;
    return &state->info;
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η θέση position βρίσκεται εντός του παραλληλογράμμου με πάνω αριστερή
// γωνία top_left και κάτω δεξιά bottom_right.

List state_objects(State state, Vector2 top_left, Vector2 bottom_right) {
    List visible_objects = list_create(NULL);
    if(state == NULL) return visible_objects;

	ListNode node = LIST_BOF;
    for(int i = 0; i < vector_size(state->objects); i++) {
        Object obj = vector_get_at(state->objects, i);
		
        if(obj->position.x >= top_left.x && obj->position.x <= bottom_right.x && obj->position.y >= top_left.y && obj->position.y <= bottom_right.y) {
            list_insert_next(visible_objects, node, obj);
			node = list_next(visible_objects, node);
        }
    }

    return visible_objects;
}

// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.

void state_update(State state, KeyState keys) {
    if(state == NULL || keys == NULL) return;

    state->info.spaceship->position = vec2_add(state->info.spaceship->position, vec2_scale(state->info.spaceship->speed, state->speed_factor));

    if(keys->left) {
        state->info.spaceship->orientation = vec2_rotate(state->info.spaceship->orientation, -SPACESHIP_ROTATION);
    }
    if(keys->right) {
        state->info.spaceship->orientation = vec2_rotate(state->info.spaceship->orientation, SPACESHIP_ROTATION);
    }

    if(keys->up) {
        Vector2 acceleration = vec2_scale(state->info.spaceship->orientation, SPACESHIP_ACCELERATION);
        state->info.spaceship->speed = vec2_add(state->info.spaceship->speed, acceleration);
    } else {
		Vector2 deceleration = vec2_scale(state->info.spaceship->orientation, -SPACESHIP_SLOWDOWN);
		state->info.spaceship->speed = vec2_add(state->info.spaceship->speed, deceleration);

		if(state->info.spaceship->speed.x < 0) state->info.spaceship->speed.x = 0;
		if(state->info.spaceship->speed.y < 0) state->info.spaceship->speed.y = 0;
	}

    for(int i = 0; i < vector_size(state->objects); i++) {
        Object obj = vector_get_at(state->objects, i);
        obj->position = vec2_add(obj->position, vec2_scale(obj->speed, state->speed_factor));
    }

	// ΑΣΚΗΣΗ 3

	// Ελέγχος και προσθήκη αστεροειδών αν χρειάζεται
	int current_asteroids = 0;
	for(int i = 0; i < vector_size(state->objects); i++) { // Δεν χρησιμοποιώ την state_objects() διότι θέλω το rendering να γίνεται εκτός οθόνης (θεωρώντας αρκετά μεγάλο ASTEROID_MAX_DIST)
		Object obj = vector_get_at(state->objects, i);
		if(obj->type == ASTEROID && vec2_distance(obj->position, state->info.spaceship->position) <= ASTEROID_MAX_DIST) {
			current_asteroids++;
		}
	}

	int asteroidsToAdd = ASTEROID_NUM - current_asteroids;
	if(asteroidsToAdd > 0) {
		add_asteroids(state, asteroidsToAdd);
		state->info.score += 1;
	}

	// Συγκρούσεις
	for(int i = 0; i < vector_size(state->objects); i++) {
		Object obj = vector_get_at(state->objects, i);

		// Συγκρούση διαστημοπλοίου με αστεροειδή
		if(obj->type == ASTEROID && CheckCollisionCircles(obj->position, obj->size, state->info.spaceship->position, SPACESHIP_SIZE)) {
			vector_remove_at(state->objects, i);
			state->info.score = state->info.score/2;

			i--; // Διόρθωση του index λόγω αφαίρεσης στοιχείου
		}

		// Συγκρούση σφαίρας με αστεροειδή
		for(int j = i + 1; j < vector_size(state->objects); j++) {
			Object obj2 = vector_get_at(state->objects, j);
			if(obj->type == BULLET && obj2->type == ASTEROID && CheckCollisionCircles(obj->position, obj->size, obj2->position, obj2->size)) {
				vector_remove_at(state->objects, j); // Διαγράφεται ο αστεροειδής
				j--;

				if(obj2->size/2 >= ASTEROID_MIN_SIZE) {
					for(int k = 0; k < 2; k++) {
						Object asteroid = create_object(ASTEROID, obj2->position, vec2_scale(vec2_scale(obj2->speed, 2.5), state->speed_factor), vec2_rotate(obj2->orientation, randf(0, 2*PI)), obj2->size/2);
						vector_insert_last(state->objects, asteroid);
					}
				}

				vector_remove_at(state->objects, i); // Διαγράφεται η σφαίρα
				i--;

				state->info.score -= 10;

				break; // Έξοδος από το loop γιατί η σφαίρα καταστράφηκε
			}
		}
	}

	// Σφαίρες
	state->next_bullet--;
	if(keys->space && state->next_bullet <= 0) {
		Vector2 bullet_position = state->info.spaceship->position;
		Vector2 bullet_speed = vec2_scale(vec2_add(state->info.spaceship->speed, vec2_scale(state->info.spaceship->orientation, BULLET_SPEED)), state->speed_factor);

		Object bullet = create_object(BULLET, bullet_position, bullet_speed, state->info.spaceship->orientation, BULLET_SIZE);
		vector_insert_last(state->objects, bullet);
		state->next_bullet = BULLET_DELAY;  // Ρύθμιση καθυστέρησης για την επόμενη σφαίρα
	}
	
	if(state->info.score%100 == 0) state->speed_factor *= 1.1;
}

// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	// Προς υλοποίηση
}
