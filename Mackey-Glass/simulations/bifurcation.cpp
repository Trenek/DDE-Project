#include <capd/capdlib.h>

#include "mackeyGlass.hpp"
#include "draw.hpp"

#define N 5

int main() {
    class gnuPlotManager manager{{
        {
            .name = "Bifurkacja",
            .file = "bif.dat",

            .xName = "n",
            .yName = "xN"
        }
    }};

    constexpr uint32_t order = 20;
    long double n = 8.7;
    capd::LDMap f{mackeyGlass<N>, N + 1, N + 1, 1}; {
        f.setParameter(0, n);
    }
    capd::LDOdeSolver solver{f, order}; {
        solver.setStep(0.1);
    }
    capd::LDCoordinateSection section{N + 1, 0, 0.6}; // alternatywa - capd::LDAffineSection
    capd::LDPoincareMap map{solver, section, capd::poincare::MinusPlus};
    capd::LDTimeMap timeMap{solver};

    capd::LDVector u(N + 1); {
        for (auto &e : u) {
            e = 1.1;
        }
    }

    capd::LDVector temp{map(timeMap(100, u))};

    manager.print(0, "{} {}\n", n, temp[N]);
    manager.fflush();

    // Stała Feigenbauma, (Golden Ratio + Feigenbaum Constant) / 2 = pi
    while (n <= 9) {
        temp = timeMap(100, u);

        for (int i = 0; i < 200; i++) {
            temp = map(temp);

            manager.print(0, "{} {}\n", n, temp[N]);
        }

        f.setParameter(0, n);
        n += 0.001;
    }

    return 0;
}
