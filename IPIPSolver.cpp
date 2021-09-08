/*
 * TSPSolver.cpp
 *
 *  Created on: Mar 16, 2013
 *      Author: Rodrigo
 */

#include "IPIPSolver.h"

#include <python3.7m/Python.h>

#include <iostream>
#include <numeric>

#include "cppitertools/combinations.hpp"

class Player {
   public:
    Player(double w) : weight(w), powerIndex(0.0){};
    // Player(){};
    double weight;
    double powerIndex;

    bool operator<(const Player& player) const {
        return (weight < player.weight);
    }

    bool operator>(const Player& player) const {
        return (weight > player.weight);
    }
    bool operator==(const Player& p) const {
        return this == &p;
    }
};

double taxicabMDE();
double taxicabDE(double desired, double found);
double quadraticMDE();
double quadraticDE(double desired, double found);
double quotaQuadraticToPlayers(std::vector<double> chromosome);
double quotaLinearToWeights(std::vector<double> chromosome);
double quotaCubicToWeights(std::vector<double> chromosome);
std::vector<double> banzhaf(double quota, std::vector<double> weights);
std::vector<double> shapleyshubik(double quota, std::vector<double> weights);

IPIPSolver::IPIPSolver(const IPIPInstance& instance, const std::vector<double>& chromosome) : deltaError(0.0) {
    unsigned chromosomeSize = chromosome.size();

    // Parameters required to minimize
    // Error function
    //      Taxicab -> |d - p|
    //      Quadratic -> (d - p) * (d - p)
    double (*calcMaxDeltaError)() = taxicabMDE;
    double (*calcDeltaError)(double, double) = taxicabDE;
    // Quota calc
    //      1 -> n * genome(q)^2
    //      2 -> sum(genome(w)) * genome(q)
    //      3 -> sum(genome(w)) * [1/2 + ((2 * (genome(q) - 1/2))^3) / 2]
    double (*calcQuota)(std::vector<double>) = quotaLinearToWeights;

    // The chromosome is composed by the genomes corresponding to the quota (first position)
    // and the weights (second to last positions)

    // 1) Generate quota using the method given
    double quota = calcQuota(chromosome);

    // 2) Compute the sum of weights; if it is less than the quota the game is not defined
    double sum = std::accumulate(chromosome.begin() + 1, chromosome.end(), 0.0);
    if (sum < quota) {
        deltaError = calcMaxDeltaError();
        return;
    }

    // 3) The weights are the chromosome genomes
    std::vector<double> weights(chromosomeSize - 1);
    for (unsigned i = 1; i < chromosomeSize; i++) {
        weights[i - 1] = chromosome[i];
    }

    // 4) Compute the power indices using banzhaf or shapley-shubik
    std::vector<double> powerIndicesFound = banzhaf(quota, weights);

    // 5) Compute the error between the desired and found power indices (delta)
    std::vector<double> powerIndicesDesired = instance.getPowerIndices();
    for (unsigned i = 0; i < chromosomeSize - 1; i++) {
        deltaError += calcDeltaError(powerIndicesDesired[i], powerIndicesFound[i]);
    }
}

IPIPSolver::~IPIPSolver() {}

double IPIPSolver::getDeltaError() const { return deltaError; }

// Delta error is the maximum error defined for |d - p|, plus 1
double taxicabMDE() {
    return 3.0;
}

// Delta error is |d - p|
double taxicabDE(double desired, double found) {
    return abs(desired - found);
}

// Delta error is the maximum error defined for (d - p)^2, plus 1
double quadraticMDE() {
    return 5.0;
}

// Delta error is (d - p)^2
double quadraticDE(double desired, double found) {
    return (desired - found) * (desired - found);
}

// linear to weights / 2
// quadratic to weights
// cubic to players

// Quota is the number of players multiplied by the squared quota genome
// Range: [0, n]
double quotaQuadraticToPlayers(std::vector<double> chromosome) {
    return (chromosome.size() - 1) * (chromosome[0] * chromosome[0]);
}

