#include <stdlib.h>
#include "state.h"
#include "ADTSet.h"
#include "set_utils.h"
#include "Sounds.h"

Object lastplatform = NULL;

// Οι ολοκληρωμένες πληροφορίες της κατάστασης του παιχνιδιού.
// Ο τύπος State είναι pointer σε αυτό το struct, αλλά το ίδιο το struct
// δεν είναι ορατό στον χρήστη.

struct state {
	Set objects;			// περιέχει στοιχεία Object (Πλατφόρμες, Αστέρια)
	struct state_info info;	// Γενικές πληροφορίες για την κατάσταση του παιχνιδιού
	float speed_factor;		// Πολλαπλασιαστής ταχύτητας (1 = κανονική ταχύτητα, 2 = διπλάσια, κλπ)
	struct state_sounds sounds;		// Οι ήχοι που πρέπει να αναπαραχθούν σε αυτή την κατάσταση
};

// Δημιουργεί και επιστρέφει ένα αντικείμενο

static Object create_object(ObjectType type, float x, float y, float width, float height, VerticalMovement vert_mov, float speed, bool unstable) {
	Object obj = malloc(sizeof(*obj));
	obj->type = type;
	obj->rect.x = x;
	obj->rect.y = y;
	obj->rect.width = width;
	obj->rect.height = height;
	obj->vert_mov = vert_mov;
	obj->vert_speed = speed;
	obj->unstable = unstable;
	return obj;
}

// Προσθέτει αντικείμενα στην πίστα (η οποία μπορεί να περιέχει ήδη αντικείμενα).
// Τα αντικείμενα ξεκινάνε από x = start_x, και επεκτείνονται προς τα δεξιά.
//
// ΠΡΟΣΟΧΗ: όλα τα αντικείμενα έχουν συντεταγμένες x,y σε ένα
// καρτεσιανό επίπεδο.
// - Στο άξονα x το 0 είναι η αρχή της πίστας και οι συντεταγμένες
//   μεγαλώνουν προς τα δεξιά.
//
// - Στον άξονα y το 0 είναι το πάνω μέρος της πίστας, και οι συντεταγμένες μεγαλώνουν
//   προς τα _κάτω_. Η πίστα έχει σταθερό ύψος, οπότε όλες οι
//   συντεταγμένες y είναι ανάμεσα στο 0 και το SCREEN_HEIGHT.
//
// Πέρα από τις συντεταγμένες, αποθηκεύουμε και τις διαστάσεις width,height
// κάθε αντικειμένου. Τα x,y,width,height ορίζουν ένα παραλληλόγραμμο, οπότε
// μπορούν να αποθηκευτούν όλα μαζί στο obj->rect τύπου Rectangle (ορίζεται
// στο include/raylib.h). Τα x,y αναφέρονται στην πάνω-αριστερά γωνία του Rectangle.

static void add_objects(State state, float start_x) {
	// Προσθέτουμε PLATFORM_NUM πλατφόρμες, με τυχαία χαρακτηριστικά.
	srand(1);
	for (int i = 0; i < PLATFORM_NUM; i++) {
		Object platform = create_object(
			PLATFORM,
			start_x + 150 + rand() % 80,						// x με τυχαία απόσταση από το προηγούμενο στο διάστημα [150, 230]
			SCREEN_HEIGHT/4 + rand() % SCREEN_HEIGHT/2,			// y τυχαία στο διάστημα [SCREEN_HEIGHT/4, 3*SCREEN_HEIGHT/4]
			i == 0 ? 250 : 50 + rand()%200,						// πλάτος τυχαία στο διάστημα [50, 250] (η πρώτη πάντα 250)
			20,													// ύψος
			i < 3 || rand() % 2 == 0 ? MOVING_UP : MOVING_DOWN,	// τυχαία αρχική κίνηση (οι πρώτες 3 πάντα πάνω)
			0.6 + 3*(rand()%100)/100,							// ταχύτητα τυχαία στο διάστημα [0.6, 3.6]
			i > 0 && (rand() % 10) == 0							// το 10% (τυχαία) των πλατφορμών είναι ασταθείς (εκτός από την πρώτη)
		);
		set_insert(state->objects, platform);
		lastplatform  = platform;

		// Στο 50% των πλατφορμών (τυχαία), εκτός της πρώτης, προσθέτουμε αστέρι
		if(i != 0 && rand() % 2 == 0) {
			Object star = create_object(
				STAR,
				start_x + 200 + rand() % 60,				// x με τυχαία απόσταση από την προηγούμενη πλατφόρμα στο διάστημα [200,260]
				SCREEN_HEIGHT/8 + rand() % SCREEN_HEIGHT/2,	// y τυχαία στο διάστημα [SCREEN_HEIGHT/8, 5*SCREEN_HEIGHT/8]
				30, 30,										// πλάτος, ύψος
				IDLE,										// χωρίς κίνηση
				0,										 	// ταχύτητα 0
				false										// 'unstable' πάντα false για τα αστέρια
			);
			set_insert(state->objects, star);
		}

		start_x = platform->rect.x + platform->rect.width;	// μετακίνηση των επόμενων αντικειμένων προς τα δεξιά
	}
}

