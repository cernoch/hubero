// Copyright (c) 2018 Radomír Černoch (radomir.cernoch@gmail.com)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef HUBERO_CORE_H_
#define HUBERO_CORE_H_

#include <hubero/tools.hpp>

#include <cassert>
#include <string>
#include <limits>

#if __cplusplus > 201703L
    // C++17
    #define HUBERO_IF_CONSTEXPR(x) if constexpr (x)
#else
    #if defined(_MSC_VER)
        // pre-C++17 MSVC
        #define HUBERO_IF_CONSTEXPR(x) __pragma(warning(push)) __pragma(warning(disable:4127)) if (x) __pragma(warning(pop))
    #else
        // pre-C++17 GCC/clang
        #define HUBERO_IF_CONSTEXPR(x) if (x)
    #endif
#endif


namespace hubero {

template<
    class T,
    T MAX = std::numeric_limits<T>::max() / 2
>
class VarT {

    static_assert(std::is_unsigned<T>::value,
        "Variable's underlying type must be unsigned integral type.");

    T var_id;

public:

    VarT() : var_id(0) {}

    template<class U>
    explicit VarT(const U& id)
    : var_id(static_cast<T>(id))
    {
        static_assert(std::is_integral<U>::value,
            "Variable can be created only from integral types.");

        // compile-time check-avoider speeds-up the Debug-mode
        HUBERO_IF_CONSTEXPR (std::is_signed<U>::value) {
            if (id < 0) { // run-time bounds-check
                throw std::out_of_range(std::string(
                    "Variable can represent non-negative values, but " )
                        + std::to_string(id) + " was given." );
            }
        }

        // compile-time check-avoider speeds-up the Debug-mode
        HUBERO_IF_CONSTEXPR (MAX < std::numeric_limits<U>::max()) {
            // make compilers happy about signed/unsigned comparison in the if(...)
            auto unsigned_id = static_cast<typename std::make_unsigned<U>::type>(id);

             // run-time bounds-check
            if (MAX < unsigned_id) {
                throw std::out_of_range(std::string(
                    "Variable can represent values 0.." ) + std::to_string(MAX)
                    + ", but " + std::to_string(id) + " was given." );
            }
        }
    }



    template<class U, U U_MAX>
    explicit VarT(const VarT<U,U_MAX>& var)
    : var_id(static_cast<T>(static_cast<U>(var)))
    {
        // compile-time check-avoider speeds-up the Debug-mode
        HUBERO_IF_CONSTEXPR (MAX < U_MAX) {
             // run-time bounds-check
            if (MAX < static_cast<U>(var)) {
                throw std::out_of_range(std::string(
                    "Variable can represent values 0.." ) + std::to_string(MAX)
                    + ", but " + std::to_string(static_cast<U>(var)) + " was given." );
            }
        }
    }



    template<class U, U U_MAX>
    VarT& operator =(const VarT<U,U_MAX>& prototype)
    {
        // bound-check is done in the casting
        var_id = static_cast<T>(prototype);
        return *this;
    }




    bool operator ==(const VarT<T>& rhs) const
    {
        return var_id == rhs.var_id;
    }

    bool operator !=(const VarT<T>& rhs) const
    {
        return var_id != rhs.var_id;
    }



    bool operator <(const VarT<T>& rhs) const
    {
        return var_id < rhs.var_id;
    }

    bool operator >(const VarT<T>& rhs) const
    {
        return var_id > rhs.var_id;
    }

    bool operator <=(const VarT<T>& rhs) const
    {
        return var_id <= rhs.var_id;
    }

    bool operator >=(const VarT<T>& rhs) const
    {
        return var_id >= rhs.var_id;
    }



    VarT<T,MAX> operator +(const VarT<T,MAX>& rhs) const
    {
        T result = var_id + rhs.var_id;
        assert(result >= var_id && "Variable overflow detected in operator +");
        assert(result >= rhs.var_id && "Variable overflow detected in operator +");
        return VarT<T,MAX>(result);
    }

    VarT<T,MAX>& operator ++()
    {
        assert(var_id < MAX &&
            "Variable overflow detected in"
            " the pre-increment operator");
        ++var_id;
        return *this;
    }

    VarT<T,MAX> operator ++(int)
    {
        assert(var_id < MAX &&
            "Variable overflow detected in"
            " the post-increment operator");
        auto copy = *this;
        ++var_id;
        return copy;
    }

