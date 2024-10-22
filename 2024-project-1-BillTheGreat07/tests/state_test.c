//////////////////////////////////////////////////////////////////
//
// Test για το state.h module
//
//////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include "ADTSet.h"
#include "ADTVector.h"
#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing
#include "vec2.h"

#include "state.h"

#include "set_utils.h"

struct state {
	Vector objects;			// περιέχει στοιχεία Object (αστεροειδείς, σφαίρες)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	int next_bullet;		// Αριθμός frames μέχρι να επιτραπεί ξανά σφαίρα
	float speed_factor;		// Πολλαπλασιαστής ταχύτητς (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
};

///// Βοηθητικές συναρτήσεις ////////////////////////////////////////
//
// Ελέγχει την (προσεγγιστική) ισότητα δύο double
// (λόγω λαθών το a == b δεν είναι ακριβές όταν συγκρίνουμε double).
static bool double_equal(double a, double b) {
	return abs(a-b) < 1e-6;
}

// Ελέγχει την ισότητα δύο διανυσμάτων
static bool vec2_equal(Vector2 a, Vector2 b) {
	return double_equal(a.x, b.x) && double_equal(a.y, b.y);
}
/////////////////////////////////////////////////////////////////////


void test_state_create() {

	State state = state_create();
	TEST_ASSERT(state != NULL);

	TEST_ASSERT(state->next_bullet == 0);
	TEST_ASSERT(state->speed_factor == 1);

	StateInfo info = state_info(state);
	TEST_ASSERT(info != NULL);

	TEST_ASSERT(!info->paused);
	TEST_ASSERT(info->score == 0);

    List objects = state_objects(state, (Vector2){0, 0}, (Vector2){SCREEN_WIDTH, SCREEN_HEIGHT});
    TEST_ASSERT(list_size(objects) == ASTEROID_NUM);
    TEST_ASSERT(vec2_equal(info->spaceship->position, (Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2}));

	ListNode node = LIST_BOF;
    for(int i = 0; i < list_size(objects); i++) {
		node = list_next(objects, node);
		if(node == LIST_EOF) break;

        Object obj = list_node_value(objects, node);

        TEST_ASSERT(obj->type == ASTEROID);
        TEST_ASSERT(obj->position.x >= 0 && obj->position.x <= SCREEN_WIDTH);
        TEST_ASSERT(obj->position.y >= 0 && obj->position.y <= SCREEN_HEIGHT);
    }

    state_destroy(state);
}

void test_state_update() {
	State state = state_create();
	TEST_ASSERT(state != NULL && state_info(state) != NULL);

	// Διασφαλίζουμε ότι το αρχικό σκορ είναι 0 και το speed_factor είναι 1
	TEST_ASSERT(state->info.score == 0);
	TEST_ASSERT(double_equal(state->speed_factor, 1.0));

	// Πληροφορίες για τα πλήκτρα (αρχικά κανένα δεν είναι πατημένο)
	struct key_state keys = { false, false, false, false, false, false, false };
	
	// Χωρίς κανένα πλήκτρο, το διαστημόπλοιο παραμένει σταθερό με μηδενική ταχύτητα
	state_update(state, &keys);

	TEST_ASSERT( vec2_equal( state_info(state)->spaceship->position, (Vector2){0,0}) );
	TEST_ASSERT( vec2_equal( state_info(state)->spaceship->speed,    (Vector2){0,0}) );

	// Με πατημένο το πάνω βέλος, η ταχήτητα αυξάνεται ενώ το διαστημόπλοιο παραμένει για την ώρα ακίνητο
	keys.up = true;
	state_update(state, &keys);

	TEST_ASSERT( vec2_equal( state_info(state)->spaceship->position, (Vector2){0,0}) );
	TEST_ASSERT( vec2_equal( state_info(state)->spaceship->speed,    (Vector2){0,SPACESHIP_ACCELERATION}) );

	
	keys.up = false;

	// Περιστροφή δεξιά
    keys.right = true;
    state_update(state, &keys);

    TEST_ASSERT( vec2_equal(state_info(state)->spaceship->position, (Vector2){0,SPACESHIP_ACCELERATION}) );
    TEST_ASSERT( vec2_equal(state_info(state)->spaceship->orientation, vec2_rotate((Vector2){0,0}, SPACESHIP_ROTATION)) );

    // Περιστροφή αριστερά
    keys.right = false;
    keys.left = true;
    state_update(state, &keys);

    TEST_ASSERT( vec2_equal(state_info(state)->spaceship->orientation, (Vector2){0, 0}) );

    // Αφήνοντας όλα τα πλήκτρα
    keys.left = false;

	Vector2 previousSpeed = state_info(state)->spaceship->speed;

    state_update(state, &keys);

    TEST_ASSERT( !vec2_equal(state_info(state)->spaceship->speed, previousSpeed) );

	// Άσκηση 3
	keys.space = true;
	int initial_asteroids = vector_size(state->objects);

	state_update(state, &keys);

	// Έλεγχος: Προστέθηκε νέο αντικείμενο (σφαίρα) 
	Object last_added = vector_get_at(state->objects, vector_size(state->objects) - 1);

	TEST_ASSERT(vector_size(state->objects) == initial_asteroids + 1 && last_added->type == BULLET); // +1 σφαίρα

	// Προσομοίωση: Συγκρούσεις
	for(int i = 0; i < vector_size(state->objects); i++) {
		Object obj = vector_get_at(state->objects, i);

		if(obj->type == ASTEROID) {
			obj->position = last_added->position;
			break;
		}
	}

	int previousScore = state->info.score;

	keys.space = false;
	state_update(state, &keys);

	TEST_ASSERT(state->info.score <= previousScore - 10); // Το σκορ μειώθηκε κατά 10 λόγω σύγκρουσης

	state->info.score = 100;
	state_update(state, &keys);

	TEST_ASSERT(double_equal(state->speed_factor, (state->info.score % 100 == 0) ? 1.1 : 1.0)); // Αύξηση της ταχύτητας αν το σκορ είναι πολλαπλάσιο του 100

    state_destroy(state);
}

int compare_ints(Pointer a, Pointer b) {
    int int_a = *(int *)a;
    int int_b = *(int *)b;

    return (int_a > int_b) - (int_a < int_b);
}

void test_set_utils() {
    Set set = set_create(compare_ints, NULL);
    int values[] = {10, 20, 30, 40, 50};

    for(int i = 0; i < sizeof(values)/sizeof(values[0]); i++) {
        set_insert(set, &values[i]);
    }

    int test_val = 25;
    int *result;

    result = set_find_eq_or_greater(set, &test_val);
    TEST_CHECK(*result == 30);

    result = set_find_eq_or_smaller(set, &test_val);
    TEST_CHECK(*result == 20);

    set_destroy(set);
}

// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_state_create", test_state_create },
	{ "test_state_update", test_state_update },
	{ "test_set_utils", test_set_utils},

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};