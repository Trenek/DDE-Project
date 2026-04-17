#include <csignal>
#include <atomic>

static std::atomic<bool> cclose{false};

inline void signal_handler(int signal) {
    if (signal == SIGINT) {
        cclose = true;
    }
}
