#include <capd/capdlib.h>

#include "mackeyGlass.hpp"
#include "draw.hpp"

#define N 5
#define n 8.7

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

static void setGNUPlot(int id, struct thing &drawer) {
    fprintf(drawer.gnuplot, "set term qt %d size 800,600\n", id);
    fprintf(drawer.gnuplot, "set title '%s'\n", drawer.name);
    fprintf(drawer.gnuplot, "set xlabel '%s'\n", drawer.xName);
    fprintf(drawer.gnuplot, "set ylabel '%s'\n", drawer.yName);
    fprintf(drawer.gnuplot, "set pointsize 1.5\n");
    fprintf(drawer.gnuplot, "set grid\n");

    fprintf(drawer.gnuplot, "pause 1\n");

    fprintf(drawer.gnuplot, "plot \"%s\" index 0 with lines lw 2 lc rgb 'red' title 'f(X)', \\\n", drawer.file);
    fprintf(drawer.gnuplot, "     \"%s\" index 1 with lines lw 2 lc rgb 'blue' title 'X'\n", drawer.file);

    fprintf(drawer.gnuplot, "bind 'q' 'exit'\n");
    fprintf(drawer.gnuplot, "while (1) {\n");
    fprintf(drawer.gnuplot, "    pause 1\n");
    fprintf(drawer.gnuplot, "    replot\n");
    fprintf(drawer.gnuplot, "}\n");
    fflush(drawer.gnuplot);
}

capd::LDVector Newton(capd::LDVector u, capd::LDPoincareMap map) {
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

capd::LDMatrix calcEigenVector(capd::LDVector u0, capd::LDPoincareMap map) {
    capd::LDMatrix dPhi(N + 1, N + 1);

    capd::LDVector u1 = map(u0, dPhi);
    capd::LDMatrix dP = map.computeDP(u0, dPhi);

    capd::LDVector eigRe(N + 1);
    capd::LDVector eigIm(N + 1);
    capd::LDMatrix vectRe(N + 1, N + 1);
    capd::LDMatrix vectIm(N + 1, N + 1);

    capd::alglib::computeEigenvaluesAndEigenvectors(dP, eigRe, eigIm, vectRe, vectIm);

    return capd::matrixAlgorithms::gaussInverseMatrix(vectRe) * dP * vectRe;
}

void drawBorder(double b0, double b1, double e0, double e1, capd::LDMatrix A, struct gnuPlotManager *manager) {
    capd::LDVector v(N + 1);
    capd::LDVector img;

    for (double t = 0; t <= 1.0; t += 0.05) {
        v[0] = b0 + t * (e0 - b0);
        v[1] = b1 + t * (e1 - b1);
        
        img = A * v;
        manager->print(0, "{} {}\n", img[0], img[1]);
    }
};

void checkForCoveringRelationship(capd::LDVector u, capd::LDPoincareMap map, struct gnuPlotManager *manager) {
    double side = 10e-20;
    capd::LDVector l(N + 1); {
        l[0] = -side; 
    }
    capd::LDVector r(N + 1); {
        r[0] = side;
    }

    capd::LDMatrix A = calcEigenVector(Newton(u, map), map);

    drawBorder(-side, -side, side, -side, A, manager);
    drawBorder(side, -side, side, side, A, manager);
    drawBorder(side, side, -side, side, A, manager);
    drawBorder(-side, side, -side, -side, A, manager);

    manager->print(0, "\n\n"); 
    manager->print(0, "{} {}\n", -side, -side);
    manager->print(0, "{} {}\n", side, -side);
    manager->print(0, "{} {}\n", side, side);
    manager->print(0, "{} {}\n", -side, side);
    manager->print(0, "{} {}\n", -side, -side);
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

    constexpr uint32_t order = 20;
    capd::LDMap f{mackeyGlass<N>, N + 1, N + 1, 1};
    capd::LDOdeSolver solver{f, order}; {
        solver.setStep(0.01);
    }
    capd::LDCoordinateSection section{N + 1, 0, 0.6};
    capd::LDPoincareMap map{solver, section, capd::poincare::MinusPlus};

    capd::LDVector u(N + 1); {
        for (auto &e : u) {
            e = 1.1; 
        }
        u[0] = 0.6; 
    }

    f.setParameter(0, n);

    checkForCoveringRelationship(u, map, &manager);

    manager.fflush();
    manager.initGNUPlot();

    return 0;
}