    VarT<T,MAX>& operator +=(const VarT<T,MAX>& rhs)
    {
#if !defined(NDEBUG)
        T before = var_id;
#endif
        var_id += rhs.var_id;

        assert(var_id <= MAX &&
            "Variable exceeded MAX in operator +=");
        assert(before < var_id && rhs.var_id < var_id &&
            "Variable overflow detected in operator +=");
        return *this;
    }



    VarT<T,MAX> operator -(const VarT<T,MAX>& rhs) const
    {
        T result = var_id - rhs.var_id;
        assert(result <= var_id && "Variable overflow detected in operator -");
        return VarT(result);
    }

    VarT<T,MAX>& operator --()
    {
        assert(var_id > 0 &&
            "Variable overflow detected in"
            " the pre-decrement operator");
        --var_id;
        return *this;
    }

    VarT<T,MAX> operator --(int)
    {
        assert(var_id > 0 &&
            "Variable overflow detected in"
            " the post-decrement operator");
        auto copy = *this;
        --var_id;
        return copy;
    }

    VarT<T,MAX>& operator -=(const VarT<T,MAX>& rhs)
    {
#if !defined(NDEBUG)
        T before = var_id;
#endif
        var_id -= rhs.var_id;
        assert(before > var_id &&
            "Variable overflow detected in operator -=");
        return *this;
    }

    // conversions

    template<class U, U U_MAX>
    explicit operator VarT<U,U_MAX>() const
    {
        // bounds-check is done in the constructor
        return VarT<U,U_MAX>(var_id);
    }

    template<class U>
    explicit operator U() const
    {
        static_assert(std::is_integral<U>::value,
            "Variable can be casted only to integral types.");

        // compile-time check-avoider speeds-up the Debug-mode
        HUBERO_IF_CONSTEXPR (MAX > std::numeric_limits<U>::max()) {
            // run-time bounds-check
            if (var_id > std::numeric_limits<U>::max()) {
                throw std::out_of_range(std::string("Variable ")
                    + std::to_string(var_id) + " is too big to be casted to "
                    + tools::type_to_string<U>() + "." );
            }
        }

        return static_cast<U>(var_id);
    }

    std::string to_string() const
    {
        return std::to_string(var_id);
    }

}; // VarT

using Var = VarT<unsigned>;



namespace mini {

template<
    class T,
    T MAX = std::numeric_limits<T>::max()
>
class LitT {

    static_assert(std::is_unsigned<T>::value,
        "Literal's underlying type must be unsigned integral type.");

    static_assert(MAX % 2 == 1,
        "Literal's max-value must not be divisible by 2.");
    // Otherwise, the true-literal of the largest variable would NOT be
    // representable by this class, yet the false-literal would be.

    T lit_id;

public:

    LitT()
    : lit_id(0)
    {}

    template<class U>
    explicit LitT(const U& id)
    : lit_id(static_cast<T>(id))
    {
        static_assert(std::is_integral<U>::value,
            "Literal can be created only from integral types.");

        // compile-time check-avoider speeds-up the Debug-mode
        HUBERO_IF_CONSTEXPR (std::is_signed<U>::value) {
            if (id < 0) { // run-time bounds-check
                throw std::out_of_range(std::string(
                    "Literal can represent non-negative values, but " )
                        + std::to_string(id) + " was given." );
            }
        }

        // compile-time check-avoider speeds-up the Debug-mode
        HUBERO_IF_CONSTEXPR (MAX < std::numeric_limits<U>::max()) {
            // make compilers happy about signed/unsigned comparison in the if(...)
            auto unsigned_id = static_cast<typename std::make_unsigned<U>::type>(id);

             // run-time bounds-check
            if (MAX < unsigned_id) {
                throw std::out_of_range(std::string(
                    "Literal can represent values 0.." ) + std::to_string(MAX)
                    + ", but " + std::to_string(id) + " was given." );
            }
        }
    }

    template<class U, U U_MAX>
    LitT(const LitT<U, U_MAX>& lit)
    : lit_id(static_cast<T>(static_cast<U>(lit)))
    {
        // compile-time check-avoider speeds-up the Debug-mode
        HUBERO_IF_CONSTEXPR (MAX < U_MAX) {
            // run-time bounds-check
            if (MAX < static_cast<U>(lit)) {
                throw std::out_of_range(std::string(
                    "Literal can represent values 0.." ) + std::to_string(MAX)
                    + ", but " + std::to_string(static_cast<U>(lit)) + " was given." );
            }
        }
    }

