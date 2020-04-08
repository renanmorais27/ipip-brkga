from common import Player
import shapleyshubik

def get_power_index_list(args):
    quota, *weights = args
    # print(quota, [w for w in weights]) 
    players = []
    for i, w in enumerate(weights):
        players.append(Player(w, i))
    # print(quota, [p.weight for p in players]) 
    players = shapleyshubik.compute(quota, players)   
    # print(quota, [p.weight for p in players]) 
    vector = [quota, *[p.power_index for p in players]]
    # print(vector)
    return vector