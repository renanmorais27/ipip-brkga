from itertools import islice
from math import sqrt

# EU population by Kurz ON THE INVERSE POWER INDEX PROBLEM
countries = [
    {"name": "Germany", "pop": 82500},
    {"name": "United Kingdom", "pop": 60600},
    {"name": "France", "pop": 60000},
    {"name": "Italy", "pop": 58500},
    {"name": "Spain", "pop": 43000},
    {"name": "Poland", "pop": 38200},
    {"name": "Romania", "pop": 21700},
    {"name": "Netherlands", "pop": 16300},
    {"name": "Greece", "pop": 11100},
    {"name": "Czech Republic", "pop": 10500},
    {"name": "Belgium", "pop": 10400},
    {"name": "Hungary", "pop": 10200},
    {"name": "Portugal", "pop": 10100},
    {"name": "Sweden", "pop": 9000},
    {"name": "Austria", "pop": 8200},
    {"name": "Bulgaria", "pop": 7800},
    {"name": "Denmark", "pop": 5400},
    {"name": "Slovakia", "pop": 5400},
    {"name": "Finland", "pop": 5200},
    {"name": "Ireland", "pop": 4100},
    {"name": "Latvia", "pop": 3400},
    {"name": "Lithuania", "pop": 2300},
    {"name": "Slovenia", "pop": 2000},
    {"name": "Estonia", "pop": 1300},
    {"name": "Cyprus", "pop": 700},
    {"name": "Luxembourg", "pop": 500},
    {"name": "Malta", "pop": 400},
]

for n in range(2, 27 + 1):
    f = open(str(n), "w")
    popSum = sum(sqrt(c["pop"]) for c in islice(countries, 0, n))
    f.write(str(n) + "\n")

    for c in islice(countries, 0, n):
        pi = sqrt(c["pop"]) / popSum
        f.write(str(pi) + "\n")

    f.write("\n")
    for c in islice(countries, 0, n):
        f.write("# {} {}\n".format(c["name"], c["pop"]))