// Quota is the sum of weight's genomes multiplied by the quota genome
// Range: [0, sum(w)]
double quotaLinearToWeights(std::vector<double> chromosome) {
    double sum = std::accumulate(chromosome.begin() + 1, chromosome.end(), 0.0);
    return sum * chromosome[0];
}

// Quota is the sum of weight's genomes multiplied by [1/2 + ((2 * (genome(q) - 1/2))^3) / 2]
// Range: [0, sum(w)]
double quotaCubicToWeights(std::vector<double> chromosome) {
    double sum = std::accumulate(chromosome.begin() + 1, chromosome.end(), 0.0);
    return sum * (0.5 + (pow(2 * (chromosome[0] - 0.5), 3)) / 2);
}

std::vector<double> banzhaf(double quota, std::vector<double> weights) {
    std::vector<Player> players;
    for (auto&& w : weights) {
        players.push_back(Player(w));
    }

    std::sort(players.begin(), players.end());
    int maxCoalition = 0;
    double totalWeight = 0;
    for (int i = 0; i < players.size(); i++) {
        totalWeight += players[i].weight;
        if (totalWeight >= quota) {
            maxCoalition = i + 1;
            break;
        }
    }

    std::sort(players.begin(), players.end(), std::greater<>());

    for (int n = 0; n <= maxCoalition; n++) {
        for (auto&& coalition : iter::combinations(players, n)) {
            double cWeight = std::accumulate(coalition.begin(),
                                             coalition.end(),
                                             0.0,
                                             [](const double& a, Player& b) { return a + b.weight; });
            if (cWeight >= quota) {
                for (auto&& player : coalition) {
                    if (cWeight - player.weight < quota)
                        player.powerIndex += 1;
                    else
                        break;
                }
            }
        }
    }
    double total = std::accumulate(players.begin(),
                                   players.end(),
                                   0.0,
                                   [](const double& a, Player& b) { return a + b.powerIndex; });
    for (auto&& p : players) {
        p.powerIndex /= total;
    }
    std::vector<double> powerIndices;
    for (auto&& p : players) {
        powerIndices.push_back(p.powerIndex);
    }
    return powerIndices;
}

std::vector<double> shapleyshubik(double quota, std::vector<double> weights) {
    std::vector<Player> players;
    for (auto&& w : weights) {
        players.push_back(Player(w));
    }

    unsigned nPlayers = players.size();
    std::sort(players.begin(), players.end(), std::greater<>());
    
    int maxCoalition = 0;
    double totalWeight = 0;
    for (int i = 0; i < nPlayers; i++) {
        totalWeight += players[nPlayers-1-i].weight;
        if (totalWeight >= quota) {
            maxCoalition = i + 1;
            break;
        }
    }

    int minCoalition = 0;
    totalWeight = 0;
    for (int i = 0; i < nPlayers; i++) {
        totalWeight += players[i].weight;
        if (totalWeight >= quota) {
            minCoalition = i;
            break;
        }
    }

    std::vector<double> factsCalc;
    unsigned long long r;
    unsigned np;
    for (unsigned i = 0; i < nPlayers; i++) {
        r = 1;
        np = nPlayers;
        for (unsigned d = 1; d <= i; d++) {
            r *= np--;
            r /= d;
        }
        r *= np;
        factsCalc.push_back(1.0/r);
    }

    for (int n = minCoalition; n <= maxCoalition; n++) {
        for (auto&& coalition : iter::combinations(players, n)) {
            double cWeight = std::accumulate(coalition.begin(),
                                             coalition.end(),
                                             0.0,
                                             [](const double& a, Player& b) { return a + b.weight; });
            if (cWeight >= quota) {
                for (auto&& player : coalition) {
                    if (cWeight - player.weight < quota)
                        player.powerIndex += factsCalc[coalition.size() - 1];
                    else
                        break;
                }
            }
        }
    }
    std::vector<double> powerIndices;
    for (auto&& p : players) {
        powerIndices.push_back(p.powerIndex);
    }
    return powerIndices;
}
// if(std::find(coalition.begin(), coalition.end(), std::as_const(player)) == coalition.end())