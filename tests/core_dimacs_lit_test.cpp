// Copyright (c) 2018 radek
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <hubero/core.hpp>
using namespace hubero;
using namespace hubero::dimacs;

#include "catch.hpp"

TEST_CASE("dimacs::Lit::Lit+static_cast<integral>")
{
    SECTION("no-arg initializes to value 0")
    {
        REQUIRE(static_cast<int>(Lit()) == 0);
    }
    SECTION("direct-init non-casting")
    {
        REQUIRE(static_cast<int>(Lit(13)) == 13);
    }
    SECTION("variable and sign")
    {
        auto v0 = VarT<uint8_t>(0);
        auto l0f = LitT<int8_t>(v0, false);
        auto l0t = LitT<int8_t>(v0, true);

        auto v1 = VarT<uint8_t>(1);
        auto l1f = LitT<int8_t>(v1, false);
        auto l1t = LitT<int8_t>(v1, true);

        auto v2 = VarT<uint8_t>(2);
        auto l2f = LitT<int8_t>(v2, false);
        auto l2t = LitT<int8_t>(v2, true);

        REQUIRE(static_cast<int8_t>(l0f) == 0);
        REQUIRE(static_cast<int8_t>(l0t) == 0);
        REQUIRE(static_cast<int8_t>(l1f) == -1);
        REQUIRE(static_cast<int8_t>(l1t) == +1);
        REQUIRE(static_cast<int8_t>(l2f) == -2);
        REQUIRE(static_cast<int8_t>(l2t) == +2);

        REQUIRE_THROWS_AS(LitT<int8_t>(Var(128), false), std::out_of_range);
        REQUIRE_THROWS_AS(LitT<int8_t>(Var(128), true), std::out_of_range);
    }
    SECTION("copy-constructor")
    {
        Lit l1(11);
        Lit l2(l1);
        REQUIRE(static_cast<int>(l2) == 11);
    }
    SECTION("move-constructor")
    {
        Lit l1(7);
        Lit l2(std::move(l1));
        REQUIRE(static_cast<int>(l2) == 7);
    }
    SECTION("move-constructor")
    {
        Lit l1(7);
        Lit l2(std::move(l1));
        REQUIRE(static_cast<int>(l2) == 7);
    }
    SECTION("direct-init casting constructor checks bounds")
    {
        REQUIRE_THROWS_AS(LitT<int8_t>(-128), std::out_of_range);
        REQUIRE(static_cast<int8_t>(LitT<int8_t>(-127)) == -127);
        REQUIRE(static_cast<int8_t>(LitT<int8_t>(   0)) ==    0);
        REQUIRE(static_cast<int8_t>(LitT<int8_t>( 127)) ==  127);
        REQUIRE_THROWS_AS(LitT<int8_t>( 128), std::out_of_range);
    }
    SECTION("type-modyfing constructor checks the bounds")
    {
        auto l0 = LitT<int32_t>();
        auto l127 = LitT<int32_t>(127);
        auto l128 = LitT<int32_t>(128);

        REQUIRE(static_cast<int8_t>(LitT<int8_t>(l0)) == 0);
        REQUIRE(static_cast<int8_t>(LitT<int8_t>(l127)) == 127);
        REQUIRE_THROWS_AS(LitT<int8_t>(l128), std::out_of_range);
    }
}

TEST_CASE("dimacs::Lit::operator=")
{
    SECTION("same type")
    {
        SECTION("copy-assignment")
        {
            const Lit l1(5);
            Lit l2;
            l2 = l1;
            REQUIRE(static_cast<int>(l2) == 5);
        }
        SECTION("move-assignment")
        {
            const Lit l1(3);
            Lit l2;
            l2 = std::move(l1);
            REQUIRE(static_cast<int>(l2) == 3);
        }
    }
    SECTION("different types")
    {
        SECTION("copy-assignment")
        {
            const LitT<int16_t> l1(5);
            LitT<int32_t> l2;
            l2 = l1;
            REQUIRE(static_cast<int>(l2) == 5);
        }
        SECTION("move-assignment")
        {
            const LitT<int16_t> l1(3);
            LitT<int32_t> l2;
            l2 = std::move(l1);
            REQUIRE(static_cast<int>(l2) == 3);
        }
    }
}



TEST_CASE("dimacs::Lit::operator==")
{
    const auto l0 = Lit();
    const auto l1 = Lit(1);
    SECTION("positive")
    {
        REQUIRE(l0 == l0);
        REQUIRE(l1 == l1);
    }
    SECTION("negative")
    {
        REQUIRE(!(l0 == l1));
    }
}

