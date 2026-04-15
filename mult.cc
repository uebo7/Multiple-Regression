/*
 * Filename: mult.cc
 * Authors: Logan Kaufman & Adrian Olweiler
 * Class: Parallel Programming
 * Description: Uses Jthreads that perform multiple functions in parallel to
 * compute the confidence interval of a multiple regression problem given a
 * dataset.

 * Assumptions:
    - 3 sets of data
    - N is equal in every data set

 * Notes:
    - substituting ta/2 for z score
    - Data minus average can be calculated in parallel and in a separate
    function to make it possible to calculate the sum of squares and products in
    parallel. This allows us to use 6 threads in parallel
 */

/**************************************************************************/
// include

#include <algorithm>
#include <iostream>
#include <print>
#include <random>
#include <thread>
#include <type_traits>
#include <vector>

/**************************************************************************/
// headers

void
getInput ();

void
printResults ();

void
computeAverage ();

void
fillRandom ();

void
calcSumOfSquares ();

void
calcSumOfProducts ();

void
calcSlope ();

void
computePointEstimate ();

void
computeStandardErr ();

void
findConfidenceInt ();

/**************************************************************************/
// main

int
main ()
{
  return 0;
}

/**************************************************************************/
// functions

// Needs N from user for amount of data obtained in each column
void
getInput ()
{
  int N {};
  std::print ("N ==> ");
  std::cin >> N;
}

/*
 * print the confidence interval
 */
void
printResults ();

// fill each column with "N" random data values (range TBD)
template<typename T, typename U>
  requires std::is_arithmetic_v<T>
void
fillRandom (std::span<T> seq, U min, U max, unsigned seed)
{
  static std::minstd_rand generator (seed);
  std::uniform_real_distribution<float> distribute (min, max);
  std::ranges::generate (seq, [&] () { return distribute (generator); });
}

// uses jthreads to get average of all 3 columns of data in parallel
float
computeAverage (std::vector<float> dataValues)
{
  float total {};
  for (float value : dataValues)
    total += value;

  float average = total / dataValues.size ();
  return average;
}

// calculates x, y, and z minus respective averages
std::vector<float>
calcColumns (std::vector<float>& dataset, float average);

// Use Jthreads to calculate sum of squares functions in parallel (3)
// Requires averages from previous threads
void
calcSumOfSquares ();

// Use Jthreads to calculate sum of products functions in parallel (3)
void
calcSumOfProducts ();

// Use Jthreads to calculate slopes and intercept
// relies on previous data

void
calcSlope ();

// Calculate point estimate
// No threads needed one operation and relies on previous input. Next step
// relies on this
void
computePointEstimate ();

// Use Jthreads to compute Standard Error of 3 random variables
void
computeStandardErr ();

// Use Jthreads to find confidence interval of Beta
void
findConfidenceInt ();
