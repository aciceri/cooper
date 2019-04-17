#!/bin/python3
from sys import argv
from subprocess import run


def main():
    if len(argv) != 4:
        print("Wrong arguments number!")
    else:
        formula = argv[1]
        variables = argv[2].split() #tutte le variabili
        var = variables[0] #la prima è quella da eliminare
        guess = argv[3]
        smt_source = ""

        for v in variables:
            if v is not var:
                smt_source += "(declare-const {} Int)\n".format(v)

        wff_out = run(["./test", formula, var],
                      capture_output=True).stdout.decode()
        smt_source += "(assert (not (= {} {})))\n".format(wff_out, guess)
        smt_source += "(check-sat)\n"

        with open("eq.smt", "w") as source:
            print(smt_source, file=source)

        result = run(["z3", "eq.smt"], capture_output=True).stdout.decode()

        if "unsat" in result:
            print("Sono equivalenti")
        elif "error" in result:
            print("Errore di z3:\n{}".format(result))
        elif "sat" in result:
            print("Non sono equivalenti")


if __name__ == '__main__':
    main()
