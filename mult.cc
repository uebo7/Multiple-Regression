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
#include <span>
// #include <thread>
#include <type_traits>
#include <vector>

/**************************************************************************/
// headers

unsigned
getInput ();

void
printResults ();

template<typename T>
T
computeAverage (std::vector<T> dataValues);

template<typename T>
std::vector<T>
findDataValues (std::vector<T> dataValues, T average);

template<typename T, typename U>
  requires std::is_arithmetic_v<T>
void
fillRandom (std::span<T> seq, U min, U max, unsigned seed);

template<typename T>
T
calcSumOfSquares (std::vector<T> dataValues);

template<typename T>
T
calcSumOfProducts (std::vector<T> firstValue, std::vector<T> secondValues);

template<typename T>
T
calcSlopes (T S11, T S22, T S12, T S1y, T S2y);

template<typename T>
T
calcFinalSlope (T ybar, T b1, T xbar1, T b2, T xbar2);

template<typename T>
T
computePointEstimate (T Syy, T b1, T S11, T b2, T S22, T S1y, T S2y, T S12);

template<typename T>
T
computeStandardErr (T S, T N, T xbar1, T S22, T xbar2, T S11, T S12);

template<typename T>
T
findConfidenceInt (T b, T se);

/**************************************************************************/
// main

int
main ()
{
  unsigned N;
  N = getInput ();
  using type = float;

  std::vector<type> data (N);

  int min { 1 };
  int max { 100 };
  unsigned seed { 1 };
  fillRandom (std::span<type> { data }, min, max, seed);

  // std::println ("{}", data);
}

/**************************************************************************/
// functions

// Needs N from user for amount of data obtained in each column
unsigned
getInput ()
{
  std::print ("Size   ==> ");
  unsigned n;
  std::cin >> n;
  return n;
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
  std::minstd_rand gen (0);
  if constexpr (std::is_floating_point_v<T>)
  {
    std::uniform_real_distribution<T> fDistribution (min, max);
    std::ranges::generate (seq, [&] () { return fDistribution (gen); });
  }
  else
  {
    std::uniform_int_distribution<int> iDistribution (min, max);
    std::ranges::generate (seq, [&] () { return iDistribution (gen); });
  }
}

template<typename T>
std::vector<T>
findDataValues (std::vector<T> dataValues, T average);

// uses jthreads to get average of all 3 columns of data in parallel
template<typename T>
T
computeAverage (std::vector<T> dataValues)
{
  T total {};
  for (T value : dataValues)
    total += value;

  T average = total / dataValues.size ();
  return average;
}

// Use Jthreads to calculate sum of squares functions in parallel (3)
// Requires averages from previous threads
template<typename T>
T
calcSumOfSquares (std::vector<T> dataValues);

// Use Jthreads to calculate sum of products functions in parallel (3)
template<typename T>
T
calcSumOfProducts (std::vector<T> firstValue, std::vector<T> secondValues);

// Use Jthreads to calculate slopes and intercept
// relies on previous data

template<typename T>
T
calcSlopes (T S11, T S22, T S12, T S1y, T S2y);

template<typename T>
T
calcFinalSlope (T ybar, T b1, T xbar1, T b2, T xbar2);

// Calculate point estimate
// No threads needed one operation and relies on previous input. Next step
// relies on this
template<typename T>
T
computePointEstimate (T Syy, T b1, T S11, T b2, T S22, T S1y, T S2y, T S12);

// Use Jthreads to compute Standard Error of 3 random variables
template<typename T>
T
computeStandardErr (T S, T N, T xbar1, T S22, T xbar2, T S11, T S12);

// Use Jthreads to find confidence interval of Beta
template<typename T>
T
findConfidenceInt (T b, T se);
