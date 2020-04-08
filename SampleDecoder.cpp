/*
 * SampleDecoder.cpp
 *
 *  Created on: Jan 14, 2011
 *      Author: rtoso
 */

#include "SampleDecoder.h"
#include <python3.7m/Python.h>
#include <stdlib.h>
#include <iostream>
#include <numeric>

SampleDecoder::SampleDecoder() {}

SampleDecoder::~SampleDecoder() {}


double SampleDecoder::decode(const std::vector<double> &chromosome) const {
    unsigned chromosomeSize = chromosome.size();

    // std::vector<double> powerIndexDesired{0.45, 0.2, 0.2, 0.116667, 0.033333};
    std::vector<double> powerIndexDesired{0.532143, 0.125, 0.125, 0.10119, 0.048810, 0.022619, 0.022619, 0.022619};
    std::vector<double> powerIndexFound(chromosomeSize - 1);

    double quota;
    quota = (chromosomeSize-1) * (chromosome[0]*chromosome[0]);
    // std::cout << "\nQuota: " << quota << std::endl;

    double sum = std::accumulate(chromosome.begin() + 1, chromosome.end(), 0.0);
    if(sum < quota){
        // std::cout << "1+";
        return 5;
    }
    std::vector<double> weights(chromosomeSize - 1);
    // for (unsigned i = 0; i < chromosomeSize; i++) {
    //     // std::cout << chromosome[i] << " ";
    // }
    // std::cout << "| " << sum << std::endl;
    // std::cout << "Weights: \t";
    for (unsigned i = 1; i < chromosomeSize; i++) {
        // double weight = chromosome[i] / sum;
        double weight = chromosome[i];
        weights[i - 1] = weight;
        // std::cout << weights[i - 1] << " ";
        // if (weight > quota) {
        //     return chromosomeSize + 1;
        // }
    }
    // std::cout << std::endl;

    Py_Initialize();

    PyRun_SimpleString("import sys");
    PyRun_SimpleString("sys.path.append(\"./power_index\")");
    PyObject *pName = PyUnicode_FromString("run");
    PyObject *pModule = PyImport_Import(pName);

    if (pModule) {
        PyObject *pFunc = PyObject_GetAttrString(pModule, "get_power_index_list");

        PyObject *l = PyList_New(chromosomeSize);
        PyList_SetItem(l, 0, PyFloat_FromDouble(quota));
        for (size_t i = 1; i < chromosomeSize; i++) {
            PyList_SetItem(l, i, PyFloat_FromDouble(weights[i - 1]));
        }

        PyObject *arglist = Py_BuildValue("(O)", l);

        if (pFunc && PyCallable_Check(pFunc)) {
            PyObject *pValue = PyObject_CallObject(pFunc, arglist);
            // double powerIndexCheck = 0;
            // std::cout << "Power Index: \t";
            for (size_t i = 1; i < chromosomeSize; i++) {
                powerIndexFound[i - 1] = PyFloat_AsDouble(PyList_GetItem(pValue, i));
                // powerIndexCheck += powerIndexFound[i - 1];
                // std::cout << powerIndexFound[i - 1] << " ";
            }
            // if (powerIndexCheck < 0.98 || powerIndexCheck > 1.02) {
            //     std::cout << "\tPOWER INDEX TOTAL IS NOT 1 ! is " << powerIndexCheck << " | The weights and quota are:" << std::endl;
            //     std::cout << "[" << quota << " ; ";
            //     int flag = 0;
            //     for (unsigned k = 1; k < chromosomeSize; k++) {
            //         std::cout << weights[k-1];
            //         if(weights[k-1] < quota) flag = 1;
            //         if (k != chromosomeSize - 1) std::cout << " , ";
            //     }
            //     std::cout << "]" << std::endl;
            //     std::cout << "\tPower indices are:" << std::endl;
            //     std::cout << "[";
            //     for (unsigned k = 1; k < chromosomeSize; k++) {
            //         std::cout << powerIndexFound[k-1];
            //         if (k != chromosomeSize - 1) std::cout << " , ";
            //     }
            //     std::cout << "]" << std::endl;
            //     if(sum < quota) std::cout << "The sum is lower than the quota" << std::endl;
            //     if(!flag) std::cout << std::endl << "ALL WEIGHTS BIGGER THAN QUOTA" << std::endl << std::endl;
            // }
            // std::cout << std::endl;
        } else {
            printf("ERROR: function getPowerIndexList()\n");
        }

    } else {
        printf("ERROR: Module not imported\n");
    }

    Py_Finalize();

    double fitness = 0;
    // std::cout << "Adding power indices -> ";
    for (unsigned i = 0; i < chromosomeSize - 1; i++) {
        fitness += abs(powerIndexDesired[i] - powerIndexFound[i]);
        // fitness += (powerIndexDesired[i] - powerIndexFound[i])*(powerIndexDesired[i] - powerIndexFound[i]);
        // std::cout << "|" << powerIndexDesired[i] << "-" << powerIndexFound[i] << "|" << abs(powerIndexDesired[i] - powerIndexFound[i]);
        // if (i == chromosomeSize - 1) {
        //     std::cout << " = ";
        // } else {
        //     std::cout << " + ";
        // }
    }
    // std::cout << fitness << std::endl;
    // if (fitness < 0.05) {
    //     double f = 0;
    //     std::cout << "\nAdding power indices -> ";
    //     for (unsigned i = 0; i < chromosomeSize - 1; i++) {
    //         f += abs(powerIndexDesired[i] - powerIndexFound[i]);
    //         std::cout << "|" << powerIndexDesired[i] << "-" << powerIndexFound[i] << "|" << abs(powerIndexDesired[i] - powerIndexFound[i]);
    //         if (i == chromosomeSize - 1) {
    //             std::cout << " = ";
    //         } else {
    //             std::cout << " + ";
    //         }
    //     }
    //     std::cout << f << std::endl;
    //     std::cout << "\nQuota: " << quota << std::endl;

    //     std::cout << "Weights: \t";
    //     for (unsigned i = 1; i < chromosomeSize; i++) {
    //         std::cout << weights[i - 1] << " ";
    //     }
    //     std::cout << std::endl;
    // }
    return fitness;
}

