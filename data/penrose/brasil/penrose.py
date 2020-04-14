from itertools import islice
from math import sqrt

# IBGE 1 de julho 2019 ftp://ftp.ibge.gov.br/Estimativas_de_Populacao/Estimativas_2019/estimativa_dou_2019.xls
states = [
    {"name": "São Paulo", "pop": 45919049},
    {"name": "Minas Gerais", "pop": 21168791},
    {"name": "Rio de Janeiro", "pop": 17264943},
    {"name": "Bahia", "pop": 14873064},
    {"name": "Paraná", "pop": 11433957},
    {"name": "Rio Grande do Sul", "pop": 11377239},
    {"name": "Pernambuco", "pop": 9557071},
    {"name": "Ceará", "pop": 9132078},
    {"name": "Pará", "pop": 8602865},
    {"name": "Santa Catarina", "pop": 7164788},
    {"name": "Maranhão", "pop": 7075181},
    {"name": "Goiás", "pop": 7018354},
    {"name": "Amazonas", "pop": 4144597},
    {"name": "Espírito Santo", "pop": 4018650},
    {"name": "Paraíba", "pop": 4018127},
    {"name": "Rio Grande do Norte", "pop": 3506853},
    {"name": "Mato Grosso", "pop": 3484466},
    {"name": "Alagoas", "pop": 3337357},
    {"name": "Piauí", "pop": 3273227},
    {"name": "Distrito Federal", "pop": 3015268},
    {"name": "Mato Grosso do Sul", "pop": 2778986},
    {"name": "Sergipe", "pop": 2298696},
    {"name": "Rondônia", "pop": 1777225},
    {"name": "Tocantins", "pop": 1572866},
    {"name": "Acre", "pop": 881935},
    {"name": "Amapá", "pop": 845731},
    {"name": "Roraima", "pop": 605761},
]

for n in range(2, 27 + 1):
    f = open(str(n), "w")
    popSum = sum(sqrt(c["pop"]) for c in islice(states, 0, n))
    f.write(str(n) + "\n")

    for c in islice(states, 0, n):
        pi = sqrt(c["pop"]) / popSum
        f.write(str(pi) + "\n")

    f.write("\n")
    for c in islice(states, 0, n):
        f.write("# {} {}\n".format(c["name"], c["pop"]))
