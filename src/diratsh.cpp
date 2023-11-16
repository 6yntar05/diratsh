#include <cstddef>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

#include "utils.h"
#include "parse.h"

constexpr char PS1[] = "~ ";

void signalHandler(int signum) {} // SIGINT is sent to child processes

int main(int argc, char* argv[]) {
    if (argc > 1) {
        script::execScript(argv[1]);
        return 0;
    }

    signal(SIGINT, signalHandler);

    std::string line;
    for (;;) {
        std::cout << PS1 << std::flush;
        std::getline(std::cin, line);
        if (line.size())
            commands::handle(line);
        else if (std::cin.eof())
            return 0;
    }
    return 0;
}