    template<class U, U U_MAX>
    LitT(VarT<U,U_MAX> var, bool sign)
    : lit_id(2 * static_cast<T>(static_cast<U>(var)) + (sign ? 1u : 0u))
    {
        // compile-time check-avoider speeds-up the Debug-mode
        HUBERO_IF_CONSTEXPR (MAX < 2 * U_MAX) {
             // run-time bounds-check
            if (MAX < 2 * static_cast<U>(var)) {
                throw std::out_of_range(std::string(
                    "Literal can represent variables 0.." ) + std::to_string(MAX)
                    + ", but " + std::to_string(static_cast<U>(var)) + " was given." );
            }
        }
    }

    bool sign() const
    {
        return lit_id & 1;
    }

    VarT<T,MAX> var() const
    {
        return VarT<T,MAX>(lit_id / 2);
    }

    LitT<T,MAX> operator ~() const
    {
        return LitT<T,MAX>(lit_id ^ 1u);
    }

    LitT<T,MAX> operator ^(bool sign) const
    {
        return LitT<T,MAX>(lit_id ^ (sign ? 1u : 0u));
    }

    // integer-type conversions

    bool operator ==(const LitT<T,MAX>& rhs) const
    {
        return lit_id == rhs.lit_id;
    }

    bool operator !=(const LitT<T,MAX>& rhs) const
    {
        return lit_id != rhs.lit_id;
    }



    bool operator <(const LitT<T,MAX>& rhs) const
    {
        return lit_id < rhs.lit_id;
    }

    bool operator >(const LitT<T,MAX>& rhs) const
    {
        return lit_id > rhs.lit_id;
    }

    bool operator <=(const LitT<T,MAX>& rhs) const
    {
        return lit_id <= rhs.lit_id;
    }

    bool operator >=(const LitT<T,MAX>& rhs) const
    {
        return lit_id >= rhs.lit_id;
    }



    LitT<T,MAX> operator +(const LitT<T,MAX>& rhs) const
    {
        T result = lit_id + rhs.lit_id;
        assert(result >= lit_id && "Literal overflow detected in operator +");
        assert(result >= rhs.lit_id && "Literal overflow detected in operator +");
        return LitT<T,MAX>(result);
    }

    LitT<T,MAX>& operator ++()
    {
        assert(lit_id < MAX &&
            "Literal overflow detected in"
            " the pre-increment operator");
        ++lit_id;
        return *this;
    }

    LitT<T,MAX> operator ++(int)
    {
        assert(lit_id < MAX &&
            "Literal overflow detected in"
            " the post-increment operator");
        auto copy = *this;
        ++lit_id;
        return copy;
    }

    LitT<T,MAX>& operator +=(const LitT<T,MAX>& rhs)
    {
#if !defined(NDEBUG)
        T before = lit_id;
#endif
        lit_id += rhs.lit_id;

        assert(lit_id <= MAX &&
            "Literal exceeded MAX in operator +=");
        assert(before < lit_id && rhs.lit_id < lit_id &&
            "Literal overflow detected in operator +=");
        return *this;
    }



    LitT<T,MAX> operator -(const LitT<T,MAX>& rhs) const
    {
        T result = lit_id - rhs.lit_id;
        assert(result <= lit_id && "Literal overflow detected in operator -");
        return LitT<T,MAX>(result);
    }

    LitT<T,MAX>& operator --()
    {
        assert(lit_id > 0 &&
            "Literal overflow detected in"
            " the pre-decrement operator");
        --lit_id;
        return *this;
    }

    LitT<T,MAX> operator --(int)
    {
        assert(lit_id > 0 &&
            "Literal overflow detected in"
            " the post-decrement operator");
        auto copy = *this;
        --lit_id;
        return copy;
    }

    LitT<T,MAX>& operator -=(const LitT<T,MAX>& rhs)
    {
#if !defined(NDEBUG)
        T before = lit_id;
#endif
        lit_id -= rhs.lit_id;
        assert(before > lit_id &&
            "Literal overflow detected in operator -=");
        return *this;
    }

    // conversions

