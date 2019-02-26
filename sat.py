#!/bin/python3
from sys import argv
from subprocess import run


def main():
    if len(argv) != 3:
        print("Wrong arguments number!")
    else:
        wff = argv[1]
        variables = argv[2].split()
        yices = ""

        for var in variables:
            yices += "(define {}::int)\n".format(var)

        wff_out = run(["./test", wff, variables[0]], capture_output=True).stdout.decode()
        yices += "(assert {})\n(check)".format(wff_out)

        with open("source.ys", "w") as source:
            print(yices, file=source)
            
        run(["yices", "source.ys"])


if __name__ == '__main__':
    main()
