#include <capd/capdlib.h>

#include "mackeyGlass.hpp"
#include "draw.hpp"

#define N 5

static void setGNUPlot(int id, struct thing &drawer) {
    fprintf(drawer.gnuplot, "set term qt %d size 800,600\n", id);
    fprintf(drawer.gnuplot, "set title '%s'\n", drawer.name);
    fprintf(drawer.gnuplot, "set xlabel '%s'\n", drawer.xName);
    fprintf(drawer.gnuplot, "set ylabel '%s'\n", drawer.yName);
    fprintf(drawer.gnuplot, "set pointsize 1.5\n");
    fprintf(drawer.gnuplot, "set grid\n");

    fprintf(drawer.gnuplot, "pause 1\n");

    fprintf(drawer.gnuplot, "plot "
            "\"%s\" index 0 with lines lw 2 lc rgb 'orange' title 'X', "
            "\"%s\" index 1 with lines lw 2 lc rgb 'cyan' title 'X L', "
            "\"%s\" index 2 with lines lw 2 lc rgb 'red' title 'X R', "
            "\"%s\" index 3 with lines lw 2 lc rgb 'green' title 'f(X)', "
            "\"%s\" index 4 with lines lw 2 lc rgb 'blue' title 'f(X) L', "
            "\"%s\" index 5 with lines lw 2 lc rgb 'yellow' title 'f(X) R'\n",
    drawer.file, drawer.file, drawer.file, drawer.file, drawer.file, drawer.file);

    fprintf(drawer.gnuplot, "bind 'q' 'exit'\n");
    fprintf(drawer.gnuplot, "while (1) {\n");
    fprintf(drawer.gnuplot, "    pause 1\n");
    fprintf(drawer.gnuplot, "    replot\n");
    fprintf(drawer.gnuplot, "}\n");
    fflush(drawer.gnuplot);
}

static capd::LDVector Newton(capd::LDVector u, capd::LDPoincareMap map) {
    capd::vectalg::MaxNorm<capd::LDVector, capd::LDMatrix> maxNorm;

    capd::LDMatrix Dphi(N + 1, N + 1);
    capd::LDVector P = map(u, Dphi);

    size_t i = 0;

    while (maxNorm(P - u) >= 10e-18) {
        u -= capd::matrixAlgorithms::gaussInverseMatrix(
            map.computeDP(P, Dphi) - capd::LDMatrix::Identity(N + 1)
        ) * (P - u);
        P = map(u, Dphi);
    }

    return u;
}

static capd::LDMatrix calcEigenVector(capd::LDVector u0, capd::LDPoincareMap map, capd::LDMap f) {
    capd::LDMatrix dPhi(N + 1, N + 1);

    capd::LDVector u1 = map(u0, dPhi);
    capd::LDMatrix dP = map.computeDP(u0, dPhi);

    capd::LDVector eigRe(N + 1);
    capd::LDVector eigIm(N + 1);
    capd::LDMatrix vectRe(N + 1, N + 1);
    capd::LDMatrix vectIm(N + 1, N + 1);

    capd::alglib::computeEigenvaluesAndEigenvectors(dP, eigRe, eigIm, vectRe, vectIm);

    capd::LDMatrix Res(N + 1, N + 1);
    capd::LDVector f_val = f(u0); 
    for(size_t i = 0; i <= N; ++i) {
        Res[i][0] = f_val[i];
    }

    size_t targetCol = 1;
    for (size_t srcCol = 0; srcCol <= N; ++srcCol) {
        if (srcCol != N - 1) {
            for (size_t row = 0; row <= N; ++row) {
                Res[row][targetCol] = vectRe[row][srcCol];
            }
            targetCol++;
        }
    }

    return Res;
}

static void drawInitialRectangle(capd::IVector v, struct gnuPlotManager *manager) {
    manager->print(0, "{} {}\n", v[1].leftBound(), v[2].leftBound());
    manager->print(0, "{} {}\n", v[1].rightBound(), v[2].leftBound());
    manager->print(0, "{} {}\n", v[1].rightBound(), v[2].rightBound());
    manager->print(0, "{} {}\n", v[1].leftBound(), v[2].rightBound());
    manager->print(0, "{} {}\n", v[1].leftBound(), v[2].leftBound());

    manager->print(0, "\n\n"); 
}

