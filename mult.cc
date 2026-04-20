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
    - alpha is between .01 and .99

 * Notes:
    - substituting ta/2 for z score
    - Data minus average can be calculated in parallel and in a separate
    function to make it possible to calculate the sum of squares and products in
    parallel. This allows us to use 6 threads in parallel
 */

/**************************************************************************/
// include
#include <boost/math/distributions/students_t.hpp>
#include <cmath>
#include <iostream>
#include <print>
#include <span>
#include <thread>
#include <vector>

// local include

#include "ThreadSafeQueue.hpp"
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

  {
    std::jthread t1 (
      [&]
      {
        auto avg1 = computeAverage (x1);
        x1 = findDataValues (x1, avg1);
      });

    std::jthread t2 (
      [&]
      {
        auto avg2 = computeAverage (x2);
        x2 = findDataValues (x2, avg2);
      });

    std::jthread t3 (
      [&]
      {
        auto avg3 = computeAverage (y);
        y = findDataValues (y, avg3);
      });
  }
  type sum1, sum2, sum3;
  type prod1, prod2, prod3;
  {
    std::jthread sumsquare1 ([&] { sum1 = calcSumOfSquares (x1); });
    std::jthread sumsquare2 ([&] { sum2 = calcSumOfSquares (x2); });
    std::jthread sumsquare3 ([&] { sum3 = calcSumOfSquares (y); });

    std::jthread sumproduct1 ([&] { prod1 = calcSumOfProducts (x1, x2); });
    std::jthread sumproduct2 ([&] { prod2 = calcSumOfProducts (x2, y); });
    std::jthread sumproduct3 ([&] { prod3 = calcSumOfProducts (x1, y); });
  }
  type slop1, slop2;
  {
    //bools at the end are to determine which slope equation to use
    std::jthread slope1 (
      [&] { slop1 = calcSlopes (sum1, sum2, prod1, prod2, prod3, true); });
    std::jthread slope2 (
      [&] { slop2 = calcSlopes (sum1, sum2, prod1, prod3, prod3, false); });
  }
  std::println ("{}", x1);
  std::println ("{}", x2);
  std::println ("{}", y);

  std::println ("{}", prod1);
  std::println ("{}", prod2);
  std::println ("{}", prod3);
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
  std::print ("\nalpha  ==> ");
  std::cin >> in.alpha;
  return in;
}

// print the confidence interval
void
printResults ();