    template<class U, U U_MAX>
    explicit operator LitT<U, U_MAX>() const
    {
        // bounds-check is done in the constructor
        return LitT<U, U_MAX>(lit_id);
    }

    template<class U>
    explicit operator U() const
    {
        static_assert(std::is_integral<U>::value,
            "Literal can be casted only to integral types.");

        // compile-time check-avoider speeds-up the Debug-mode
        HUBERO_IF_CONSTEXPR (MAX > std::numeric_limits<U>::max()) {
            // run-time bounds-check
            if (lit_id > std::numeric_limits<U>::max()) {
                throw std::out_of_range(std::string("Literal ")
                    + std::to_string(lit_id) + " is too big to be casted to "
                    + tools::type_to_string<U>() + "." );
            }
        }

        return static_cast<U>(lit_id);
    }

    std::string to_string() const
    {
        std::string retval;
        if (!sign()) {
            retval += "-";
        }

        retval += std::to_string(static_cast<T>(var()));
        return retval;
    }
}; // LitT

using Lit = LitT<unsigned>;

} // mini

namespace dimacs {

template<
    class T,
    T MAX = std::numeric_limits<T>::max()
>
class LitT {

    static_assert(std::is_signed<T>::value,
        "Literal's underlying type must be a signed integral type.");

    T lit_id;

    using UNSIGNED_T = typename std::make_unsigned<T>::type;

public:

    LitT()
    : lit_id(0)
    {}

    template<class U>
    explicit LitT(const U& id)
    : lit_id(static_cast<T>(id))
    {
        static_assert(std::is_integral<U>::value,
            "Literal can be created only from integral types.");

        // signed-safe compile-time check-avoider speeds-up the Debug-mode
        HUBERO_IF_CONSTEXPR (std::is_signed<U>::value) {
            auto minus_min = -std::numeric_limits<U>::min();
            using UNSIGN_U = typename std::make_unsigned<U>::type;
            auto unsign_mm = static_cast<UNSIGN_U>(minus_min);
            HUBERO_IF_CONSTEXPR (unsign_mm > MAX) {

                if (id < -MAX) { // run-time bounds-check
                    throw std::out_of_range(std::string("Literal can represent values ")
                        + std::to_string(-MAX) + ".." + std::to_string(MAX) + ", but "
                        + std::to_string(id) + " was given." );
                }
            }
        }

        // compile-time check-avoider speeds-up the Debug-mode
        HUBERO_IF_CONSTEXPR (MAX < std::numeric_limits<U>::max()) {
            // make compilers happy about signed/unsigned comparison in the if(...)
            auto signed_id = static_cast<typename std::make_signed<U>::type>(id);
             // run-time bounds-check
            if (MAX < signed_id) {
                throw std::out_of_range(std::string("Literal can represent values ")
                    + std::to_string(-MAX) + ".." + std::to_string(MAX) + ", but "
                    + std::to_string(id) + " was given." );
            }
        }
    }

    template<class U, U U_MAX>
    LitT(const LitT<U, U_MAX>& lit)
    : lit_id(static_cast<T>(static_cast<U>(lit)))
    {
        // compile-time check-avoider speeds-up the Debug-mode
        HUBERO_IF_CONSTEXPR (MAX < U_MAX) {
            // run-time bounds-check
            if (MAX < static_cast<U>(lit)) {
                throw std::out_of_range(std::string("Literal can represent values ")
                    + std::to_string(-MAX) + ".." + std::to_string(MAX) + ", but "
                    + std::to_string(static_cast<U>(lit)) + " was given." );
            }
        }
    }

    template<class U, U U_MAX>
    LitT(VarT<U,U_MAX> var, bool sign)
    : lit_id(static_cast<T>(static_cast<U>(var)) * (sign ? +1 : -1))
    {
        // compile-time check-avoider speeds-up the Debug-mode
        HUBERO_IF_CONSTEXPR (MAX < U_MAX) {
             // run-time bounds-check
            if (MAX < static_cast<U>(var)) {
                throw std::out_of_range(std::string("Literal can represent values ")
                    + std::to_string(-MAX) + ".." + std::to_string(MAX) + ", but "
                    + std::to_string(static_cast<U>(var)) + " was given." );
            }
        }
    }

    bool sign() const
    {
        return lit_id >= 0;
    }

