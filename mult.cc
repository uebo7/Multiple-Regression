/*
 * Filename: mult.cc
 * Authors: Logan Kaufman & Adrian Olweiler
 * Class: Parallel Programming
 * Description: Uses Jthreads that perform multiple functions in parallel to
 * compute the confidence interval of a multiple regression problem given a
 * dataset.

 * Assumptions:
    - 3 sets of data
    - N is equal in every data set (vector)
    - N > 3
    - denominators can't be 0
    - alpha is between .01 and .99

 * Notes:
    - Data minus average can be calculated in parallel and in a separate
    function to make it possible to calculate the sum of squares and products in
    parallel. This allows us to use 6 threads in parallel
 */

/**************************************************************************/
// include
#include "Timer.hpp"
#include <boost/math/distributions/students_t.hpp>
#include <cmath>
#include <iostream>
#include <print>
#include <span>
#include <thread>
#include <vector>

// local include

// #include "ThreadSafeQueue.hpp"
#include "mult.hpp"

/**************************************************************************/
// headers

struct Input
{
  unsigned n;
  double alpha;
};

Input
getInput ();

template<typename T>
void
runParallel (std::vector<T> x1, std::vector<T> x2, std::vector<T> y, Input in);

template<typename T>
void
runSerial (std::vector<T> x1, std::vector<T> x2, std::vector<T> y, Input in);

void
printResults ();

/**************************************************************************/
// main

int
main ()
{
  auto input = getInput ();
  unsigned N = input.n;
  using type = float;

  std::vector<type> x1 (N);
  std::vector<type> x2 (N);
  std::vector<type> y (N);

  int min{1};
  int max{100};
  unsigned seed{1};
  fillRandom (std::span<type>{x1}, min, max, seed);
  fillRandom (std::span<type>{x2}, min, max, seed);
  fillRandom (std::span<type>{y}, min, max, seed);

  runParallel (x1, x2, y, input);
  runSerial (x1, x2, y, input);
}

/**************************************************************************/
// functions

// Needs N from user for amount of data obtained in each column
// Needs alpha for confidence interval
Input
getInput ()
{
  Input in{};
  std::print ("Size   ==> ");
  std::cin >> in.n;
  std::print ("alpha  ==> ");
  std::cin >> in.alpha;
  return in;
}

template<typename T>
void
runParallel (std::vector<T> x1, std::vector<T> x2, std::vector<T> y, Input in)
{
  Timer timer;
  timer.start ();
  T avg1, avg2, avg3;
  T sum1, sum2, sum3;
  {
    std::jthread t1 (
      [&]
      {
        avg1 = computeAverage (x1);
        findDataValues (x1, avg1);
        sum1 = calcSumOfSquares (x1);
      });
    std::jthread t2 (
      [&]
      {
        avg2 = computeAverage (x2);
        findDataValues (x2, avg2);
        sum2 = calcSumOfSquares (x2);
      });
    std::jthread t3 (
      [&]
      {
        avg3 = computeAverage (y);
        findDataValues (y, avg3);
        sum3 = calcSumOfSquares (y);
      });
  }
  T prod1, prod2, prod3;
  {

    std::jthread sumproduct1 ([&] { prod1 = calcSumOfProducts (x1, x2); });
    std::jthread sumproduct2 ([&] { prod2 = calcSumOfProducts (x2, y); });
    std::jthread sumproduct3 ([&] { prod3 = calcSumOfProducts (x1, y); });
  }
  auto [b1, b2] = calcSlopes (sum1, sum2, prod1, prod2, prod3);
  T intercept = calcIntercept (avg3, b1, avg1, b2, avg2);
  // point estimate
  T point =
    computePointEstimate (sum3, b1, sum1, b2, sum2, prod2, prod3, prod1, in.n);
  StandardErrors<T> standardErr =
    computeStandardErr (point, in.n, avg1, sum2, avg2, sum1, prod1);
  ConfidenceInterval<T> ce1, ce2, ce3;
  {
    std::jthread confidenceInterval1 (
      [&] { ce1 = findConfidenceInt (b1, standardErr.B0, in.alpha, in.n); });
    std::jthread confidenceInterval2 (
      [&] { ce2 = findConfidenceInt (b2, standardErr.B1, in.alpha, in.n); });
    std::jthread confidenceInterval3 (
      [&]
      { ce3 = findConfidenceInt (intercept, standardErr.B2, in.alpha, in.n); });
  }
  timer.stop ();
  double time = timer.getElapsedMs ();

  printResults (ce1, ce2, ce3, time);
}

template<typename T>
void
runSerial (std::vector<T> x1, std::vector<T> x2, std::vector<T> y, Input in)
{
  Timer timer;
  timer.start ();

  T avg1 = computeAverage (x1);
  findDataValues (x1, avg1);
  T avg2 = computeAverage (x2);
  findDataValues (x2, avg2);
  T avg3 = computeAverage (y);
  findDataValues (y, avg3);

  T sum1 = calcSumOfSquares (x1);
  T sum2 = calcSumOfSquares (x2);
  T sum3 = calcSumOfSquares (y);

  T prod1 = calcSumOfProducts (x1, x2);
  T prod2 = calcSumOfProducts (x2, y);
  T prod3 = calcSumOfProducts (x1, y);
  auto [b1, b2] = calcSlopes (sum1, sum2, prod1, prod2, prod3);
  T intercept = calcIntercept (avg3, b1, avg1, b2, avg2);
  // point estimate
  T point =
    computePointEstimate (sum3, b1, sum1, b2, sum2, prod2, prod3, prod1, in.n);
  StandardErrors<T> standardErr =
    computeStandardErr (point, in.n, avg1, sum2, avg2, sum1, prod1);
  ConfidenceInterval<T> ce1 =
    findConfidenceInt (b1, standardErr.B0, in.alpha, in.n);
  ConfidenceInterval<T> ce2 =
    findConfidenceInt (b2, standardErr.B1, in.alpha, in.n);
  ConfidenceInterval<T> ce3 =
    findConfidenceInt (intercept, standardErr.B2, in.alpha, in.n);

  timer.stop ();
  double serialTime = timer.getElapsedMs ();
  printResults (ce1, ce2, ce3, serialTime);
}

// print the confidence interval
template<typename T>
void
printResults (const ConfidenceInterval<T> ce1, const ConfidenceInterval<T> ce2,
              const ConfidenceInterval<T> ce3, const double time)
{
  std::println ("{} < B0 < {}", ce1.lower, ce1.upper);
  std::println ("{} < B1 < {}", ce2.lower, ce2.upper);
  std::println ("{} < B2 < {}", ce3.lower, ce3.upper);
  std::println ("Time:  {}\n", time);
}
