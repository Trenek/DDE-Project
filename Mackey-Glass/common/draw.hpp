#include <print>
#include <vector>

struct thing {
    FILE *fileDesc;
    const char *name;
    const char *file;
};

class gnuPlotManager {
public:
    FILE *traj;
    std::vector<struct thing> array;

    gnuPlotManager(std::vector<struct thing> &&array);
    ~gnuPlotManager();

    template <typename... Args>
    void print(size_t i, std::format_string<Args...> s, Args&&... args) {
        std::print(array[i].fileDesc, s, std::forward<Args>(args)...);
        std::fflush(array[i].fileDesc);
    }
};