int compare_objects(Pointer a, Pointer b) {
	Object obj1 = (Object) a;
	Object obj2 = (Object) b;
	if((int)obj1->rect.x > (int)obj2->rect.x) return 1;
	else if((int)obj1->rect.x < (int)obj2->rect.x) return -1;
	else return 0;
}

// Δημιουργεί και επιστρέφει την αρχική κατάσταση του παιχνιδιού

State state_create() {
	// Δημιουργία του state
	State state = malloc(sizeof(*state));

	// Γενικές πληροφορίες
	state->info.playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	state->info.paused = false;				// Χωρίς να είναι paused.
	state->speed_factor = 1;				// Κανονική ταχύτητα
	state->info.score = 0;				// Αρχικό σκορ 0

	// Δημιουργούμε το set των αντικειμένων, και προσθέτουμε αντικείμενα
	// ξεκινώντας από start_x = 0.
	state->objects = set_create((CompareFunc)compare_objects, NULL);
	add_objects(state, 0);

	// Δημιουργούμε την μπάλα τοποθετώντας τη πάνω στην πρώτη πλατφόρμα
	Object firstpixel = create_object(
		PLATFORM,
		0, 
		0, 
		1, 
		1, 
		IDLE, 
		0, 
		false
	);
	Object first_platform = set_find_eq_or_greater(state->objects, firstpixel);
	free(firstpixel);
	state->info.ball = create_object(
		BALL,
		first_platform->rect.x,			// x στην αρχή της πλατφόρμας
		first_platform->rect.y - 40,	// y πάνω στην πλατφόρμα
		45, 45,							// πλάτος, ύψος
		IDLE,							// χωρίς αρχική κατακόρυφη κίνηση
		0,								// αρχική ταχύτητα 0
		false							// "unstable" πάντα false για την μπάλα
	);

	return state;
}

// Επιστρέφει τις βασικές πληροφορίες του παιχνιδιού στην κατάσταση state

StateInfo state_info(State state) {
	StateInfo info = malloc(sizeof(*info));
	info->playing = state->info.playing;
	info->paused = state->info.paused;
	info->score = state->info.score;
	info->ball = state->info.ball;
	return info;
}

Statesounds sounds_info(State state) {
	Statesounds sounds = malloc(sizeof(*sounds));
	sounds->gameover = state->sounds.gameover;
	sounds->jumping = state->sounds.jumping;
	sounds->gotStar = state->sounds.gotStar;
	return sounds;
}

// Επιστρέφει μια λίστα με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state,
// των οποίων η συντεταγμένη y είναι ανάμεσα στο x_from και x_to.

List state_objects(State state, float x_from, float x_to) {
	List list_objects = list_create(NULL);
	Object xFromObj = create_object(
		PLATFORM,
		x_from,
		0,
		1,
		1,
		IDLE,
		0,
		false
	);
	Object xToObj = create_object(
		PLATFORM,
		x_to,
		0,
		1,
		1,
		IDLE,
		0,
		false
	);
	xFromObj = (Object) set_find_eq_or_greater(state->objects, xFromObj);
	xToObj = (Object) set_find_eq_or_smaller(state->objects, xToObj);
	if(xFromObj == NULL || xToObj == NULL) {
		return list_objects;
	}
	for(SetNode node = set_find_node(state->objects, xFromObj); node != set_next(state->objects,set_find_node(state->objects, xToObj)); node = set_next(state->objects, node)) {
		Object object = set_node_value(state->objects, node);
		list_insert_next(list_objects, LIST_BOF, object);
	}
	return list_objects;
}

void game_over(State state){
	state->info.playing = false;
}

