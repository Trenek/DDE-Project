#include <unistd.h>

#include "draw.hpp"

static void setGNUPlot(int id, struct thing &drawer) {
    fprintf(drawer.gnuplot, "set term qt %d size 800,600\n", id);
    fprintf(drawer.gnuplot, "set title '%s'\n", drawer.name);
    fprintf(drawer.gnuplot, "set xlabel '%s'\n", drawer.xName);
    fprintf(drawer.gnuplot, "set ylabel '%s'\n", drawer.yName);
    fprintf(drawer.gnuplot, "set pointsize 1.5\n");
    fprintf(drawer.gnuplot, "set grid\n");

    fprintf(drawer.gnuplot, "pause 1\n");
    fprintf(drawer.gnuplot, "plot \"%s\" with dots lc rgb '#000000' notitle\n", drawer.file);
    fprintf(drawer.gnuplot, "bind 'q' 'exit'\n");
    fprintf(drawer.gnuplot, "while (1) {\n");
    fprintf(drawer.gnuplot, "    pause 1\n");
    fprintf(drawer.gnuplot, "    replot\n");
    fprintf(drawer.gnuplot, "}\n");
    fflush(drawer.gnuplot);
}

void gnuPlotManager::fflush() {
    for (const auto &drawer : this->drawers) {
        std::fflush(drawer.dataFile);
    }
}

void gnuPlotManager::removeData() {
    for (const auto &drawer : this->drawers) {
        std::remove(drawer.file);
    }
}

void gnuPlotManager::initGNUPlot() {
    static bool isEnabled = false;
    size_t id = 0;

    if (isEnabled == false) {
        for (auto &drawer : this->drawers) {
            setGNUPlot(id++, drawer);
        }
    }

    isEnabled = true;
}

gnuPlotManager::gnuPlotManager(std::vector<struct thing> &&array) : drawers(std::move(array)) {
    removeData();

    sleep(4);

    for (auto &drawer : this->drawers) {
        drawer.dataFile = fopen(drawer.file, "w");
        drawer.gnuplot = popen("gnuplot", "w");
    }

    initGNUPlot();
}

gnuPlotManager::~gnuPlotManager() {
    for (auto &drawer : this->drawers) {
        fclose(drawer.dataFile);
        pclose(drawer.gnuplot);
    }
}
