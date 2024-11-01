TARGET				= tetris_game
LIBRARY 			= s21_tetris.a
FUNC_FILES 			= brick_game/tetris/*.c
FRONT_FILES 		= gui/cli/*.c
OBJ					= *.o
FUNC_TEST			= tests/*.check
REPORT_FILE 		= report/index.html
VALGRIND_FILE 		= valgrind_errors.txt

UNAME_S := $(shell uname -s)

CC			= gcc
CFLAGS		= -c -std=c11 -Wall -Werror -Wextra
L_FLAGS		= -std=c11 -Wall -Werror -Wextra
COVER_FLAG  = --coverage
LIB_FLAGS 	= ar rcs

ifeq ($(UNAME_S), Linux)
	OS_SPECIFIC_FLAGS 				= -lcheck -lsubunit -lm -lncurses
	OS_SPECIFIC_GCOV_REPORT 		= lcov_branch_coverage=1
	OS_SPECIFIC_GCOV_REPORT_HTML 	= genhtml_branch_coverage=1
else ifeq ($(UNAME_S), Darwin)
	OS_SPECIFIC_FLAGS 				= -lcheck -lncurses
    OS_SPECIFIC_GCOV_REPORT 		= branch_coverage=1
    OS_SPECIFIC_GCOV_REPORT_HTML 	= branch_coverage=1
endif

all: s21_tetris.a

s21_tetris.a:
	gcc -c brick_game/tetris/try.c -lncurses
	ar rcs s21_brick.a try.o

install: s21_tetris.a
	gcc gui/cli/front.c -L. s21_brick.a -lcheck -lncurses -o game

uninstall:
	make clean
	rm -rf game score.txt

dist:
	mkdir -p dist
	cp -r brick_game gui tests Makefile tests dist/
	tar -czf Brick_dist.tar.gz dist
	rm -rf dist

test:
	checkmk clean_mode=1 tests/*.check > test.c
	gcc -c --coverage brick_game/tetris/try.c -lncurses
	ar rcs s21_brick.a try.o
	gcc test.c --coverage -L. s21_brick.a $(OS_SPECIFIC_FLAGS) -o test
	./test
	#make gcov_report
	#open report/index.html

gcov_report: test
	lcov -t "test" -o brick.info -c -d . --rc branch_coverage=1
	genhtml -o report brick.info --rc branch_coverage=1

dvi:
	@echo "School 21 Brick Game"

test_file:
	checkmk clean_mode=1 tests/*.check > test.c
	gcc test.c -L . libs21_brick.a $(OS_SPECIFIC_FLAGS) -o test

clean:
	rm -rf $(OBJ) $(LIBRARY) *.a *.gc* test.c *.info report test dist *.tar.gz

rebuild:
	clean all