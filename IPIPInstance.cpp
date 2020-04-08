/*
 * IPIPInstance.cpp
 *
 *  Created on: Mar 16, 2013
 *      Author: Rodrigo
 */

#include "IPIPInstance.h"
#include <iostream>

IPIPInstance::IPIPInstance(const std::string& instanceFile) throw(IPIPInstance::Error) : powerIndices() {
    std::ifstream fin(instanceFile.c_str());
    if (!fin) {
        throw Error("IPIPInstance: Cannot open input file.");
    }

    std::string line;

    try {
        std::getline(fin, line);
        std::istringstream sin(line);
        unsigned n;
        sin >> n;
		
        std::getline(fin, line);
        for (unsigned i = 0; i < n; i++) {
            readPowerIndices(line);

            std::getline(fin, line);
        }
    } catch (const Error& error) {
        throw error;
    }
}

IPIPInstance::~IPIPInstance() {}

std::vector<double> IPIPInstance::getPowerIndices() const { return powerIndices; }

void IPIPInstance::readPowerIndices(const std::string& line) throw(IPIPInstance::Error) {
    std::istringstream sin(line);
    double value;
    sin >> value;

    if (value > 1 || value < 0) {
        throw Error("Power Index invalid.");
    }
    powerIndices.push_back(value);
}
