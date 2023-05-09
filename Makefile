# Το Makefile αυτό βρίσκεται στο root ολόκληρου του project και χρησιμεύει για
# να κάνουμε εύκολα compile πολλά τμήματα του project μαζί. Το Makefile αυτό
# καλεί το make στα διάφορα directories ως
#   $(MAKE) -C <dir> <target>
# το οποίο είναι ισοδύναμο με το να τρέξουμε make <target> μέσα στο directory <foo>

# Ολα τα directories μέσα στο programs directory
PROGRAMS = $(subst programs/, , $(wildcard programs/*))
# Ολα τα directories μέσα στο tests directory
TESTS = $(subst tests/, , $(wildcard tests/*))

# Compile: όλα, προγράμματα, tests
all: programs tests

# Η παρακάτω γραμμή δημιουργεί ένα target programs-<foo> για οποιοδήποτε <foo>. Η μεταβλητή $* περιέχει το "foo"
programs-%:
	$(MAKE) -C programs/$*

programs: $(addprefix programs-, $(PROGRAMS))		# depend στο programs-<foo> για κάθε στοιχείο του PROGRAMS

# Η παρακάτω γραμμή δημιουργεί ένα target tests-<foo> για οποιοδήποτε <foo>. Η μεταβλητή $* περιέχει το "foo"
tests-%:
	$(MAKE) -C tests/$*

tests: $(addprefix tests-, $(TESTS))		# depend στο test-<foo> για κάθε στοιχείο του TESTS


# Εκτέλεση: όλα, προγράμματα, tests
run: run-tests run-programs

run-programs-%:
	$(MAKE) -C programs/$* run

run-programs: $(addprefix run-programs-, $(PROGRAMS))

run-tests-%:
	$(MAKE) -C tests/$* run

run-tests: $(addprefix run-tests-, $(TESTS))
# Εκκαθάριση
clean-programs-%:
	$(MAKE) -C programs/$* clean

clean-tests-%:
	$(MAKE) -C tests/$* clean

clean: $(addprefix clean-programs-, $(PROGRAMS)) $(addprefix clean-tests-, $(TESTS))
	$(MAKE) -C lib clean

# Δηλώνουμε ότι οι παρακάτω κανόνες είναι εικονικοί, δεν παράγουν αρχεία. Θέλουμε δηλαδή
# το "make programs" να εκτελεστεί παρόλο που υπάρχει ήδη ένα directory "programs".
#
.PHONY: programs tests lib run run-programs run-tests clean