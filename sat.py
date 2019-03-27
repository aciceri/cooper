#!/bin/python3
from sys import argv
from subprocess import run


def main():
    if len(argv) != 4:
        print("Wrong arguments number!")
    else:
        wff = argv[1]
        variables = argv[2].split()
        var = argv[3]
        yices = ""

        for v in variables:
            if v is not var:
                yices += "(define {}::int)\n".format(v)

        wff_out = run(["./test", wff, var],
                      capture_output=True).stdout.decode()
        yices += "(assert {})\n".format(wff_out)

        with open("source.ys", "w") as source:
            print(yices, file=source)

        run(["yices", "source.ys"])


if __name__ == '__main__':
    main()
