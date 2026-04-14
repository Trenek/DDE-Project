#include "computations/integrateFuncs.h"
#include "computations/toOdeFuncs.h"
#include <iomanip>
#include <iostream>
using namespace capd;
using namespace std;

// the matrix needed for pseudospectral approximation
DMatrix approxMatrix;

/*
Pseudospectral approximation of Cubic Ikeda
f(x(t), x(t-tau)) = a(x(t-tau) - x(t-tau)^3)
*/

void approx_CubicIkeda(capd ::autodiff ::Node /*t*/, // unused time variable
                       capd ::autodiff ::Node in[],
                       int dimIn, // input variables x1 ,... , xn
                       capd ::autodiff ::Node out[],
                       int dimOut, // output : function values
                       capd ::autodiff ::Node params[],
                       int noParam // parameters
) {
  int N = dimIn - 1;

  // out[0] = f(x(t), x(t-tau))
  //        = f(in[0], in[N])
  //        = a(in[N]-in[N])^3
  capd ::autodiff ::Node a = params[0];
  capd ::autodiff ::Node xDelayed = in[N];
  out[0] = a * xDelayed * (1 - xDelayed * xDelayed);

  for (int i = 1; i <= N; i++) {
    capd ::autodiff ::Node result(0.);
    for (int j = 0; j <= N; j++)
      result += approxMatrix[i][j] * in[j];
    out[i] = result;
  }
}

// MAIN FUNCTION //

int main() {
  int N = 6;
  int dimIn = N + 1, dimOut = N + 1, noParams = 1, highestDerivative = 1;
  double tau = 1., a = 1.57;
  int taylorOrder = 20;
  double returnTime = 0;

  // relative paths for storing produced data
  string filenameM = "output/matrixM.txt";
  string filenamePoincare = "output/poincare.txt";
  string filenameSolutionCurve = "output/curve.csv";
  string filenameBifurc = "output/bifurcDiagram.png";

  DVector x(N + 1);
  for (int i = 0; i <= N; i++)
    x[i] = 0.5;
  cout << "Starting point: " << x << endl;

  approxMatrix = compute_approxMatrix(tau, N, filenameM);

  DMap CubicIkeda(approx_CubicIkeda, dimIn, dimOut, noParams,
                  highestDerivative);
  CubicIkeda.setParameters({a});

  DOdeSolver solver(CubicIkeda, taylorOrder);
  DTimeMap timeMap(solver);

  // eventually will change to plot it using gnuplot,
  // for now it save it in a file and plots in python script
  DTimeMap::SolutionCurve solution =
      getSolutionCurve(timeMap, x, 500., 0., filenameSolutionCurve);

  double aStart = 1.35;
  double aEnd = 1.56;
  double aIncrease = 0.0005;
  double noSteps = 1000;

  DCoordinateSection section(N + 1, 0, 0);
  DPoincareMap pm(solver, section, poincare::MinusPlus);

  getPoincareValues(pm, x, filenamePoincare);

  plotBifurcationDiagram(CubicIkeda, pm, x, aStart, aEnd, aIncrease, noSteps,
                         filenameBifurc);
  return 0;
}