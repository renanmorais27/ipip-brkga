/*
 * TSPInstance.h
 *
 * Reads an instance from TSPLIB (Symmetric TSP).
 *
 * Here's the URL: http://www.iwr.uni-heidelberg.de/groups/comopt/software/TSPLIB95/tsp/
 *
 * Here's the format:
 *
 * NAME : a280
 * COMMENT : drilling problem (Ludwig)
 * TYPE : TSP
 * DIMENSION: 280
 * EDGE_WEIGHT_TYPE : EUC_2D
 * NODE_COORD_SECTION
 * 1 288 149
 * 2 288 129
 * 3 270 133
 * 4 256 141
 * ...
 * EOF
 *
 *  Created on: Mar 16, 2013
 *      Author: Rodrigo
 */

#ifndef IPIPINSTANCE_H
#define IPIPINSTANCE_H

#include <cmath>
#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <stdexcept>

class IPIPInstance {
public:
	typedef std::runtime_error Error;

	IPIPInstance(const std::string& instanceFile);
	virtual ~IPIPInstance();

	// Getters:
	std::vector< double > getPowerIndices() const;

private:
	std::vector< double > powerIndices;

	void readPowerIndices(const std::string& line);
};

#endif