static void checkForCoveringRelationship(capd::LDVector u, capd::LDPoincareMap map, capd::IPoincareMap iMap, struct gnuPlotManager *manager, double side, capd::LDMap &f) {
    capd::LDVector fixed = Newton(u, map);
    std::cout.precision(16);
    COUT(fixed);
    capd::LDMatrix Re = calcEigenVector(fixed, map, f);

    capd::IMatrix iRe{Re};
    capd::IMatrix iRem = capd::matrixAlgorithms::gaussInverseMatrix(iRe);

    capd::IVector iFixed{fixed};
    capd::IVector iFixedL{fixed}; iFixedL -= (capd::IVector)iRe.column(1) * side;
    capd::IVector iFixedR{fixed}; iFixedR += (capd::IVector)iRe.column(1) * side;

    capd::IVector r(N + 1); {
        for (size_t i = 1; i < N + 1; i += 1) {
            r[i] = capd::Interval(-side, side);
        }
    }
    capd::IVector rL{r}; rL[1] = capd::Interval(-10e-10, 10e-10);
    capd::IVector rR{r}; rR[1] = capd::Interval(-10e-10, 10e-10);

    drawInitialRectangle(r, manager);
    drawInitialRectangle(rL, manager);
    drawInitialRectangle(rR, manager);
    COUT(iFixed);
    COUT(iFixedL);
    COUT(iFixedR);
    COUT(r);
    COUT(rL);
    COUT(rR);

    capd::C0Rect2Set x{iFixed, iRe, r};
    capd::C0Rect2Set xL{iFixedL, iRe, rL};
    capd::C0Rect2Set xR{iFixedR, iRe, rR};

    capd::DInterval tt;
    capd::IVector fx = iMap(x, iFixed, iRem, tt); drawInitialRectangle(fx, manager);
    
    tt = 0;
    std::cout << iMap.getSolver().getStep() << std::endl;
    capd::IVector fxL = iMap(xL, iFixed, iRem, tt); drawInitialRectangle(fxL, manager);

    tt = 0;
    std::cout << iMap.getSolver().getStep() << std::endl;
    capd::IVector fxR = iMap(xR, iFixed, iRem, tt); drawInitialRectangle(fxR, manager);
    COUT(fx);
    COUT(fxL);
    COUT(fxR);
}

int main() {
    class gnuPlotManager manager{{
        {
            .name = "Relacja Nakrywajaca",
            .file = "cRel.dat",

            .xName = "n",
            .yName = "s",

            .setGNUPlot = setGNUPlot,
        },
    }};

    constexpr double n = 8.97;
    constexpr uint32_t order = 20;
    capd::LDMap f{mackeyGlass<N>, N + 1, N + 1, 1};
    capd::LDOdeSolver solver{f, order}; {
        solver.setStep(0.01);
    }
    capd::LDCoordinateSection section{N + 1, 0, 0.6};
    capd::LDPoincareMap map{solver, section, capd::poincare::MinusPlus};

    capd::IMap iF{mackeyGlass<N>, N + 1, N + 1, 1};
    capd::IOdeSolver iSolver{iF, order}; {
        // iSolver.setStep(0.01);
    }
    capd::ICoordinateSection iSection{N + 1, 0, 0.6};
    capd::IPoincareMap iMap{iSolver, iSection, capd::poincare::MinusPlus};

    double side = 10e-6;
    capd::LDVector u(N + 1); {
        for (auto &e : u) {
            e = 1.1; 
        }
        u[0] = 0.6; 
    }

    // for (double n = 8.7; n < 9; n += 0.001) {
        f.setParameter(0, n);
        iF.setParameter(0, n);
        checkForCoveringRelationship(u, map, iMap, &manager, side, f);

        manager.fflush();
        manager.initGNUPlot();
    // }

    return 0;
}
// drugi Puancare
// Puancare map (set, Macierz, punkt (wektor))
// C0Rect2Set <<<<<------------ lepsze
// C1Rect2Set(x0, macierz współrzędnych, promień) <------- przekomplikowane
// Tripleton??
//
// Interwały
