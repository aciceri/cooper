SHELL := /bin/bash
PARAMS = -std=c99 -Wall -g #compila nello standard C99 e abilita tutti i warning
leak-check = yes #valgrind effettua una ricerca dei leak più accurata
track-origins = yes #valgrind fornisce più informazioni
wff = "(and (= (+ (* -2 x) (* 3 y)) 3) \
	    (> (+ (* 5 x) (* 3 y)) 1) \
            (div (+ (* 2 x) (* 4 y)) 1))" #formula in ingresso
wff = "(and (div (+ (* 3 z)) 3) (= (+ (* 2 y) (* 3 x)) 2) (= (+ (* 2 x)) 4))"
wff= "(and (> (+ (* 1 x)) 5) (> (+ (* -1 x) (* 1 y)) 0))"
#wff="(and (= (+ (* 2 a) (* 3 b) (* 4 c)) 3) (> (+ (* 3 x) (* 2 y)) 1) (= (+ (* 2 x) (* 4 y)) 3) (> (+ (* -12 x) (* 3 y)) 4))"
#wff="(and (= (+ (* 2 x) (* -1 y)) 3) (= (+ (* 4 a) (* 1 y)) 1) (> (+ (* -2 x) (* 1 y)) -10) (= (+ (* 3 a)) 6))"
vars = "x y" #variabili presenti nella formula
var = "x" #variabile da eliminare

test: test.c cooper.o
	gcc $(PARAMS) test.c cooper.o -o test

test2: test2.c cooper.o
	gcc $(PARAMS) test2.c cooper.o -o test2

cooper.o: cooper.c cooper.h
	gcc $(PARAMS) -c cooper.c -o cooper.o

run: test #esegue test e restituisce il tempo impiegato
	@echo -e 'Elimino la variabile $(var) dalla seguente formula:\n$(wff) ---> \n'
	@time ./test $(wff) $(var)

run2: test2
	@time ./test2 $(wff) $(var)

eq: test eq.py #prende la formula e la variabile da eliminare, la elimina controlla con z3 se il risultato è equivalente a $(guess)
	@./eq.py "$(formula)" "$(variables)" "$(guess)" #la prima in variables è quella da eliminare

valgrind: test
	valgrind --track-origins=$(track-origins) \
		 --leak-check=$(leak-check) ./test $(wff) $(var)

valgrind2: test2
	valgrind --track-origins=$(track-origins) \
		 --leak-check=$(leak-check) ./test2 $(wff) $(var)

debug: test #esegue test col debugger gdb
	gdb --args test $(wff) $(var)

eval: test3 #valuta il valore della formula equivalente,
	   #funziona solo se ogni variabile è già stata eliminata
	./eval.scm "`./test3 $(wff) $(vars) | tail -n 1`"

clean:
	rm -f *.o
	rm -f test test2
	rm -f eq.py
