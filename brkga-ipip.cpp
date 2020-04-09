#include <iomanip>
#include <iostream>

#include "brkgaAPI/BRKGA.h"
#include "brkgaAPI/MTRand.h"
#include "IPIPDecoder.h"
#include "IPIPInstance.h"

int main(int argc, char* argv[]) {
	if(argc < 2) { std::cerr << "usage: <DATA-file>" << std::endl; return -1; }

	std::cout << "Welcome to the BRKGA API.\nFinding a (heuristic) minimizer for "
			<< " the Inverse Power Index Problem." << std::endl;
            
    const clock_t begin = clock();

    const std::string instanceFile = std::string(argv[1]);
	std::cout << "Instance file: " << instanceFile << std::endl;

	// Read the instance:
	IPIPInstance instance(instanceFile);
    std::cout << "\nInstance read! The power indices given were:" << std::endl;
    std::vector< double > powerIndices = instance.getPowerIndices();
    std::cout << "[ ";
	for(unsigned i = 0; i < powerIndices.size(); i++){
        std::cout << powerIndices[i];
        if(i != powerIndices.size() - 1) std::cout << ", ";
    }
    std::cout << " ]\n" << std::endl;

    IPIPDecoder decoder(instance);  // initialize the decoder

    const long unsigned rngSeed = 0;  // seed to the random number generator
    MTRand rng(rngSeed);              // initialize the random number generator

    const unsigned n = powerIndices.size() + 1;      // size of chromosomes
    const unsigned p = 100;    // size of population
    const double pe = 0.20;    // fraction of population to be the elite-set
    const double pm = 0.10;    // fraction of population to be replaced by mutants
    const double rhoe = 0.70;  // probability that offspring inherit an allele from elite parent
    const unsigned K = 3;      // number of independent populations
    const unsigned MAXT = 1;   // number of threads for parallel decoding

    // initialize the BRKGA-based heuristic
    BRKGA<IPIPDecoder, MTRand> algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);

    const unsigned X_INTVL = 100;   // exchange best individuals at every 100 generations
    const unsigned X_NUMBER = 2;    // exchange top 2 best
    const unsigned MAX_GENS = 100;  // run for 1000 gens

    std::cout << "Running algorithm with:" << std::endl;
    std::cout << "\t Size of chromosomes: " << n << std::endl;
    std::cout << "\t Size of population: " << p << std::endl;
    std::cout << "\t Fraction of pop to be elite set: " << pe << std::endl;
    std::cout << "\t Fraction of pop to replace with mutants: " << pm << std::endl;
    std::cout << "\t Allele inherit probability: " << rhoe << std::endl;
    std::cout << "\t Independent populations: " << K << std::endl;
    std::cout << "\t Exchange best individuals in " << X_INTVL << "th generation" << std::endl;
    std::cout << "\t Exchange top " << X_NUMBER << " best\n" << std::endl;

	// Print info about multi-threading:
	#ifdef _OPENMP
		std::cout << "Running for " << MAX_GENS << " generations using " << MAXT
				<< " out of " << omp_get_max_threads()
				<< " available thread units...\n" << std::endl;
	#endif
	#ifndef _OPENMP
		std::cout << "Running for " << MAX_GENS
				<< " generations without multi-threading...\n" << std::endl;
	#endif

    unsigned generation = 0;        // current generation
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
    std::cout << "\nFitness of the top 10 individuals of each population:" << std::endl;
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