void state_init(State state){
	set_destroy(state->objects);
	state->info.playing = true;				// Το παιχνίδι ξεκινάει αμέσως
	state->info.paused = false;				// Χωρίς να είναι paused.
	state->speed_factor = 1;				// Κανονική ταχύτητα
	state->info.score = 0;				// Αρχικό σκορ 0

	 state->objects = set_create((CompareFunc)compare_objects, NULL);
	 add_objects(state, 0);

	// Δημιουργούμε το set των αντικειμένων, και προσθέτουμε αντικείμενα
	// ξεκινώντας από start_x = 0.

	// Δημιουργούμε την μπάλα τοποθετώντας τη πάνω στην πρώτη πλατφόρμα
	Object firstpixel = create_object(
		PLATFORM,
		0, 
		0, 
		1, 
		1, 
		IDLE, 
		0, 
		false
	);
	Object first_platform = set_find_eq_or_greater(state->objects, firstpixel);
	free(firstpixel);
	free(state->info.ball);
	state->info.ball = create_object(
		BALL,
		first_platform->rect.x,			// x στην αρχή της πλατφόρμας
		first_platform->rect.y - 40,	// y πάνω στην πλατφόρμα
		45, 45,							// πλάτος, ύψος
		IDLE,							// χωρίς αρχική κατακόρυφη κίνηση
		0,								// αρχική ταχύτητα 0
		false							// "unstable" πάντα false για την μπάλα
	);
}

Object findCurrentPlatform(State state){
	Object currentPlatform = (Object) set_find_eq_or_smaller(state->objects, state->info.ball);
	while(true){
		if(currentPlatform->type == PLATFORM){
			return currentPlatform;
		}
		currentPlatform = (Object) set_node_value(state->objects, set_previous(state->objects, set_find_node(state->objects, currentPlatform)));
	}
}

Object findNextPlatform(State state){
	Object nextPlatform = (Object) set_node_value(state->objects, set_next(state->objects, set_find_node(state->objects, findCurrentPlatform(state))));
	while(true){
		if(nextPlatform->type == PLATFORM){
			return nextPlatform;
		}
		nextPlatform = (Object) set_node_value(state->objects, set_next(state->objects, set_find_node(state->objects, nextPlatform)));
	}
}

Object findNextStar(State state){
	Object nextPlatform = (Object) set_node_value(state->objects, set_next(state->objects, set_find_node(state->objects, findCurrentPlatform(state))));
	while(true){
		if(nextPlatform->type == STAR){
			return nextPlatform;
		}
		nextPlatform = (Object) set_node_value(state->objects, set_next(state->objects, set_find_node(state->objects, nextPlatform)));
	}
}

