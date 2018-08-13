// Copyright (c) 2018 radek
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include <hubero/core.hpp>
using namespace hubero;
using namespace hubero::mini;

#include "catch.hpp"

TEST_CASE("mini::Lit::Lit+static_cast<integral>")
{
    SECTION("no-arg initializes to value 0")
    {
        REQUIRE(static_cast<unsigned>(Lit()) == 0);
    }
    SECTION("direct-init non-casting")
    {
        REQUIRE(static_cast<unsigned>(Lit(13u)) == 13);
    }
    SECTION("variable and sign")
    {
        auto v0 = VarT<uint8_t>(0);
        auto l0f = LitT<uint8_t>(v0, false);
        auto l0t = LitT<uint8_t>(v0, true);

        auto v1 = VarT<uint8_t>(1);
        auto l1f = LitT<uint8_t>(v1, false);
        auto l1t = LitT<uint8_t>(v1, true);

        auto v2 = VarT<uint8_t>(2);
        auto l2f = LitT<uint8_t>(v2, false);
        auto l2t = LitT<uint8_t>(v2, true);

        REQUIRE(static_cast<uint8_t>(l0f) == 0);
        REQUIRE(static_cast<uint8_t>(l0t) == 1);
        REQUIRE(static_cast<uint8_t>(l1f) == 2);
        REQUIRE(static_cast<uint8_t>(l1t) == 3);
        REQUIRE(static_cast<uint8_t>(l2f) == 4);
        REQUIRE(static_cast<uint8_t>(l2t) == 5);

        REQUIRE_THROWS_AS(LitT<uint8_t>(Var(128), false), std::out_of_range);
        REQUIRE_THROWS_AS(LitT<uint8_t>(Var(128), true), std::out_of_range);
    }
    SECTION("copy-constructor")
    {
        Lit l1(11);
        Lit l2(l1);
        REQUIRE(static_cast<unsigned>(l2) == 11);
    }
    SECTION("move-constructor")
    {
        Lit l1(7);
        Lit l2(std::move(l1));
        REQUIRE(static_cast<unsigned>(l2) == 7);
    }
    SECTION("move-constructor")
    {
        Lit l1(7);
        Lit l2(std::move(l1));
        REQUIRE(static_cast<unsigned>(l2) == 7);
    }
    SECTION("direct-init casting constructor checks bounds")
    {
        REQUIRE_THROWS_AS(LitT<uint8_t>(-1),  std::out_of_range);
        REQUIRE(static_cast<uint8_t>(LitT<uint8_t>(0)) == 0);
        REQUIRE(static_cast<uint8_t>(LitT<uint8_t>(255)) == 255);
        REQUIRE_THROWS_AS(LitT<uint8_t>(256), std::out_of_range);
    }
    SECTION("type-modyfing constructor checks the bounds")
    {
        auto l0 = LitT<uint32_t>();
        auto l255 = LitT<uint32_t>(255);
        auto l256 = LitT<uint32_t>(256);

        REQUIRE(static_cast<uint8_t>(LitT<uint8_t>(l0)) == 0);
        REQUIRE(static_cast<uint8_t>(LitT<uint8_t>(l255)) == 255);
        REQUIRE_THROWS_AS(LitT<uint8_t>(l256), std::out_of_range);
    }
}

TEST_CASE("mini::Lit::operator=")
{
    SECTION("same type")
    {
        SECTION("copy-assignment")
        {
            const Lit l1(5);
            Lit l2;
            l2 = l1;
            REQUIRE(static_cast<unsigned>(l2) == 5);
        }
        SECTION("move-assignment")
        {
            const Lit l1(3);
            Lit l2;
            l2 = std::move(l1);
            REQUIRE(static_cast<unsigned>(l2) == 3);
        }
    }
    SECTION("different types")
    {
        SECTION("copy-assignment")
        {
            const LitT<uint16_t> l1(5);
            LitT<uint32_t> l2;
            l2 = l1;
            REQUIRE(static_cast<unsigned>(l2) == 5);
        }
        SECTION("move-assignment")
        {
            const LitT<uint16_t> l1(3);
            LitT<uint32_t> l2;
            l2 = std::move(l1);
            REQUIRE(static_cast<unsigned>(l2) == 3);
        }
    }
}



TEST_CASE("mini::Lit::operator==")
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

TEST_CASE("mini::Lit::operator!=")
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



TEST_CASE("mini::Lit::comparators")
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



TEST_CASE("mini::Lit::operator+") {
    const Lit l1(21);
    const Lit l2(23);
    REQUIRE(l1 + l2 == Lit(44));
}

TEST_CASE("mini::Lit::operator-") {
    const Lit l1(21);
    const Lit l2(23);
    REQUIRE(l2 - l1 == Lit(2));
}



TEST_CASE("mini::Lit::operator++")
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

TEST_CASE("mini::Lit::operator--")
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



TEST_CASE("mini::Lit::operator+=")
{
    Lit l1(43);
    const Lit l2(47);
    l1 += l2;
    REQUIRE(l1 == Lit(90));
}

TEST_CASE("mini::Lit::operator-=")
{
    Lit l1(41);
    const Lit l2(37);
    l1 -= l2;
    REQUIRE(l1 == Lit(4));
}



TEST_CASE("mini::Lit::static_cast<integral>")
{
    auto v = LitT<uint16_t>(300);
    REQUIRE(static_cast<uint32_t>(v) == 300u); // cast
    REQUIRE(static_cast<uint16_t>(v) == 300u); // no casting
    REQUIRE_THROWS_AS(static_cast<uint8_t>(v), std::out_of_range); // cast
}

TEST_CASE("mini::Lit::static_cast<Lit>")
{
    auto l0_32 = LitT<uint32_t>();
    auto l255_32 = LitT<uint32_t>(255u);
    auto l256_32 = LitT<uint32_t>(256u);

    auto l0_8 = static_cast<LitT<uint8_t>>(l0_32);
    REQUIRE(static_cast<LitT<uint8_t>>(l0_8) == LitT<uint8_t>(Var(0u), false));

    auto l255_8 = static_cast<LitT<uint8_t>>(l255_32);
    REQUIRE(static_cast<uint8_t>(l255_8) == 255u);

    REQUIRE_THROWS_AS(static_cast<LitT<uint8_t>>(l256_32), std::out_of_range);
}



TEST_CASE("mini::Lit::to_string")
{
    REQUIRE(Lit(Var(0), true).to_string() == "0");
    REQUIRE(Lit(Var(0), false).to_string() == "-0");
    REQUIRE(Lit(Var(31), true).to_string() == "31");
    REQUIRE(Lit(Var(31), false).to_string() == "-31");
}
