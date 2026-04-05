#include <capd/capdlib.h>

#include "draw.hpp"

#define gamma 2
#define beta 4
#define N 6
#define k 1
#define n 9.65

capd::autodiff::Node f(capd::autodiff::Node x0, capd::autodiff::Node xN) {
    return - (gamma * x0) + ((beta * (xN ^ k)) / (1 + (xN ^ n)));
}

double x(double j) {
    return cos((j * std::numbers::pi) / N);
}

double c(double i) {
    return 1 + (i == 0) + (i == N);
}

std::array<std::array<double, N + 1>, N + 1> generateChebyshev() {
    std::array<std::array<double, N + 1>, N + 1> result;

    for (int i = 0; i < N + 1; i += 1) {
        for (int j = 0; j < N + 1; j += 1) {
            result[i][j] =
                (i == 0 && j == 0) ?   (2.0 * N * N + 1) / 6 :
                (i == N && j == N) ? - (2.0 * N * N + 1) / 6 :
                (i == j)           ? - x(i) / (2 * (1 - pow(x(i), 2))) :
                                       c(i) * pow(-1, i + j) / (c(j) * (x(i) - x(j)));
            result[i][j] *= 2;
        }
    }

    return result;
}

void applyChebyshev(capd::autodiff::Node in[], capd::autodiff::Node out[]) {
    static std::array<std::array<double, N + 1>, N + 1> M = generateChebyshev();

    for (size_t i = 1; i <= N; i += 1) {
        out[i] = M[i][0] * in[0];
        for (size_t j = 1; j <= N; j += 1) {
            out[i] += M[i][j] * in[j];
        }
    }
}

void mackeyGlass(
    capd::autodiff::Node &time,
    capd::autodiff::Node in[],
    int dimIn,
    capd::autodiff::Node out[],
    int dimOut,
    capd::autodiff::Node param[],
    int noParam
) {
    out[0] = f(in[0], in[dimIn - 1]);
    applyChebyshev(in, out);
}

int main() {
    capd::LDVector u{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

    class gnuPlotManager manager{{
        {
            .name = "Kąt między maksymalnymi wychyleniami",
            .file = "data.dat"
        }
    }};

    manager.print(0, "{} {}\n", u[0], u[N]);

    constexpr uint32_t order = 200;
    capd::LDMap f(mackeyGlass, N + 1, N + 1, 0);

    capd::LDOdeSolver solver{f, order};

    solver.setStep(0.001);

    long double t = 0.0;

    while (true) {
        u = solver(t, u);
        manager.print(0, "{} {}\n", u[0], u[N]);
    }

    return 0;
}
