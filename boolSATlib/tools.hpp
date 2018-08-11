// Copyright (c) 2018 Radomír Černoch (radomir.cernoch@gmail.com)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef BOOLSATLIB_TOOLS_H_
#define BOOLSATLIB_TOOLS_H_

#include <stdexcept>
#include <string>
#include <typeinfo>
#include <type_traits>

#if defined(__GNUC__)
// gcc + clang
#include <cxxabi.h>
#include <cstdlib>
#else // MSVC
#include <sstream>
#endif

namespace boolSAT {
namespace tools {

    template<class type>
    std::string type_to_string()
    {
#if defined(__GNUC__) // gcc + clang
        int status;
        char* name = abi::__cxa_demangle(
            typeid(type).name(),
            nullptr, 0, &status
        );

        if (status == 0) {
            if (name != nullptr) {
                std::string type_name(name);
                free(name);
                return type_name;
            } else {
                throw std::runtime_error(std::string(
                    "unknown error, abi::__cxa_demangle did not return the name"
                ));
            }

        } else if (status == -1) {
            throw std::bad_alloc();

        } else {
            throw std::runtime_error(std::string(
                "unknown error, abi::__cxa_demangle returned "
            ) + std::to_string(status) );
        }

#else // MSVC
        std::stringstream type_name_stream;
        type_name_stream << typeid(type).name();
        auto type_name_str = type_name_stream.str();
        
        // Try removing "class " from the beginning
        if (type_name_str.size() > 6 &&
            type_name_str.substr(0, 6) == "class ") {
            return type_name_str.substr(6);
        } else {
            return type_name_str;
        }
#endif
    } // type_to_string
} // tools
} // boolSAT
#endif // BOOLSATLIB_TOOLS_H_
