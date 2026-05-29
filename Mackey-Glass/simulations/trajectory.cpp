#include <capd/capdlib.h>

#include "mackeyGlass.hpp"
#include "draw.hpp"

#define N 5
#define n 8.97

int main() {
    class gnuPlotManager manager{{
        {
            .name = "Trajektoria",
            .file = "traj.dat",

            .xName = "x0",
            .yName = "xN"
        }
    }, true};

    constexpr uint32_t order = 20;
    capd::LDMap f{mackeyGlass<N>, N + 1, N + 1, 1}; {
        f.setParameter(0, n);
    }
    capd::LDOdeSolver solver{f, order}; {
        solver.setStep(0.00001);
    }

    capd::LDVector u{0.6,0.5618695104336075,0.6204585444319234,0.7708628763257664,0.94131646849228,1.020675789800567};

    long double t = 0.0;

    manager.print(0, "{} {}\n", u[0], u[N]);
    manager.fflush();

    while (true) {
        u = solver(t, u);
        manager.print(0, "{} {}\n", u[0], u[N]);
    }

    return 0;
}
