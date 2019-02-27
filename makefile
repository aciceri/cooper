PARAMS = -std=c99 -Wall -g
leak-check = yes
track-origins = yes
#wff = "(and (= (+ (* -2 x) (* 2 a) (* 3 b) (* 3 c)) 3) (> (+ (* 5 x) (* 3 c)) 1) (div (+ (* 2 x) (* 2 y)) 1))"
wff = "(and (= (+ (* 2 x) (* 1 y)) 4))"
vars = "x y a b c" #variables
var = "x" #to eliminate

test: test.c cooper.o
	gcc $(PARAMS) test.c cooper.o -o test

cooper.o: cooper.c cooper.h
	gcc $(PARAMS) -c cooper.c -o cooper.o

run: test
	time ./test $(wff) $(var)

sat: test sat.py
	./sat.py $(wff) $(vars)

valgrind: test
	valgrind --track-origins=$(track-origins) --leak-check=$(leak-check) ./test $(wff) $(var)

debug: test
	gdb --args test $(wff) $(var)

eval: test
	./eval.scm "`./test $(wff) $(var) | tail -n 1`"

clean:
	rm -f *.o
	rm test

