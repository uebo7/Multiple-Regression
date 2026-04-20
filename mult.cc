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
  fillRandom (std::span<type>{data}, min, max, seed);

  // std::println ("{}", data);
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