/*
 * TSPSolver.h
 *
 *  Created on: Mar 16, 2013
 *      Author: Rodrigo
 */

#ifndef TSPSOLVER_H
#define TSPSOLVER_H

#include <list>
#include <limits>
#include <vector>
#include <algorithm>
#include "IPIPInstance.h"

class IPIPSolver {
public:
	IPIPSolver(const IPIPInstance& instance, const std::vector< double >& chromosome);
	virtual ~IPIPSolver();

	double getDeltaError() const;

private:
	double deltaError;
};

#endif
