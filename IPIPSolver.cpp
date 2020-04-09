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

double taxicabMDE();
double taxicabDE(double desired, double found);
double quadraticMDE();
double quadraticDE(double desired, double found);

IPIPSolver::IPIPSolver(const IPIPInstance& instance, const std::vector<double>& chromosome) : deltaError(0.0) {
    unsigned chromosomeSize = chromosome.size();

    // Parameters required to minimize
    //      Taxicab -> |d - p|
    //      Quadratic -> (d - p) * (d - p)
    double (*calcMaxDeltaError)() = quadraticMDE;
    double (*calcDeltaError)(double, double) = quadraticDE;

    // The chromossome is composed by the genomes corresponding to the quota (first position)
    // and the weights (second to last positions)

    // 1) Generate quota using n/genome^2
    double quota = (chromosomeSize - 1) * (chromosome[0] * chromosome[0]);

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

    // 4) Compute the power indices using external python script
    std::vector<double> powerIndicesFound(chromosomeSize - 1);

    Py_Initialize();

    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\"./power_index\")");
    PyObject* pName = PyUnicode_FromString("run");
    PyObject* pModule = PyImport_Import(pName);

    if (pModule) {
        PyObject* pFunc = PyObject_GetAttrString(pModule, "get_power_index_list");
        PyObject* l = PyList_New(chromosomeSize);
        PyList_SetItem(l, 0, PyFloat_FromDouble(quota));
        for (size_t i = 1; i < chromosomeSize; i++) {
            PyList_SetItem(l, i, PyFloat_FromDouble(weights[i - 1]));
        }
        PyObject* arglist = Py_BuildValue("(O)", l);

        if (pFunc && PyCallable_Check(pFunc)) {
            PyObject* pValue = PyObject_CallObject(pFunc, arglist);
            for (size_t i = 1; i < chromosomeSize; i++) {
                powerIndicesFound[i - 1] = PyFloat_AsDouble(PyList_GetItem(pValue, i));
            }
        } else {
            printf("ERROR: function getPowerIndexList()\n");
        }
    } else {
        printf("ERROR: Module not imported\n");
    }

    Py_Finalize();

    // 5) Compute the error between the desired and found power indices (delta)
    std::vector<double> powerIndicesDesired = instance.getPowerIndices();
    for (unsigned i = 0; i < chromosomeSize - 1; i++) {
        deltaError += calcDeltaError(powerIndicesDesired[i], powerIndicesFound[i]);
    }
}

IPIPSolver::~IPIPSolver() {}

double IPIPSolver::getDeltaError() const { return deltaError; }

double taxicabMDE() {
    // Delta error is the maximum error defined for |d - p|, plus 1
    return 3;
}

double taxicabDE(double desired, double found) {
    // Delta error is |d - p|
    return abs(desired - found);
}

double quadraticMDE() {
    // Delta error is the maximum error defined for (d - p)^2, plus 1
    return 5;
}

double quadraticDE(double desired, double found) {
    // Delta error is (d - p)^2
    return (desired - found) * (desired - found);
}