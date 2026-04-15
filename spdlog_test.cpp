#include <spdlog/spdlog.h>

int main() {
    spdlog::info("spdlog test started");
    spdlog::warn("This is a warning message");
    spdlog::error("This is a sample error message");
    spdlog::info("Value check: {}", 42);

    return 0;
}

/*
C:/msys64/ucrt64/bin/g++.exe -std=c++20 -g spdlog_test.cpp -o spdlog_test.exe -lspdlog -lfmt
.\spdlog_test.exe
*/