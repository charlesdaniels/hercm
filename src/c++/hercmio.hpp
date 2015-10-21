#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <typeinfo>
#include <cmath>
#include <algorithm>
#include <iterator>

#ifndef HERCMIO_H
#define HERCMIO_H

#define HERCMIO_STATUS_FAILURE 100
#define HERCMIO_STATUS_SUCCESS 101

using namespace std;


bool checkVectorForString(vector<string>);
int stringToInt(string);
float stringToFloat(string);
double stringToDouble(string);
vector<string> split(string, char);
int readHercmHeader(string, int&, int&, int&, string&, float&);
int readHercm(string, float *, int *, int *);
int readHercm(string, double *, int *, int *);
bool checkIfSorted(int *, int);
bool checkIfRowMajor(int *, int *, int);
int cooToCsr(int *, int *, float *, int *, int, int);
int cooToCsr(int *, int *, double *, int *, int, int);
int makeRowMajor(int *, int *, float *, int);
int makeRowMajor(int *, int *, double *, int);
float generateVerificationSum(int *, int* , float *, int);
float generateVerificationSum(int *, int* , double *, int);
bool verifyMatrix(string, float *, int *, int *);
bool verifyMatrix(string, double *, int *, int *);
int writeHercm(string, int, int, int, float *, int *, int *, string, float);
int writeHercm(string, int, int, int, double *, int *, int *, string, float);
#endif