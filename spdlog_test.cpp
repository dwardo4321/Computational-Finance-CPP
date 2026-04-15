#include <spdlog/spdlog.h>

int main() {
    spdlog::info("spdlog test started");
    spdlog::warn("This is a warning message");
    spdlog::error("This is a sample error message");
    spdlog::info("Value check: {}", 42);

    return 0;
}