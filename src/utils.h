#pragma once

#include <vector>
#include <string>
#include <filesystem>

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

inline std::string findExecutable(const std::string& cmd) {
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