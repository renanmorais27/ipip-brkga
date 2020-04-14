from common import Player
import shapleyshubik, banzhaf

def get_power_index_list(args):
    quota, *weights = args
    players = []
    for w in weights:
        players.append(Player(w))

    players = banzhaf.compute(quota, players)

    vector = [quota, *[p.power_index for p in players]]
    return vector