    VarT<UNSIGNED_T, static_cast<UNSIGNED_T>(MAX)> var() const
    {
        // TODO: bounds-check can be avoided
        return VarT<UNSIGNED_T, static_cast<UNSIGNED_T>(MAX)>(
            sign() ? lit_id : -lit_id
        );
    }

    LitT<T,MAX> operator ~() const
    {
        return LitT<T,MAX>(-lit_id);
    }

    LitT<T,MAX> operator ^(bool sign) const
    {
        return LitT<T,MAX>(lit_id * (sign ? +1 : -1));
    }

    // integer-type conversions

    bool operator ==(const LitT<T,MAX>& rhs) const
    {
        return lit_id == rhs.lit_id;
    }

    bool operator !=(const LitT<T,MAX>& rhs) const
    {
        return lit_id != rhs.lit_id;
    }



    bool operator <(const LitT<T,MAX>& rhs) const
    {
        return lit_id < rhs.lit_id;
    }

    bool operator >(const LitT<T,MAX>& rhs) const
    {
        return lit_id > rhs.lit_id;
    }

    bool operator <=(const LitT<T,MAX>& rhs) const
    {
        return lit_id <= rhs.lit_id;
    }

    bool operator >=(const LitT<T,MAX>& rhs) const
    {
        return lit_id >= rhs.lit_id;
    }



    LitT<T,MAX> operator +(const LitT<T,MAX>& rhs) const
    {
        T result = lit_id + rhs.lit_id;
        // TODO: check bounds
        return LitT<T,MAX>(result);
    }

    LitT<T,MAX>& operator ++()
    {
        assert(lit_id < MAX &&
            "Literal overflow detected in"
            " the pre-increment operator");
        ++lit_id;
        return *this;
    }

    LitT<T,MAX> operator ++(int)
    {
        assert(lit_id < MAX &&
            "Literal overflow detected in"
            " the post-increment operator");
        auto copy = *this;
        ++lit_id;
        return copy;
    }

    LitT<T,MAX>& operator +=(const LitT<T,MAX>& rhs)
    {
        lit_id += rhs.lit_id;
        // TODO: check bounds
        return *this;
    }



    LitT<T,MAX> operator -(const LitT<T,MAX>& rhs) const
    {
        T result = lit_id - rhs.lit_id;
        // TODO: check bounds
        return LitT(result);
    }

    LitT<T,MAX>& operator --()
    {
        assert(lit_id > -MAX &&
            "Literal overflow detected in"
            " the pre-decrement operator");
        --lit_id;
        return *this;
    }

    LitT<T,MAX> operator --(int)
    {
        assert(lit_id > -MAX &&
            "Literal overflow detected in"
            " the post-decrement operator");
        auto copy = *this;
        --lit_id;
        return copy;
    }

    LitT<T,MAX>& operator -=(const LitT<T,MAX>& rhs)
    {
        lit_id -= rhs.lit_id;
        // TODO: check bounds
        return *this;
    }

    // conversions

    template<class U, U U_MAX>
    explicit operator LitT<U, U_MAX>() const
    {
        // bounds-check is done in the constructor
        return LitT<U, U_MAX>(lit_id);
    }

    template<class U>
    explicit operator U() const
    {
        static_assert(std::is_signed<U>::value,
            "Literal can be casted only to integral types.");

        if (lit_id < std::numeric_limits<U>::min()) {
            throw std::out_of_range(std::string("Literal ")
                + std::to_string(lit_id) + " is too small to be casted to "
                + tools::type_to_string<U>() + "." );
        }

        // compile-time check-avoider speeds-up the Debug-mode
        HUBERO_IF_CONSTEXPR (std::numeric_limits<U>::max() < MAX) {
            // run-time bounds-check
            if (std::numeric_limits<U>::max() < lit_id) {
                throw std::out_of_range(std::string("Literal ")
                    + std::to_string(lit_id) + " is too big to be casted to "
                    + tools::type_to_string<U>() + "." );
            }
        }

        return static_cast<U>(lit_id);
    }

    std::string to_string() const
    {
        std::string retval;
        if (!sign()) {
            retval += "-";
        }

        auto var_id = static_cast<UNSIGNED_T>(var());
        retval += std::to_string(var_id);
        return retval;
    }
}; // LitT

using Lit = LitT<int>;

} // dimacs
} // hubero
#endif // HUBERO_CORE_H_
