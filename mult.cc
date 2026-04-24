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

  //needed
  auto normalize = [] (auto& v)
  {
    auto mean = std::reduce (v.begin (), v.end ()) / v.size ();

    auto var = std::transform_reduce (v.begin (), v.end (), 0.0, std::plus<> (),
                                      [mean] (auto x)
                                      { return (x - mean) * (x - mean); }) /
               v.size ();

    auto stddev = std::sqrt (var + 1e-12);

    for (auto& x : v)
      x = (x - mean) / stddev;
  };

  normalize (x1);
  normalize (x2);
  normalize (y);

  std::println ("PARALLEL");
  runParallel (x1, x2, y, input);
  std::println ("SERIAL");
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
  std::string N;
  std::cin >> N;
  N.erase (std::remove (N.begin (), N.end (), '\''), N.end ());
  in.n = std::stoul (N);
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
  T xbar1, xbar2, ybar;
  T S11, S22, Syy;
  {
    std::jthread t1 (
      [&]
      {
        xbar1 = computeAverage (x1);
        findDataValues (x1, xbar1);
        S11 = calcSumOfSquares (x1);
      });
    std::jthread t2 (
      [&]
      {
        xbar2 = computeAverage (x2);
        findDataValues (x2, xbar2);
        S22 = calcSumOfSquares (x2);
      });
    std::jthread t3 (
      [&]
      {
        ybar = computeAverage (y);
        findDataValues (y, ybar);
        Syy = calcSumOfSquares (y);
      });
  }
  T S12, S1y, S2y;
  {

    std::jthread sumproduct1 ([&] { S12 = calcSumOfProducts (x1, x2); });
    std::jthread sumproduct2 ([&] { S1y = calcSumOfProducts (x2, y); });
    std::jthread sumproduct3 ([&] { S2y = calcSumOfProducts (x1, y); });
  }
  auto [b1, b2] = calcSlopes (S11, S22, S12, S1y, S2y);
  T intercept = calcIntercept (ybar, b1, xbar1, b2, xbar2);
  // point estimate
  T point = computePointEstimate (Syy, b1, S11, b2, S22, S1y, S2y, S12, in.n);
  StandardErrors<T> standardErr =
    computeStandardErr (point, in.n, xbar1, S22, xbar2, S11, S12);
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

  T xbar1 = computeAverage (x1);
  findDataValues (x1, xbar1);
  T xbar2 = computeAverage (x2);
  findDataValues (x2, xbar2);
  T ybar = computeAverage (y);
  findDataValues (y, ybar);

  T S11 = calcSumOfSquares (x1);
  T S22 = calcSumOfSquares (x2);
  T Syy = calcSumOfSquares (y);

  T S12 = calcSumOfProducts (x1, x2);
  T S1y = calcSumOfProducts (x2, y);
  T S2y = calcSumOfProducts (x1, y);
  auto [b1, b2] = calcSlopes (S11, S22, S12, S1y, S2y);
  T intercept = calcIntercept (ybar, b1, xbar1, b2, xbar2);
  // point estimate
  T point = computePointEstimate (Syy, b1, S11, b2, S22, S1y, S2y, S12, in.n);
  StandardErrors<T> standardErr =
    computeStandardErr (point, in.n, xbar1, S22, xbar2, S11, S12);
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
  std::println ("{:.3f} < B0 < {:.3f}", ce1.lower, ce1.upper);
  std::println ("{:.3f} < B1 < {:.3f}", ce2.lower, ce2.upper);
  std::println ("{:.3f} < B2 < {:.3f}", ce3.lower, ce3.upper);
  std::println ("Time:  {:.2f} ms\n", time);
}
