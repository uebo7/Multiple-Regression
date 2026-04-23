/*
 * Filename: mult.hpp
 * Authors: Adrian Olweiler & Logan Kaufman
 * Description: Header file for template functions used in mult.cc
 */

#include <algorithm>
#include <boost/math/distributions/students_t.hpp>
#include <cmath>
#include <execution>
#include <numeric>
#include <random>
#include <ranges>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

template<typename T>
struct StandardErrors
{
  T B0;
  T B1;
  T B2;
};

template<typename T>
struct ConfidenceInterval
{
  T lower;
  T upper;
};

//O(n)
template<typename T, typename U>
  requires std::is_arithmetic_v<T>
void
fillRandom (std::span<T> seq, U min, U max, unsigned seed)
{
  static std::minstd_rand gen (seed);

  std::uniform_real_distribution<T> fDistribution (min, max);
  std::ranges::generate (seq, [&] () { return fDistribution (gen); });
}

// uses jthreads to get average of all 3 columns of data in parallel
//O(n)
template<typename T>
T
computeAverage (const std::vector<T>& dataValues)
{
  T total{0};
  for (auto x : dataValues)
  {
    total += x;
  }

  return total / dataValues.size ();
}

//O(n)
template<typename T>
void
findDataValues (std::vector<T>& dataValues, T average)
{
  for (auto x : dataValues)
  {
    x = x - average;
  }
}

// Use Jthreads to calculate sum of squares functions in parallel (3)
// Requires averages from previous threads
//O(n)
template<typename T>
T
calcSumOfSquares (const std::vector<T>& dataValues)
{
  T total{};
  for (auto value : dataValues)
  {
    T squared = std::pow (value, 2);
    total += squared;
  }

  return total;
}

// Use Jthreads to calculate sum of products functions in parallel (3)
//O(n)
template<typename T>
T
calcSumOfProducts (const std::vector<T>& firstValue,
                   const std::vector<T>& secondValue)
{
  T total{};

  for (auto [a, b] : std::views::zip (firstValue, secondValue))
  {
    total += a * b;
  }
  return total;
}

// Use Jthreads to calculate slopes and intercept
// relies on previous data

//O(1)
template<typename T>
std::pair<T, T>
calcSlopes (const T S11, T S22, T S12, T S1y, T S2y)
{
  const T denominator = (S11 * S22 - S12 * S12);

  T b1 = (S1y * S22 - (S2y * S12)) / denominator;
  T b2 = (S2y * S11 - (S1y * S12)) / denominator;

  return std::pair<T, T> (b1, b2);
}

template<typename T>
T
//O(1)
calcIntercept (const T ybar, const T b1, const T xbar1, const T b2,
               const T xbar2)
{
  return ybar - (b1 * xbar1) - (b2 * xbar2);
}

// Calculate point estimate
// No threads needed one operation and relies on previous input. Next step
// relies on this
//O(1)
template<typename T>
T
computePointEstimate (T Syy, T b1, T S11, T b2, T S22, T S1y, T S2y, T S12,
                      int N)
{
  T SSE = Syy + b1 * b1 * S11 + b2 * b2 * S22 - 2 * b1 * S1y - 2 * b2 * S2y +
          2 * b1 * b2 * S12;

  return std::sqrt (SSE / (N - 3));
}

// Use Jthreads to compute Standard Error of 3 random variables
//O(1)
template<typename T>
StandardErrors<T>
computeStandardErr (T S, int N, T xbar1, T S22, T xbar2, T S11, T S12)
{
  const T denominator = S11 * S22 - S12 * S12;

  StandardErrors<T> se = {
    S * (std::sqrt (T (1) / N +
                    (xbar1 * S22 + xbar2 * S11 - 2 * xbar1 * xbar2 * S12) /
                      denominator)),
    S * (std::sqrt (S22 / denominator)),
    S * (std::sqrt (S11 / denominator)),
  };

  return se;
}

// Use Jthreads to find confidence interval of Beta
//O(1)
template<typename T>
ConfidenceInterval<T>
findConfidenceInt (T b, T se, double alpha, int N)
{
  int df{N - 3};
  boost::math::students_t dist (df);
  T tAlphaOver2 = quantile (boost::math::complement (dist, alpha / 2));

  ConfidenceInterval<T> ci = {b - tAlphaOver2 * se, b + tAlphaOver2 * se};

  return ci;
}
