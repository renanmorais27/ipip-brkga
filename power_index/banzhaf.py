"""The module to compute the Banzhaf power index."""

import itertools
from common import get_max_coalition


def compute(quota, players):
    max_coalition = get_max_coalition(players, quota)

    players.sort(key=lambda x: x.weight, reverse=True)
    for n in range(0, max_coalition + 1):
        coalitions = itertools.combinations(players, n)
        for c in coalitions:
            c_weight = sum(p.weight for p in c)
            if c_weight >= quota:
                for p in c:
                    if c_weight - p.weight < quota:
                        p.power_index += 1
                    else:
                        break
    total = sum(p.power_index for p in players)
    for p in players:
        p.power_index /= total

    return players
