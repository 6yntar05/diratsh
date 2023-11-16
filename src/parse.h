#pragma once

#include <spawn.h>
#include <sys/wait.h>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <iostream>
#include <fstream>

#include "utils.h"

namespace script {
    void execScript(const char* PATH);
}

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
        if (vector.size() > 1)
            chdir(vector.at(1).c_str());
        else
            //chdir($HOME);
            return;
    else if ((vector.at(0) == "source") && (vector.size() > 1))
        script::execScript(vector.at(1).c_str());
    //else if (vector.at(0) == "export")
    //    std::setenv
    else
        commands::exec(std::move(vector));
}

} // namespace commands

namespace script {
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
}