// double SampleDecoder::decode(const std::vector<double> &chromosome) const {
//     unsigned chromosomeSize = chromosome.size();

//     std::vector<double> powerIndexDesired{0.45, 0.2, 0.2, 0.116667, 0.033333};
//     std::vector<double> powerIndexFound(chromosomeSize);

//     double quota;
//     quota = 1.0;
//     if(std::accumulate(chromosome.begin(), chromosome.end(), 0.0) < 1.0){
//         return chromosomeSize + 2;
//     }
//     Py_Initialize();

//     PyRun_SimpleString("import sys");
//     PyRun_SimpleString("sys.path.append(\"./power_index\")");
//     PyObject *pName = PyUnicode_FromString("run");
//     PyObject *pModule = PyImport_Import(pName);

//     if (pModule) {
//         PyObject *pFunc = PyObject_GetAttrString(pModule, "getPowerIndexList");

//         PyObject *l = PyList_New(chromosomeSize + 1);
//         PyList_SetItem(l, 0, PyFloat_FromDouble(quota));
//         // std::cout << "Weights: \t";

//         for (size_t i = 0; i < chromosomeSize; i++) {
//             // std::cout << chromosome[i] << " ";
//             PyList_SetItem(l, i + 1, PyFloat_FromDouble(chromosome[i]));
//         }
//         // std::cout << std::endl;

//         PyObject *arglist = Py_BuildValue("(O)", l);

//         if (pFunc && PyCallable_Check(pFunc)) {
//             PyObject *pValue = PyObject_CallObject(pFunc, arglist);
//             // double powerIndexCheck = 0;
//             // std::cout << "Power Index: \t";
//             for (size_t i = 0; i < chromosomeSize; i++) {
//                 powerIndexFound[i] = PyFloat_AsDouble(PyList_GetItem(pValue, i+1));
//                 // powerIndexCheck += powerIndexFound[i];
//                 // std::cout << powerIndexFound[i] << " ";
//             }
//             // if (powerIndexCheck < 0.98 || powerIndexCheck > 1.02) {
//             //     // std::cout << "\tPOWER INDEX TOTAL IS NOT 1 !" << std::endl;
//             // }
//         //     std::cout << std::endl;
//         } else {
//             printf("ERROR: function getPowerIndexList()\n");
//         }

//     } else {
//         printf("ERROR: Module not imported\n");
//     }

//     Py_Finalize();

//     double fitness = 0;
//     // std::cout << "Adding power indices -> ";
//     for (unsigned i = 0; i < chromosomeSize; i++) {
        
        
        
        
        
//         // FAZER QUADRATICO




//         fitness += abs(powerIndexDesired[i] - powerIndexFound[i]);
//         // std::cout << "|" << powerIndexDesired[i] << "-" << powerIndexFound[i] << "|" << abs(powerIndexDesired[i] - powerIndexFound[i]);
//         // if (i == chromosomeSize - 1) {
//         //     std::cout << " = ";
//         // } else {
//         //     std::cout << " + ";
//         // }
//     }
//     // std::cout << fitness << std::endl;
//     // if (fitness < 0.05) {
//     //     double f = 0;
//     //     std::cout << "\nAdding power indices -> ";
//     //     for (unsigned i = 0; i < chromosomeSize - 1; i++) {
//     //         f += abs(powerIndexDesired[i] - powerIndexFound[i]);
//     //         std::cout << "|" << powerIndexDesired[i] << "-" << powerIndexFound[i] << "|" << abs(powerIndexDesired[i] - powerIndexFound[i]);
//     //         if (i == chromosomeSize - 1) {
//     //             std::cout << " = ";
//     //         } else {
//     //             std::cout << " + ";
//     //         }
//     //     }
//     //     std::cout << f << std::endl;
//     //     std::cout << "\nQuota: " << quota << std::endl;

//     //     std::cout << "Weights: \t";
//     //     for (unsigned i = 1; i < chromosomeSize; i++) {
//     //         std::cout << weights[i - 1] << " ";
//     //     }
//     //     std::cout << std::endl;
//     // }
//     return fitness;
// }
