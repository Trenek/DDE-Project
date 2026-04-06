#include "capd/capdlib.h"
#include <iomanip>
#include <iostream>
using namespace capd;
using namespace std;


// the matrix needed for pseudospectral approximation
DMatrix approxMatrix;


/* functions needed for calculating approxMatrix */

DVector calculate_nodes(double tau, // time delay parameter
                        int N       // dimention
) {
  DVector s(N + 1);
  const double step = M_PI / double(N);
  for (int i = 0; i <= N; i++)
    s[i] = tau / 2. * (cos(double(i) * step) - 1.);
  return s;
}

DVector calculate_c(const DVector& s, // chebyshev nodes
                    int N      // dimention
) {
  DVector c(N + 1);
  for (int i = 0; i <= N; i++) {
    c[i] = 1.;
    for (int j = 0; j <= N; j++) {
      if (j != i)
        c[i] *= (s[i] - s[j]);
    }
  }
  return c;
}


/* computing approxMatrix */

DMatrix compute_approxMatrix(double tau,        // time delay parameter
                             int N,             // dimention
                             bool save = false  // whether or not to save the matrix in .txt
) {
  DMatrix M(N + 1, N + 1);

  DVector s = calculate_nodes(tau, N);
  DVector c = calculate_c(s, N);

  for (int i = 0; i <= N; i++) {
    double tmp = 0.;
    for (int j = 0; j <= N; j++) {
      if (i != j) {
        double val = c[i] / (c[j] * (s[i] - s[j]));
        M[i][j] = val;
        tmp -= val;
      }
    }
    M[i][i] = tmp;
  }
  if (save) {
    std::ofstream out("matrixM.txt");
    for (int i = 0; i <= N; i++)
      out << M[i] << "\n";
    out.close();
  }
  return M;
}


/* Pseudospectral approximation of Cubic Ikeda 
   f(x(t), x(t-tau)) = a(x(t-tau) - x(t-tau)^3)
*/

void approx_CubicIkeda(capd ::autodiff ::Node /*t*/, // unused time variable
                       capd ::autodiff ::Node in[], int dimIn, // input variables x1 ,... , xn
                       capd ::autodiff ::Node out[], int dimOut, // output : function values
                       capd ::autodiff ::Node params[], int noParam // parameters
) {
  int N = dimIn - 1;
  
  // out[0] = f(x(t), x(t-tau)) 
  //        = f(in[0], in[N]) 
  //        = a(in[N]-in[N])^3
  capd ::autodiff ::Node a = params[0];
  capd ::autodiff ::Node xDelayed = in[N];
  out[0] = a * (xDelayed - xDelayed * xDelayed * xDelayed);

  for (int i = 1; i <= N; i++) {
    capd ::autodiff ::Node result(0.);
    for (int j = 0; j <= N; j++)
      result += approxMatrix[i][j] * in[j];
    out[i] = result;
  }
}



int main() {
  int N = 6;
  int dimIn = N + 1, dimOut = N + 1, noParams = 1, highestDerivative = 1;
  double tau = 1., a = 1.55;

  approxMatrix = compute_approxMatrix(tau, N);

  DMap CubicIkeda(approx_CubicIkeda, dimIn, dimOut, noParams, highestDerivative);
  CubicIkeda.setParameters({a});

  int order = 20;
  DOdeSolver solver(CubicIkeda, order);
  DTimeMap timeMap(solver);

  DVector x(N + 1);
  for (int i = 0; i <= N; i++)
    x[i] = 0.01;

  DTimeMap::SolutionCurve solution(0.);
  timeMap(500.0, x, solution);

  ofstream csv("output/curve.csv");
  csv << "x_present,x_delayed\n";
  for (double t = 100.; t <= 500.0; t += 0.05) {
    DVector point = solution(t);
    csv << point[0] << ","  << point[N] << "\n";
  }
  csv.close();

  return 0;
}