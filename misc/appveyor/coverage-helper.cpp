#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <numeric>
#include <regex>
#include <string>
#include <vector>
#include <locale>


void create_empty_file(std::string const& path) {
    std::ofstream ofs(path);
    ofs << '\n';
}

const std::string separator = "--sep--";
const std::string logfile_prefix = "--log-file=";

bool starts_with(std::string const& str, std::string const& pref) {
    return str.find(pref) == 0;
}

int parse_log_file_arg(std::string const& arg) {
    assert(starts_with(arg, logfile_prefix) && "Attempting to parse incorrect arg!");
    auto fname = arg.substr(logfile_prefix.size());
    create_empty_file(fname);
    std::regex regex("MemoryChecker\\.(\\d+)\\.log", std::regex::icase);
    std::smatch match;
    if (std::regex_search(fname, match, regex)) {
        return std::stoi(match[1]);
    } else {
        throw std::domain_error("Couldn't find desired expression in string: " + fname);
    }
}

char simple_to_lower(char ch) {
    std::locale loc;
    return std::tolower(ch, loc);
}

std::string mks_path(std::string path) {
    return path;
    /*auto lower_case = path;
    std::transform(
        lower_case.begin(),
        lower_case.end(),
        lower_case.begin(),
        simple_to_lower
    );

    auto start = lower_case.find("src.hubero");
    if (start == std::string::npos) {
        throw std::domain_error("Couldn't find hubero's base path");
    }
    auto end = path.find_first_of("\\/", start);
    std::cerr << "Debug: " << path.substr(0, end) << std::endl;
    return path.substr(0, end);*/
}

std::string windowsify_path(std::string path) {
    for (auto& c : path) {
        if (c == '/') {
            c = '\\';
        }
    }
    return path;
}

void exec_cmd(std::string const& cmd, int log_num, std::string const& path) {
    std::array<char, 128> buffer;
#if defined(_WIN32)
    auto real_cmd = std::string("OpenCppCoverage")
        + " --export_type binary:cov-report" + std::to_string(log_num) + ".bin"
        + " --excluded_line_regex \"' *\\} *else *\\{ *(//.*)?$'\""
        + " --quiet"
        + " --sources " + path
        + " --cover_children"
        + " -- " + cmd;
    std::cerr << "=== Marker ===: Cmd: " << real_cmd << '\n';
    std::shared_ptr<FILE> pipe(_popen(real_cmd.c_str(), "r"), _pclose);
#else // Just for testing, in the real world we will always work under WIN32
    (void)log_num; (void)path;
    std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
#endif

    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
            std::cout << buffer.data();
        }
    }
}

// argv should be:
// [0]: our path
// [1]: "--log-file=<path>"
// [2]: "--sep--"
// [3]+: the actual command
int main(int argc, char** argv) {
    std::vector<std::string> args(argv, argv + argc);
    auto sep = std::find(begin(args), end(args), separator);
    assert(sep - begin(args) == 2 && "Structure differs from expected!");

    auto num = parse_log_file_arg(args[1]);

    auto cmdline = std::accumulate(
        ++sep,
        end(args),
        std::string{},
        [] (const std::string& lhs, const std::string& rhs) {
            return lhs + ' ' + rhs;
        }
    );

    try {
        return exec_cmd(cmdline, num, windowsify_path(mks_path(args[0])));
    } catch (std::exception const& ex) {
        std::cerr << "Helper failed with: '" << ex.what() << "'\n";
        return 12;
    }
}
