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
#include <algorithm>
#include <iostream>
#include <print>
#include <random>
#include <span>
// #include <thread>
#include <condition_variable>
#include <mutex>
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
void
findDataValues (std::vector<T>& dataValues, const T average);

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
calcSlope ();

template<typename T>
T
computePointEstimate ();

template<typename T>
T
computeStandardErr ();

template<typename T>
T
findConfidenceInt ();

/**************************************************************************/
// main

int
main ()
{
  unsigned N;
  N = getInput ();
  using type = float;
  std::mutex m_mutex;
  std::condition_variable m_cv;

  std::vector<type> x1 (N);
  std::vector<type> x2 (N);
  std::vector<type> y (N);

  int min{1};
  int max{100};
  unsigned seed{1};

  fillRandom (std::span<type>{x1}, min, max, seed);
  fillRandom (std::span<type>{x2}, min, max, seed);
  fillRandom (std::span<type>{y}, min, max, seed);

  {
    std::jthread averageX1 ([&] ()
                            { findDataValues (x1, computeAverage (x1)); });
    std::jthread averageX2 ([&] ()
                            { findDataValues (x2, computeAverage (x2)); });
    std::jthread averageY ([&] () { findDataValues (y, computeAverage (y)); });
  }

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
  std::minstd_rand gen (seed);
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
void
findDataValues (std::vector<T>& dataValues, const T average)
{
  for (auto& x : dataValues)
  {
    x = x - average;
  }
}
// uses jthreads to get average of all 3 columns of data in parallel
template<typename T>
T
computeAverage (std::vector<T> dataValues)
{
  float total{};
  for (float value : dataValues)
    total += value;

  float average = total / dataValues.size ();
  return average;
}

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
