#include <cstddef>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <spawn.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <algorithm>

constexpr char PS1[] = "~ ";
void execScript(const char* PATH);

namespace utils {

    inline std::vector<std::string> split(std::string src, char by) {
        std::vector<std::string> vector;
        while (src.size()) {
            size_t begin = 0, end = src.find(by);
            vector.push_back(src.substr(begin, end));
            if (src.size() > end)
                src = src.substr(end + 1, src.size());
            else
                break;
        }

        return vector;
    }

    inline std::vector<std::string> getPath() {
        return split(std::getenv("PATH"), ':');
    }

    std::string findExecutable(const std::string& cmd) {
        if (cmd.front() == '/')
            return cmd;

        auto PATH = getPath();
        for (const auto& dir : PATH) {
            std::string full_path = dir + '/' + cmd;
            if (std::filesystem::exists(full_path)) {
                return full_path;
            }
        }
        return cmd;
    }

} // namespace utils

namespace commands {

    inline void exec(const std::vector<std::string>& vector) {
        std::string cmd = utils::findExecutable(vector[0]);

        std::vector<char*> args;
        for (const auto& arg : vector) {
            args.push_back(const_cast<char*>(arg.c_str()));
        }
        args.push_back(nullptr);
        args[0] = const_cast<char*>(cmd.c_str());

        pid_t pid;
        posix_spawn_file_actions_t file_actions;
        posix_spawn_file_actions_init(&file_actions);
        if (posix_spawn(&pid, args[0], &file_actions, nullptr, args.data(), environ) == 0) {
            int status;
            waitpid(pid, &status, 0);
        } else {
            perror("posix_spawn");
        }
        posix_spawn_file_actions_destroy(&file_actions);
    }

    void handle(std::string line) {
        // trim string
        line.erase(line.begin(), std::find_if_not(line.begin(), line.end(), ::isspace));
        // check for comment
        if (line.at(0) == '#')
            return;
        // split by args
        auto vector = utils::split(line, ' ');

        if (vector.at(0) == "clear")
            std::cout << "\ec" << std::flush;
        else if (vector.at(0) == "exit")
            std::exit(0);
        else if (vector.at(0) == "cd")
            chdir(vector.at(1).c_str());
        else if ((vector.at(0) == "source") && (vector.size() > 1))
            execScript(vector.at(1).c_str());
        else
            commands::exec(std::move(vector));
    }

} // namespace commands

void execScript(const char* PATH) {
    if (!std::filesystem::exists(PATH)) {
        std::cerr << "No such file" << std::endl;
        return;
    }
    std::ifstream scriptFile { PATH };
    std::string line;
    while (!scriptFile.eof()) {
        std::getline(scriptFile, line);
        if (line.size())
            commands::handle(line);
    }
}

void signalHandler(int signum) {} // SIGINT is sent to child processes

int main(int argc, char* argv[]) {
    if (argc > 1) {
        execScript(argv[1]);
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