TEST_CASE("dimacs::Lit::operator!=")
{
    const auto l0 = Lit();
    const auto l1 = Lit(1);
    SECTION("negative")
    {
        REQUIRE(!(l0 != l0));
        REQUIRE(!(l1 != l1));
    }
    SECTION("positive")
    {
        REQUIRE(l0 != l1);
    }
}



TEST_CASE("dimacs::Lit::comparators")
{
    const auto l07 = Lit(7);
    const auto l11 = Lit(11);
    const auto l13 = Lit(13);
    const auto l17 = Lit(17);
    const auto l19 = Lit(19);

    SECTION("operator<")
    {
        REQUIRE(l07 < l11);
        REQUIRE(l13 < l17);
        REQUIRE(!(l19 < l19));
    }
    SECTION("operator>")
    {
        REQUIRE(!(l07 > l11));
        REQUIRE(!(l13 > l17));
        REQUIRE(!(l19 > l19));
    }
    SECTION("operator<=")
    {
        REQUIRE(l07 <= l11);
        REQUIRE(l13 <= l17);
        REQUIRE(l19 <= l19);
    }
    SECTION("operator>=")
    {
        REQUIRE(!(l07 >= l11));
        REQUIRE(!(l13 >= l17));
        REQUIRE(l19 >= l19);
    }
}



TEST_CASE("dimacs::Lit::operator+") {
    const Lit l1(21);
    const Lit l2(23);
    REQUIRE(l1 + l2 == Lit(44));
}

TEST_CASE("dimacs::Lit::operator-") {
    const Lit l1(21);
    const Lit l2(23);
    REQUIRE(l2 - l1 == Lit(2));
}



TEST_CASE("dimacs::Lit::operator++")
{
    SECTION("pre-increment")
    {
        Lit l1(1);
        Lit l2(++l1);
        REQUIRE(l1 == Lit(2));
        REQUIRE(l2 == Lit(2));
    }
    SECTION("post-increment")
    {
        Lit l1(1);
        Lit l2(l1++);
        REQUIRE(l1 == Lit(2));
        REQUIRE(l2 == Lit(1));
    }
}

TEST_CASE("dimacs::Lit::operator--")
{
    SECTION("pre-decrement")
    {
        Lit l1(1);
        Lit l2(--l1);
        REQUIRE(l1 == Lit(0));
        REQUIRE(l2 == Lit(0));
    }
    SECTION("post-decrement")
    {
        Lit l1(1);
        Lit l2(l1--);
        REQUIRE(l1 == Lit(0));
        REQUIRE(l2 == Lit(1));
    }
}



TEST_CASE("dimacs::Lit::operator+=")
{
    Lit l1(43);
    const Lit l2(47);
    l1 += l2;
    REQUIRE(l1 == Lit(90));
}

TEST_CASE("dimacs::Lit::operator-=")
{
    Lit l1(41);
    const Lit l2(37);
    l1 -= l2;
    REQUIRE(l1 == Lit(4));
}



TEST_CASE("dimacs::Lit::static_cast<integral>")
{
    auto v = LitT<int16_t>(300);
    REQUIRE(static_cast<int32_t>(v) == 300); // cast
    REQUIRE(static_cast<int16_t>(v) == 300); // no casting
    REQUIRE_THROWS_AS(static_cast<int8_t>(v), std::out_of_range); // cast
}

TEST_CASE("dimacs::Lit::static_cast<Lit>")
{
    auto l0_32 = LitT<int32_t>();
    auto l127_32 = LitT<int32_t>(127);
    auto l128_32 = LitT<int32_t>(128);

    auto l0_8 = static_cast<LitT<int8_t>>(l0_32);
    REQUIRE(static_cast<LitT<int8_t>>(l0_8) == LitT<int8_t>(Var(0u), false));

    auto l127_8 = static_cast<LitT<int8_t>>(l127_32);
    REQUIRE(static_cast<int8_t>(l127_8) == 127);

    REQUIRE_THROWS_AS(static_cast<LitT<int8_t>>(l128_32), std::out_of_range);
}



TEST_CASE("dimacs::Lit::to_string")
{
    REQUIRE(Lit(Var(0), true).to_string() == "0");
    REQUIRE(Lit(Var(0), false).to_string() == "0");
    REQUIRE(Lit(Var(31), true).to_string() == "31");
    REQUIRE(Lit(Var(31), false).to_string() == "-31");
}
