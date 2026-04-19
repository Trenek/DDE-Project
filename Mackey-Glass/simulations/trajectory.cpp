#include <capd/capdlib.h>

#include "mackeyGlass.hpp"
#include "draw.hpp"

#define N 5
#define n 9

int main() {
    class gnuPlotManager manager{{
        {
            .name = "Trajektoria",
            .file = "traj.dat",

            .xName = "x0",
            .yName = "xN"
        }
    }};

    constexpr uint32_t order = 20;
    capd::LDMap f{mackeyGlass<N>, N + 1, N + 1, 1}; {
        f.setParameter(0, n);
    }
    capd::LDOdeSolver solver{f, order}; {
        solver.setStep(0.1);
    }

    capd::LDVector u(N + 1); {
        for (auto &e : u) {
            e = 0.5;
        }
    }

    long double t = 0.0;

    manager.print(0, "{} {}\n", u[0], u[N]);
    manager.fflush();

    while (true) {
        u = solver(t, u);
        manager.print(0, "{} {}\n", u[0], u[N]);
    }

    return 0;
}
