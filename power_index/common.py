"""The module with common functions for the existing power indices."""


class Player:
    def __init__(self, weight):
        self.weight = weight
        self.power_index = 0


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