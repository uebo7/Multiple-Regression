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
getInput ();

/*
 * print the confidence interval
 */
void
printResults ();

// uses jthreads to get average of all 3 columns of data in paralle
void
computeAverage ();

// fill each column with "N" random data values (range TBD)
void
fillRandom ();

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
