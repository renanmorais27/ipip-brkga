"""The module to compute the Shapley-Shubik power index."""

import math
import itertools
from common import get_max_coalition


def compute(quota, players):
    players_len = len(players)
    facts = [math.factorial(i) for i in range(players_len + 1)]

    max_coalition = get_max_coalition(players, quota)

    players.sort(key=lambda x: x.weight, reverse=True)
    for n in range(0, max_coalition + 1):
        coalitions = itertools.combinations(players, n)
        for c in coalitions:
            c_weight = sum(p.weight for p in c)
            if c_weight < quota:
                for p in players:
                    if p not in c:
                        if c_weight + p.weight >= quota:
                            c_size = len(c)
                            p.power_index += (facts[c_size] *
                                              facts[players_len - c_size - 1])
                        else:
                            break
    for p in players:
        p.power_index /= facts[players_len]

    return players
