//
//  main.cpp
//  EmbindGenerator
//
//  Created by Zsolt Erhardt on 2018. 01. 16..
//  Copyright © 2018. Zsolt Erhardt. All rights reserved.
//

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wassume"
#include <cppast/libclang_parser.hpp>
#pragma clang diagnostic pop
#include <cxxopts.hpp>

#include "emgen/generators.h"

using namespace std::literals;

namespace program {
    static constexpr const char* const name = "emgen";
    static constexpr const char* const description = "An emscripten binding generator.";
    static constexpr const char* const version = "0.0.2";
}

int main(int argc, char* argv[]) {
    std::vector<std::string> filename;
    cxxopts::Options options(program::name,
                             program::name + " - "s + program::description + "\n"s);
    options.add_options()
        ("h,help", "display this help and exit")
        ("V,version", "display version information and exit")
        ("v,verbose", "be verbose when parsing")
        ("file", "file", cxxopts::value<std::vector<std::string>>(filename))
    ;
    options.add_options("compilation")
        ("d,dbpath", "set the directory where a 'compile_commands.json' file is located containing build information. Defaults to current directory.", cxxopts::value<std::string>()->default_value("."s))
        ("s,smart-pointer", "generate smart pointer with the given templated class", cxxopts::value<std::string>()->default_value(std::string{}))
    ;

    options.positional_help("file");

    options.parse_positional("file");
    options.parse(argc, argv);

    if (options.count("help")) {
        std::cout << options.help({"", "compilation"}) << std::endl;
        return 0;
    } else if (options.count("version")) {
        std::cout << program::name << " version " << program::version << std::endl;
        return 0;
    }
    else if (filename.empty())
    {
        std::cerr << "missing file argument" << std::endl;
        return 1;
    }

    cppast::libclang_compilation_database database { options["dbpath"].as<std::string>() };
    cppast::libclang_compile_config config { database, filename.front() };
    cppast::cpp_entity_index idx;

    auto&& logger = (options.count("verbose")) ? cppast::default_verbose_logger() : cppast::default_logger();

    cppast::libclang_parser parser { logger };
    auto&& file = parser.parse(idx, filename.front(), config);
    if (!file) {
        std::cerr << "Parsing failed" << std::endl;
        return 2;
    }

    std::cout << "EMSCRIPTEN_BINDINGS(Generated) {" << std::endl;

    emgen::generate_enums(*file);
    emgen::generate_classes(*file, options["smart-pointer"].as<std::string>());

    std::cout << "}" << std::endl;

    return 0;
}
