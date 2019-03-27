SHELL := /bin/bash
PARAMS = -std=c99 -Wall -g #compila nello standard C99 e abilita tutti i warning
leak-check = yes #valgrind effettua una ricerca dei leak più accurata
track-origins = yes #valgrind fornisce più informazioni
wff = "(and (= (+ (* -2 x) (* 3 y)) 3) \
	    (> (+ (* 5 x) (* 3 y)) 1) \
            (div (+ (* 2 x) (* 4 y)) 1))" #formula in ingresso
wff = "(and (div (+ (* 4 x) (* 3 y)) 2) (= (+ (* 2 x)) 4))"
vars = "x y" #variabili presenti nella formula
var = "x" #variabile da eliminare

test: test.c cooper.o
	gcc $(PARAMS) test.c cooper.o -o test

test2: test2.c cooper.o
	gcc $(PARAMS) test2.c cooper.o -o test2

test3: test3.c cooper.o
	gcc $(PARAMS) test3.c cooper.o -o test3

cooper.o: cooper.c cooper.h
	gcc $(PARAMS) -c cooper.c -o cooper.o

run: test #esegue test e restituisce il tempo impiegato
	@echo -e 'Elimino la variabile $(var) dalla seguente formula:\n$(wff) ---> \n'
	@time ./test $(wff) $(var)

run2: test2
	@time ./test2 $(wff) $(var)

run3: test3
	time ./test3 $(wff) $(vars)

sat: test sat.py #verifica la soddisfacibilità della formula generata grazie a yices
	./sat.py $(wff) $(vars) $(var)

valgrind: test 
	valgrind --track-origins=$(track-origins) \
		 --leak-check=$(leak-check) ./test $(wff) $(var)

debug: test #esegue test col debugger gdb
	gdb --args test $(wff) $(var)

eval: test3 #valuta il valore della formula equivalente,
	   #funziona solo se ogni variabile è già stata eliminata
	./eval.scm "`./test3 $(wff) $(vars) | tail -n 1`"

clean:
	rm -f *.o
	rm -f test test2

