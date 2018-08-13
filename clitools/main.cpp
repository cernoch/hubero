// Copyright (c) 2018 Radomír Černoch (radomir.cernoch@gmail.com)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <hubero/core.hpp>

#include <cstdint>
#include <iostream>
#include <utility>

using namespace hubero;
using namespace hubero::mini;

int main(int, char**)
{
    try {
        VarT<uint16_t> v(127);
        std::cout << static_cast<uint16_t>(v) << std::endl;
        VarT<uint8_t> w(v);
        std::cout << static_cast<uint16_t>(v)
           << " " << static_cast<uint16_t>(static_cast<uint8_t>(w)) << std::endl;

        LitT<uint16_t> l(w, true);
        std::cout << static_cast<uint16_t>(l) << std::endl;
        static_cast<void>(LitT<uint8_t>(l));

    } catch (std::out_of_range& oor) {
        std::cout << oor.what() << std::endl;
    }
}
