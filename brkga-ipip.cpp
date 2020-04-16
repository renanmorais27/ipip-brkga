#include <iomanip>
#include <iostream>

#include "brkgaAPI/BRKGA.h"
#include "brkgaAPI/MTRand.h"
#include "IPIPDecoder.h"
#include "IPIPInstance.h"

void printResult(std::vector< double > chromosome, unsigned n);
void printTime(unsigned generation, double bestFitness, double genTime, double totalTime);

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
    const unsigned p = 10;    // size of population
    const double pe = 0.20;    // fraction of population to be the elite-set
    const double pm = 0.10;    // fraction of population to be replaced by mutants
    const double rhoe = 0.70;  // probability that offspring inherit an allele from elite parent
    const unsigned K = 3;      // number of independent populations
    const unsigned MAXT = 10;   // number of threads for parallel decoding

    const clock_t beginGen = clock();
    // initialize the BRKGA-based heuristic
    BRKGA<IPIPDecoder, MTRand> algorithm(n, p, pe, pm, rhoe, decoder, rng, K, MAXT);
    const clock_t endGen = clock();

    const unsigned X_INTVL = 25;   // exchange best individuals at every 100 generations
    const unsigned X_NUMBER = 2;    // exchange top 2 best
    const unsigned MAX_GENS = 350;  // run for 1000 gens

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

    unsigned generation = 1;        // current generation
    double bestFitness = algorithm.getBestFitness();

    printTime(generation, algorithm.getBestFitness(), endGen - beginGen, endGen - begin);
    printResult(algorithm.getPopulation(0).getChromosome(0), n);

    do {
        const clock_t beginGen = clock();
        algorithm.evolve();  // evolve the population for one generation

        if ((++generation) % X_INTVL == 0) {
            algorithm.exchangeElite(X_NUMBER);  // exchange top individuals
        }

        const clock_t endGen = clock();

        if((generation % (MAX_GENS / 20)) == 0){
            // std::cout << "---------------------------------------------------" << std::endl;
            printTime(generation, algorithm.getBestFitness(), endGen - beginGen, endGen - begin);
            std::cout <<  std::endl;
            // printResult(algorithm.getBestChromosome(), n);
        }
        else if(algorithm.getBestFitness() < bestFitness){
            bestFitness = algorithm.getBestFitness();
            printTime(generation, algorithm.getBestFitness(), endGen - beginGen, endGen - begin);
            printResult(algorithm.getBestChromosome(), n);
        }

    } while (generation < MAX_GENS && algorithm.getBestFitness() >= 0.00001);

    // print the fitness of the top 10 individuals of each population:
    std::cout << "\nFitness of the top 10 individuals of each population:" << std::endl;
    const unsigned bound = std::min(p, unsigned(10));  // makes sure we have 10 individuals
    for (unsigned i = 0; i < K; ++i) {
        std::cout << "Population #" << i << ":" << std::endl;
        for (unsigned j = 0; j < bound; ++j) {
            std::cout << "\t" << j << ") " << std::setprecision(7) 
                << algorithm.getPopulation(i).getFitness(j);
            printResult(algorithm.getPopulation(i).getChromosome(j), n);
        }
    }

    std::cout << "Best solution found has objective value = "
              << algorithm.getBestFitness() << std::endl;

    const clock_t end = clock();
    std::cout << "BRKGA run finished in " << (end - begin) / double(CLOCKS_PER_SEC) << " s." << std::endl;

    return 0;
}

void printResult(std::vector< double > chromosome, unsigned n){
    double sum = 0.0;
    for (unsigned k = 1; k < n; k++) {
        sum += chromosome[k];
    }
    std::cout << " | [" << std::setprecision(5) 
        // << (n-1)*(chromosome[0]*chromosome[0]) << " ; ";
        // << ( sum * chromosome[0]) << " ; ";
        // << ( sum * (0.5 + (pow(2 * (chromosome[0] - 0.5), 3)) / 2)) << " ; ";
        << ( sum * 0.5 ) << " ; ";

    for (unsigned k = 1; k < n; k++) {
        std::cout << chromosome[k];
        if (k != n - 1) std::cout << " , ";
    }
    std::cout << "]" << std::endl;
}

void printTime(unsigned generation, double bestFitness, double genTime, double totalTime){
    std::cout << std::setw(3) << generation << std::fixed;
    std::cout << std::setw(14) << std::setprecision(7) << bestFitness;
    std::cout << std::setw(10) << std::setprecision(2) << (genTime / double(CLOCKS_PER_SEC)) << " s";
    std::cout << std::setw(10) << std::setprecision(2) << (totalTime / double(CLOCKS_PER_SEC)) << " s";
}