"""The module with common functions for the existing power indices."""


class Player:
    def __init__(self, weight, key):
        self.weight = weight
        self.key = key
        self.power_index = 0


def read_instance(infile):
    players = []
    quota = float(infile.readline())
    for i, line in enumerate(infile.readlines()):
        weight, *name = line.strip().rsplit(' ', 1)[::-1]
        players.append(Player(float(weight), i+1 if not name else name[0]))
    infile.seek(0)
    return (quota, players)


def get_max_coalition(players, quota):
    players.sort(key=lambda x: x.weight)
    max_coalition = 0
    total_weight = 0
    for i, p in enumerate(players):
        total_weight += p.weight
        if total_weight >= quota:
            max_coalition = i + 1
            break
    return max_coalition


def coalition_sum(sum_repo, coalition):
    if not coalition:
        return 0
    sum_repo[coalition] = sum_repo[coalition[:-1]] + coalition[-1].weight
    return sum_repo[coalition]


def output(method_name, players):
    print('{} power indices'.format(method_name))
    for p in players:
        print('  {}: {}'.format(p.name, round(p.power_index, 4)))
