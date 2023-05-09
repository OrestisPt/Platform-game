#pragma once

#include "state.h"

// Ήχοι που πρέπει να αναπαραχθούν σε κάθε κατάσταση
typedef struct state_sounds {
	bool gameover;					// true αν το παιχνίδι τελειωσε (ένα frame)
	bool jumping;				// true αν η μπαλα χοροπηδαει (ένα frame)
    bool gotStar;               // true αν η μπαλα πήρε αστέρι (ένα frame)
}* Statesounds;

// Επιστρέφει τον ήχο που πρέπει να αναπαραχθεί στην κατάσταση state.
// Οι υλοποιήσεις της συνάρτησης βρίσκονται στα state.c και state_alt.c

Statesounds sounds_info(State state);