// Ενημερώνει την κατάσταση state του παιχνιδιού μετά την πάροδο 1 frame.
// Το keys περιέχει τα πλήκτρα τα οποία ήταν πατημένα κατά το frame αυτό.
Object nextPlatform;
Object currPlatform;
Object nextStar;
void state_update(State state, KeyState keys) { 
	nextStar = findNextStar(state);
	state->sounds.gameover = false;
	state->sounds.jumping = false;
	state->sounds.gotStar = false;
	if(state->info.playing && !state->info.paused){
		if(keys->up && state->info.ball->vert_mov == IDLE){
			state->sounds.jumping = true;
			state->info.ball->vert_mov = JUMPING;
			state->info.ball->vert_speed = 17;
		}
		if(keys->right){
			state->info.ball->rect.x += 6 * state->speed_factor;
		}
		else if(keys->left){
			state->info.ball->rect.x += 1 * state->speed_factor;
		}
		else if(keys->p){
			state->info.paused = true;
			return;
		}
		else{
			state->info.ball->rect.x += 4 * state->speed_factor;
		}
		if(state->info.ball->rect.y > SCREEN_HEIGHT){
			state->sounds.gameover = true;
			game_over(state);
			return;
		}
		if(CheckCollisionRecs(state->info.ball->rect, nextStar->rect)){
			state->sounds.gotStar = true;
			state->info.score += 10;
			set_remove(state->objects, nextStar);
			nextStar = findNextStar(state);
		}
		if(state->info.ball->vert_speed <= 0.5 && state->info.ball->vert_mov == JUMPING){
			state->info.ball->vert_mov = FALLING;
		}
		if(state->info.ball->vert_mov == IDLE){
			currPlatform = findCurrentPlatform(state);
			nextPlatform = findNextPlatform(state);
			nextStar = findNextStar(state);
			if(state_info(state)->ball->rect.x + state_info(state)->ball->rect.width >= currPlatform->rect.x && state_info(state)->ball->rect.x <= currPlatform->rect.x + currPlatform->rect.width){
				state->info.ball->rect.y = currPlatform->rect.y - state->info.ball->rect.height;
			}
			else {
				state->info.ball->vert_mov = FALLING;
				state->info.ball->vert_speed = 1.5;
			}
		}
		else if(state->info.ball->vert_mov == JUMPING){
			currPlatform = findCurrentPlatform(state);
			nextPlatform = findNextPlatform(state);	
			nextStar = findNextStar(state);		
			state->info.ball->rect.y -= state->info.ball->vert_speed;
			state->info.ball->vert_speed = (85*state->info.ball->vert_speed)/100;
		}
		else if(state->info.ball->vert_mov == FALLING){
			nextStar = findNextStar(state);
			if(CheckCollisionRecs(state->info.ball->rect, currPlatform->rect) && state->info.ball->rect.y + state->info.ball->rect.height <= currPlatform->rect.y + currPlatform->rect.height/2){
				state->info.ball->vert_mov = IDLE;
				state->info.ball->rect.y = currPlatform->rect.y - state->info.ball->rect.height;
				state->info.ball->vert_speed = 0;
			}
			if(CheckCollisionRecs(state->info.ball->rect, nextPlatform->rect) && state->info.ball->rect.y + state->info.ball->rect.height <= nextPlatform->rect.y + nextPlatform->rect.height/2){
				if(nextPlatform->unstable){
					nextPlatform->vert_mov = FALLING;
				}
				state->info.ball->vert_mov = IDLE;
				state->info.ball->rect.y = nextPlatform->rect.y - state->info.ball->rect.height;
				state->info.ball->vert_speed = 0;
			}
			state->info.ball->rect.y += state->info.ball->vert_speed;
			state->info.ball->vert_speed += (10*state->info.ball->vert_speed)/100;
			if(state->info.ball->vert_speed >= 7){
				state->info.ball->vert_speed = 7;
			}
		}
		for(SetNode node = set_find_node(state->objects, set_find_eq_or_greater(state->objects, create_object(PLATFORM, state->info.ball->rect.x - SCREEN_WIDTH , 0, 1, 1, IDLE, 0, false))); node != set_next(state->objects, set_find_node(state->objects, set_find_eq_or_smaller(state->objects, create_object(PLATFORM, state->info.ball->rect.x + 2 * SCREEN_WIDTH, 0, 1, 1, IDLE, 0, false)))); node = set_next(state->objects, node)){
			Object object = set_node_value(state->objects, node);
			if(object->type == PLATFORM){
				if(object->vert_mov == MOVING_UP){
					object->rect.y -= object->vert_speed * state->speed_factor;
					if(object->rect.y <= SCREEN_HEIGHT/4){
						object->vert_mov = MOVING_DOWN;
					}
				}
				else if(object->vert_mov == MOVING_DOWN){
					object->rect.y += object->vert_speed * state->speed_factor;
					if(object->rect.y >= 3*SCREEN_HEIGHT/4){
						object->vert_mov = MOVING_UP;
					}
				}
				else if(object->vert_mov == FALLING){
					if(object->rect.y > SCREEN_HEIGHT){
					node = set_previous(state->objects, node);
					set_remove(state->objects, object);
					currPlatform = findCurrentPlatform(state);

					}
					else{
						object->rect.y += 4 * state->speed_factor;
					}
				}
			}
		}
		if(lastplatform != NULL && state->info.ball->rect.x + state->info.ball->rect.width + SCREEN_WIDTH >= lastplatform->rect.x){
			state->speed_factor += 0.1 * state->speed_factor;
			add_objects(state, lastplatform->rect.x + lastplatform->rect.width);
		}
	}	
	else if(state->info.paused && state->info.playing){
		if(keys->n){
			state->info.paused = false;
			state_update(state, keys);
			state->info.paused = true;
		}
		else if(keys->p){
			state->info.paused = false;
		}
	}
	else{
		if(keys->enter){
			state_init(state);
		}
	}
}

// Καταστρέφει την κατάσταση state ελευθερώνοντας τη δεσμευμένη μνήμη.

void state_destroy(State state) {
	// Προς υλοποίηση
}