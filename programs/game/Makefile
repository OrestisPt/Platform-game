# paths
LIB = ../../lib
INCLUDE = ../../include
MODULES = ../../modules

# compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall -Werror -g -I$(INCLUDE)
LDFLAGS = -lm

# Αρχεία .o
OBJS1 = game.o $(MODULES)/state.o $(MODULES)/interface.o $(LIB)/k08.a $(LIB)/libraylib.a
OBJS2 = game.o $(MODULES)/set_utils.o $(MODULES)/state_alt.o $(MODULES)/interface.o $(LIB)/k08.a $(LIB)/libraylib.a

# Το εκτελέσιμο πρόγραμμα
EXEC1 = game
EXEC2 = game_alt

# Παράμετροι για δοκιμαστική εκτέλεση
ARGS =

# Παράμετροι της βιβλιοθήκης raylib
include $(LIB)/libraylib.mk

all: $(EXEC1) $(EXEC2)

$(EXEC1): $(OBJS1)
	$(CC) $(OBJS1) -o $(EXEC1) $(LDFLAGS)

$(EXEC2): $(OBJS2)
	$(CC) $(OBJS2) -o $(EXEC2) $(LDFLAGS)

# Για να φτιάξουμε τα k08.a/libraylib.a τρέχουμε το make στο lib directory.
$(LIB)/%.a:
	$(MAKE) -C $(LIB) $*.a

clean:
	rm -f $(OBJS1) $(EXEC1)
	rm -f $(OBJS2) $(EXEC2)

run: $(EXEC1)
	./$(EXEC1) $(ARGS)

run-alt: $(EXEC2)
	./$(EXEC2) $(ARGS)

valgrind: $(EXEC1)
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$(EXEC1) $(ARGS)

valgrind-alt: $(EXEC2)
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$(EXEC2) $(ARGS)