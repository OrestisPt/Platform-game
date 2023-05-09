//////////////////////////////////////////////////////////////////
//
// Test για το state.h module
//
//////////////////////////////////////////////////////////////////

#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "state.h"
#include "Sounds.h"

// Θέτει την κατάσταση state σε !playing, δηλαδή σε game over.

void game_over(State state);

// Επιστρέφει ενα Vector με όλα τα αντικείμενα του παιχνιδιού στην κατάσταση state.

void test_state_create() {

	State state = state_create();
	TEST_ASSERT(state != NULL);

	StateInfo info = state_info(state);
	TEST_ASSERT(info != NULL);

	TEST_ASSERT(info->playing);
	TEST_ASSERT(!info->paused);
	TEST_ASSERT(info->score == 0);
	// Προσθέστε επιπλέον ελέγχους
	List list = state_objects(state, 0, SCREEN_WIDTH);
	TEST_ASSERT(list_size(list) != 0);
	list = state_objects(state, 0, 0);
	TEST_ASSERT(list_size(list) == 0);
	list = state_objects(state, 0, state_info(state)->ball->rect.x);
	TEST_ASSERT(list_size(list) == 1);
	Object firstpl = list_node_value(list, list_first(list));
	TEST_ASSERT(firstpl->rect.x == state_info(state)->ball->rect.x);
	list = state_objects(state, 0, firstpl->rect.x + firstpl->rect.width);
	TEST_ASSERT(list_size(list) == 1);
	Object obj  = list_node_value(list, list_first(list));
	TEST_ASSERT(obj->type == PLATFORM);
}

void test_state_update() {
	State state = state_create();
	TEST_ASSERT(state != NULL && state_info(state) != NULL);

	// Πληροφορίες για τα πλήκτρα (αρχικά κανένα δεν είναι πατημένο)
	struct key_state keys = { false, false, false, false, false, false, false };
	
	// Χωρίς κανένα πλήκτρο, η μπάλα μετακινείται 4 pixels δεξιά
	Rectangle old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	Rectangle new_rect = state_info(state)->ball->rect;

	TEST_CHECK( new_rect.x == old_rect.x + 4 );

	// Η καταστάση δεν ενημερώνεται με το πάτημα του πλήκτρου p (pause)
	keys.p = true;
	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;
	keys.p = false;

	TEST_CHECK( new_rect.x == old_rect.x);

	//Με το πάτημα του πλήκτρου n (new game), η κατάσταση ενημερώνεται για ενα frame
	keys.n = true;
	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;
	keys.n = false;

	TEST_CHECK( new_rect.x == old_rect.x + 4);

	// Επιβεβαιώνουμε οτι η κατάσταση ακόμα δεν ενημερώθηκε πριν το πάτημα του p
	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;

	TEST_CHECK( new_rect.x == old_rect.x);

	// Με το πάτημα του πλήκτρου p (pause), η κατάσταση ενημερώνεται εκ νέου (χρειάζονται 2 frames)
	keys.p = true;
	state_update(state, &keys);
	keys.p = false;
	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;

	TEST_CHECK( new_rect.x == old_rect.x + 4);

	// Με πατημένο το δεξί βέλος, η μπάλα μετακινείται 6 pixels δεξιά
	keys.right = true;
	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;
	keys.right = false;

	TEST_CHECK( new_rect.x == old_rect.x + 6 );

	// Με πατημένο το αριστερό βέλος, η μπάλα μετακινείται 1 pixel δεξιά
	keys.left = true;
	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;
	keys.left = false;

	TEST_CHECK( new_rect.x == old_rect.x + 1 );

	// Με πατημένο το πάνω βέλος, η μπάλα μπαίνει σε κατάσταση άλματος και μετακινείται 17 pixels πάνω 
	keys.up = true;
	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	int y = state_info(state)->ball->vert_mov;
	new_rect = state_info(state)->ball->rect;
	keys.up = false;

	TEST_CHECK( y == JUMPING);
	TEST_CHECK( new_rect.y == old_rect.y - 17);

	// Σε κατάσταση που η μπάλα πηδάει, η ταχύτητα μειώνεται στο 85% της προηγουμενης τιμης
	old_rect = state_info(state)->ball->rect;
	float old_speed = state_info(state)->ball->vert_speed;
	state_update(state, &keys);
	float new_speed = state_info(state)->ball->vert_speed;
	y = state_info(state)->ball->vert_mov;
	new_rect = state_info(state)->ball->rect;

	TEST_CHECK( y == JUMPING);
	TEST_CHECK( new_rect.y == old_rect.y - old_speed);
	TEST_CHECK( new_speed == (float)(85*old_speed)/100);


	// Σε κατάσταση που η μπάλα πέφτει, η ταχύτητα αυξάνεται κατά 10%
	old_rect = state_info(state)->ball->rect;
	state_info(state)->ball->vert_speed = 0.5;
	old_speed = state_info(state)->ball->vert_speed;
	state_update(state, &keys);
	new_speed = state_info(state)->ball->vert_speed;
	y = state_info(state)->ball->vert_mov;
	new_rect = state_info(state)->ball->rect;

	TEST_CHECK( y == FALLING);
	TEST_CHECK( new_rect.y == old_rect.y + old_speed);
	TEST_CHECK( new_speed == old_speed + (float)(10*old_speed)/100);

	// Σε κατάσταση που η μπάλα πέφτει, η ταχύτητα δεν μπορεί να γίνει μεγαλύτερη από 7
	state_info(state)->ball->vert_speed = 7;
	state_update(state, &keys);
	float newsp = state_info(state)->ball->vert_speed;

	TEST_CHECK( newsp == 7);

	// Άμα το παιχνίδι τελειώσει, η κατάσταση δεν ενημερώνεται πλεον
	game_over(state);

	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;

	TEST_CHECK( new_rect.x == old_rect.x);

	// Αν έχει τελειώσει το παιχνίδι, με πατημένο το enter, η κατάσταση ενημερώνεται εκ νέου (χρειάζονται 2 frames)
	keys.enter = true;
	state_update(state, &keys);
	keys.enter = false;
	old_rect = state_info(state)->ball->rect;
	state_update(state, &keys);
	new_rect = state_info(state)->ball->rect;

	TEST_CHECK( new_rect.x == old_rect.x + 4);
}

