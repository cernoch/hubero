// Copyright (c) 2018 radek
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <boolSATlib/core.hpp>
using namespace boolSAT;

#include "catch.hpp"

TEST_CASE("Var::Var+static_cast<integral>")
{
    SECTION("no-arg initializes to value 0")
    {
        REQUIRE(static_cast<unsigned>(Var()) == 0);
    }
    SECTION("direct-init")
    {
        REQUIRE(static_cast<unsigned>(Var(13u)) == 13);
    }
    SECTION("copy-constructor")
    {
        Var v1(11);
        Var v2(v1);
        REQUIRE(static_cast<unsigned>(v2) == 11);
    }
    SECTION("move-constructor")
    {
        Var v1(7);
        Var v2(std::move(v1));
        REQUIRE(static_cast<unsigned>(v2) == 7);
    }
    SECTION("move-constructor")
    {
        Var v1(7);
        Var v2(std::move(v1));
        REQUIRE(static_cast<unsigned>(v2) == 7);
    }
    SECTION("casting constructor checks bounds")
    {
        REQUIRE_THROWS_AS(VarT<uint8_t>(-1),  std::out_of_range);
        REQUIRE(static_cast<uint8_t>(VarT<uint8_t>(0)) == 0);
        REQUIRE(static_cast<uint8_t>(VarT<uint8_t>(255)) == 255);
        REQUIRE_THROWS_AS(VarT<uint8_t>(256), std::out_of_range);
    }
    SECTION("type-modyfing constructor checks the bounds")
    {
        auto v0 = VarT<uint32_t>();
        auto v255 = VarT<uint32_t>(255);
        auto v256 = VarT<uint32_t>(256);

        REQUIRE(static_cast<uint8_t>(VarT<uint8_t>(v0)) == 0);
        REQUIRE(static_cast<uint8_t>(VarT<uint8_t>(v255)) == 255);
        REQUIRE_THROWS_AS(VarT<uint8_t>(v256), std::out_of_range);
    }
}

TEST_CASE("Var::operator=")
{
    SECTION("same type")
    {
        SECTION("copy-assignment")
        {
            const Var v1(5);
            Var v2;
            v2 = v1;
            REQUIRE(static_cast<unsigned>(v2) == 5);
        }
        SECTION("move-assignment")
        {
            const Var v1(3);
            Var v2;
            v2 = std::move(v1);
            REQUIRE(static_cast<unsigned>(v2) == 3);
        }
    }
    SECTION("different types")
    {
        SECTION("copy-assignment")
        {
            const VarT<uint16_t> v1(5);
            VarT<uint32_t> v2;
            v2 = v1;
            REQUIRE(static_cast<unsigned>(v2) == 5);
        }
        SECTION("move-assignment")
        {
            const VarT<uint16_t> v1(3);
            VarT<uint32_t> v2;
            v2 = std::move(v1);
            REQUIRE(static_cast<unsigned>(v2) == 3);
        }
    }
}



TEST_CASE("Var::operator==")
{
    const auto v0 = Var();
    const auto v1 = Var(1);
    SECTION("positive")
    {
        REQUIRE(v0 == v0);
        REQUIRE(v1 == v1);
    }
    SECTION("negative")
    {
        REQUIRE(!(v0 == v1));
    }
}

TEST_CASE("Var::operator!=")
{
    const auto v0 = Var();
    const auto v1 = Var(1);
    SECTION("negative")
    {
        REQUIRE(!(v0 != v0));
        REQUIRE(!(v1 != v1));
    }
    SECTION("positive")
    {
        REQUIRE(v0 != v1);
    }
}



TEST_CASE("Var::comparators")
{
    const auto v07 = Var(7);
    const auto v11 = Var(11);
    const auto v13 = Var(13);
    const auto v17 = Var(17);
    const auto v19 = Var(19);

    SECTION("operator<")
    {
        REQUIRE(v07 < v11);
        REQUIRE(v13 < v17);
        REQUIRE(!(v19 < v19));
    }
    SECTION("operator>")
    {
        REQUIRE(!(v07 > v11));
        REQUIRE(!(v13 > v17));
        REQUIRE(!(v19 > v19));
    }
    SECTION("operator<=")
    {
        REQUIRE(v07 <= v11);
        REQUIRE(v13 <= v17);
        REQUIRE(v19 <= v19);
    }
    SECTION("operator>=")
    {
        REQUIRE(!(v07 >= v11));
        REQUIRE(!(v13 >= v17));
        REQUIRE(v19 >= v19);
    }
}



TEST_CASE("Var::operator+") {
    const Var v1(21);
    const Var v2(23);
    REQUIRE(v1 + v2 == Var(44));
}

TEST_CASE("Var::operator-") {
    const Var v1(21);
    const Var v2(23);
    REQUIRE(v2 - v1 == Var(2));
}



TEST_CASE("Var::operator++")
{
    SECTION("pre-increment")
    {
        Var v1(1);
        Var v2(++v1);
        REQUIRE(v1 == Var(2));
        REQUIRE(v2 == Var(2));
    }
    SECTION("post-increment")
    {
        Var v1(1);
        Var v2(v1++);
        REQUIRE(v1 == Var(2));
        REQUIRE(v2 == Var(1));
    }
}

TEST_CASE("Var::operator--")
{
    SECTION("pre-decrement")
    {
        Var v1(1);
        Var v2(--v1);
        REQUIRE(v1 == Var(0));
        REQUIRE(v2 == Var(0));
    }
    SECTION("post-decrement")
    {
        Var v1(1);
        Var v2(v1--);
        REQUIRE(v1 == Var(0));
        REQUIRE(v2 == Var(1));
    }
}



TEST_CASE("Var::operator+=")
{
    Var v1(43);
    const Var v2(47);
    v1 += v2;
    REQUIRE(v1 == Var(90));
}

TEST_CASE("Var::operator-=")
{
    Var v1(41);
    const Var v2(37);
    v1 -= v2;
    REQUIRE(v1 == Var(4));
}

TEST_CASE("Var::static_cast<Var>")
{
    auto v0_32 = VarT<uint32_t>();
    auto v255_32 = VarT<uint32_t>(255u);
    auto v256_32 = VarT<uint32_t>(256u);

    auto v0_8 = static_cast<VarT<uint8_t>>(v0_32);
    REQUIRE(static_cast<uint8_t>(v0_8) == 0u);

    auto v255_8 = static_cast<VarT<uint8_t>>(v255_32);
    REQUIRE(static_cast<uint8_t>(v255_8) == 255u);

    REQUIRE_THROWS_AS(static_cast<VarT<uint8_t>>(v256_32), std::out_of_range);
}



TEST_CASE("Var::to_string")
{
    REQUIRE(Var().to_string() == "0");
    REQUIRE(Var(1).to_string() == "1");
    REQUIRE(Var(31).to_string() == "31");
}
