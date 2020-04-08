#include <iomanip>
#include <iostream>

#include "brkgaAPI/BRKGA.h"
#include "brkgaAPI/MTRand.h"
#include "IPIPDecoder.h"

int main(int argc, char* argv[]) {
    const clock_t begin = clock();

    const unsigned n = 6;      // size of chromosomes
    const unsigned p = 256;    // size of population
    const double pe = 0.20;    // fraction of population to be the elite-set
    const double pm = 0.10;    // fraction of population to be replaced by mutants
    const double rhoe = 0.70;  // probability that offspring inherit an allele from elite parent
    const unsigned K = 3;      // number of independent populations
    const unsigned MAXT = 1;   // number of threads for parallel decoding

    IPIPDecoder decoder;  // initialize the decoder

    const long unsigned rngSeed = 0;  // seed to the random number generator
    MTRand rng(rngSeed);              // initialize the random number generator

    // initialize the BRKGA-based heuristic
    BRKGA<IPIPDecoder, MTRand> algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);

    unsigned generation = 0;        // current generation
    const unsigned X_INTVL = 100;   // exchange best individuals at every 100 generations
    const unsigned X_NUMBER = 2;    // exchange top 2 best
    const unsigned MAX_GENS = 100;  // run for 1000 gens

    do {
        const clock_t beginGen = clock();
        algorithm.evolve();  // evolve the population for one generation

        if ((++generation) % X_INTVL == 0) {
            algorithm.exchangeElite(X_NUMBER);  // exchange top individuals
        }

        const clock_t endGen = clock();

        std::cout << std::setw(3) << generation << std::fixed;
        std::cout << std::setw(14) << std::setprecision(7) << algorithm.getBestFitness();
        std::cout << std::setw(10) << std::setprecision(2) << ((endGen - beginGen) / double(CLOCKS_PER_SEC)) << " s";
        std::cout << std::setw(10) << std::setprecision(2) << ((endGen - begin) / double(CLOCKS_PER_SEC)) << " s" << std::endl;
    } while (generation < MAX_GENS && algorithm.getBestFitness() >= 0.00001);

    // print the fitness of the top 10 individuals of each population:
    std::cout << "Fitness of the top 10 individuals of each population:" << std::endl;
    const unsigned bound = std::min(p, unsigned(10));  // makes sure we have 10 individuals
    for (unsigned i = 0; i < K; ++i) {
        std::cout << "Population #" << i << ":" << std::endl;
        for (unsigned j = 0; j < bound; ++j) {
            std::cout << "\t" << j << ") " << std::setprecision(7) 
                << algorithm.getPopulation(i).getFitness(j);
            // double sum = 0.0;
            // for (unsigned k = 1; k < n; k++) {
            //     sum += algorithm.getPopulation(i).getChromosome(j)[k];
            // }
            // std::cout << " | [" << std::setprecision(5) << "1" << " ; ";
            // std::cout << " | [" << std::setprecision(5) 
            //     << algorithm.getPopulation(i).getChromosome(j)[0] << " ; ";
            std::cout << " | [" << std::setprecision(5) 
                << (n-1)*(algorithm.getPopulation(i).getChromosome(j)[0]*algorithm.getPopulation(i).getChromosome(j)[0]) << " ; ";

            for (unsigned k = 1; k < n; k++) {
                // std::cout << algorithm.getPopulation(i).getChromosome(j)[k] / sum;
                std::cout << algorithm.getPopulation(i).getChromosome(j)[k];
                if (k != n - 1) std::cout << " , ";
            }
            std::cout << "]" << std::endl;
        }
    }

    std::cout << "Best solution found has objective value = "
              << algorithm.getBestFitness() << std::endl;

    const clock_t end = clock();
    std::cout << "BRKGA run finished in " << (end - begin) / double(CLOCKS_PER_SEC) << " s." << std::endl;

    return 0;
}