void test_state_update_collisions_and_platforms(){
	State state = state_create();
	TEST_ASSERT(state != NULL && state_info(state) != NULL);

	List list = state_objects(state, 0, 2*SCREEN_WIDTH);
	Object firstplatform = list_node_value(list, list_last(list));
	if(firstplatform->type != PLATFORM){
		firstplatform->type = PLATFORM;
		firstplatform->rect.width = 30;
		firstplatform->rect.height = 30;
		firstplatform->vert_mov = IDLE;
		firstplatform->vert_speed = 0;
		firstplatform->unstable = false;
	}
	Object firststar = list_node_value(list, list_first(list));
	if(firststar->type != STAR){
		firststar->type = STAR;
		firststar->rect.width = 30;
		firststar->rect.height = 30;
		firststar->vert_mov = IDLE;
		firststar->vert_speed = 0;
		firststar->unstable = false;
	}
	

	// Πληροφορίες για τα πλήκτρα (αρχικά κανένα δεν είναι πατημένο)
	struct key_state keys = { false, false, false, false, false, false, false };
	
	Rectangle old_rect = firstplatform->rect;
	state_update(state, &keys);
	Rectangle new_rect = firstplatform->rect;
	TEST_ASSERT( old_rect.y == new_rect.y + firstplatform->vert_speed);

	firstplatform->vert_mov = FALLING;
	old_rect = firstplatform->rect;
	state_update(state, &keys);
	new_rect = firstplatform->rect;
	TEST_ASSERT( old_rect.y == new_rect.y - 4);

	state_update(state, &keys);
	new_rect = firstplatform->rect;
	TEST_ASSERT( old_rect.y == new_rect.y - 8);

	state_info(state)->ball->rect.y = SCREEN_HEIGHT + 1;
	state_update(state, &keys);
	TEST_ASSERT(!state_info(state)->playing && sounds_info(state)->gameover);

	state_update(state, &keys);
	TEST_ASSERT(!state_info(state)->playing && !sounds_info(state)->gameover);

	state_info(state)->ball->rect.y = firststar->rect.y - 1.5;
	state_info(state)->ball->rect.x = firststar->rect.x - 4;
	state_update(state, &keys);
	TEST_ASSERT(CheckCollisionRecs(state_info(state)->ball->rect, firststar->rect));
}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_state_create", test_state_create },
	{ "test_state_update", test_state_update },
	{ "test_state_update", test_state_update_collisions_and_platforms },
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};