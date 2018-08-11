// Copyright (c) 2018 Radomír Černoch (radomir.cernoch@gmail.com)
//
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#ifndef BOOLSATLIB_CORE_H_
#define BOOLSATLIB_CORE_H_

#include <boolSATlib/tools.hpp>

#include <cassert>
#include <sstream>
#include <string>
#include <limits>

namespace boolSAT {

template<
    class var_type,
    class = typename std::enable_if<std::is_unsigned<var_type>::value>::type
>
class VarT {

    var_type var_id;

public:

    VarT()
    : var_id(0)
    {}

    template<
        class rhs_var_type,
        class = typename std::enable_if<std::is_integral<rhs_var_type>::value>::type
    >
    explicit VarT(const rhs_var_type& id)
    : var_id(static_cast<var_type>(id))
    {
        if (id < 0) {
            throw std::out_of_range(
                std::string("variable can represent non-negative values, but value ")
                    + std::to_string(id) + std::string("was given") );
        }

        // make compilers happy about signed/unsigned comparison in the if(...)
        auto unsigned_id = static_cast<typename std::make_unsigned<rhs_var_type>::type>(id);

        if (unsigned_id > std::numeric_limits<var_type>::max()) {
            throw std::out_of_range(std::string("variable can represent values 0..")
                    + std::to_string(std::numeric_limits<var_type>::max())
                    + ", but " + std::to_string(id) + std::string("was given") );
        }
    }



    bool operator ==(const VarT<var_type>& rhs) const
    {
        return var_id == rhs.var_id;
    }

    bool operator !=(const VarT<var_type>& rhs) const
    {
        return var_id != rhs.var_id;
    }



    bool operator <(const VarT<var_type>& rhs) const
    {
        return var_id < rhs.var_id;
    }

    bool operator >(const VarT<var_type>& rhs) const
    {
        return var_id > rhs.var_id;
    }

    bool operator <=(const VarT<var_type>& rhs) const
    {
        return var_id <= rhs.var_id;
    }

    bool operator >=(const VarT<var_type>& rhs) const
    {
        return var_id >= rhs.var_id;
    }



    VarT<var_type> operator +(const VarT<var_type>& rhs) const
    {
        return VarT(var_id + rhs.var_id);
    }

    VarT<var_type>& operator ++()
    {
        ++var_id;
        return *this;
    }

    VarT<var_type> operator ++(int)
    {
        auto copy = *this;
        ++var_id;
        return copy;
    }

    VarT<var_type>& operator +=(const VarT<var_type>& rhs)
    {
        var_id += rhs.var_id;
        return *this;
    }



    VarT<var_type> operator -(const VarT<var_type>& rhs) const
    {
        return VarT(var_id - rhs.var_id);
    }

    VarT<var_type>& operator --()
    {
        --var_id;
        return *this;
    }

    VarT<var_type> operator --(int)
    {
        auto copy = *this;
        --var_id;
        return copy;
    }

    VarT<var_type>& operator -=(const VarT<var_type>& rhs)
    {
        var_id -= rhs.var_id;
        return *this;
    }

    // conversions

    template<class rhs_var_type>
    operator VarT<rhs_var_type>() const
    {
        if (var_id > std::numeric_limits<rhs_var_type>::max()) {
            throw std::out_of_range(std::string("variable ") + to_string()
                + std::string(" is too big to be downcasted to ")
                + boolSAT::tools::type_to_string<rhs_var_type>());
        }

        return VarT<rhs_var_type>(static_cast<rhs_var_type>(var_id));
    }

    explicit operator var_type() const
    {
        return var_id;
    }

    std::string to_string() const
    {
        return std::to_string(var_id);
    }

}; // VarT

using Var = VarT<unsigned>;



namespace mini {

template<
    class var_type,
    class = typename std::enable_if<std::is_unsigned<var_type>::value>::type
>
class LitT {

    var_type lit_id;

public:

    LitT()
    : lit_id(0)
    {}

    template<
        class rhs_var_type,
        class = typename std::enable_if<std::is_integral<rhs_var_type>::value>::type
    >
    explicit LitT(const rhs_var_type& id)
    : lit_id(static_cast<var_type>(id))
    {
        if (id < 0) {
            throw std::out_of_range(
                std::string("literal is represented by unsigned types, but value ")
                    + std::to_string(id) + std::string("was given") );
        }

        // unify types to "unsigned wider-of-the-two"
        // to make compilers happy about the if(...)
        using wider = typename std::conditional<
            sizeof(rhs_var_type) >= sizeof(var_type),
            typename std::make_unsigned<rhs_var_type>::type,
            var_type
        >::type;

        auto max_value = static_cast<wider>(std::numeric_limits<var_type>::max());
        if (static_cast<wider>(id) > max_value) {
            throw std::out_of_range(std::string("literal ID=") + std::to_string(id)
                + std::string(" is too big to be represented as boolSAT::mini::Lit<")
                + boolSAT::tools::type_to_string<var_type>() + std::string(">") );
        }
    }

    LitT(VarT<var_type> var, bool sign)
    : lit_id(2 * static_cast<var_type>(var) + (sign ? 1u : 0u))
    {
        if (static_cast<var_type>(var) >
                std::numeric_limits<var_type>::max() / 2) {
            throw std::out_of_range(std::string("variable ") + var.to_string()
                + std::string(" is too big to be represented as boolSAT::mini::Lit<")
                + boolSAT::tools::type_to_string<var_type>() + std::string(">") );
        }
    }

    bool sign() const
    {
        return lit_id & 1;
    }

    VarT<var_type> var() const
    {
        return VarT<var_type>(lit_id / 2);
    }

    LitT<var_type> operator ~() const
    {
        return LitT<var_type>(lit_id ^ 1);
    }

    LitT<var_type> operator ^(bool sign) const
    {
        return LitT<var_type>(lit_id ^ (sign ? 1u : 0u));
    }

    // integer-type conversions

    bool operator ==(const LitT<var_type>& rhs) const
    {
        return lit_id == rhs.lit_id;
    }

    bool operator !=(const LitT<var_type>& rhs) const
    {
        return lit_id != rhs.lit_id;
    }



    bool operator <(const LitT<var_type>& rhs) const
    {
        return lit_id < rhs.lit_id;
    }

    bool operator >(const LitT<var_type>& rhs) const
    {
        return lit_id > rhs.lit_id;
    }

    bool operator <=(const LitT<var_type>& rhs) const
    {
        return lit_id <= rhs.lit_id;
    }

    bool operator >=(const LitT<var_type>& rhs) const
    {
        return lit_id >= rhs.lit_id;
    }



    LitT<var_type> operator +(const LitT<var_type>& rhs) const
    {
        return LitT(lit_id + rhs.lit_id);
    }

    LitT<var_type>& operator ++()
    {
        ++lit_id;
        return *this;
    }

    LitT<var_type> operator ++(int)
    {
        auto copy = *this;
        ++lit_id;
        return copy;
    }

    LitT<var_type>& operator +=(const LitT<var_type>& rhs)
    {
        lit_id += rhs.lit_id;
        return *this;
    }



    LitT<var_type> operator -(const LitT<var_type>& rhs) const
    {
        return LitT(lit_id - rhs.lit_id);
    }

    LitT<var_type>& operator --()
    {
        --lit_id;
        return *this;
    }

    LitT<var_type> operator --(int)
    {
        auto copy = *this;
        --lit_id;
        return copy;
    }

    LitT<var_type>& operator -=(const LitT<var_type>& rhs)
    {
        lit_id -= rhs.lit_id;
        return *this;
    }

    // conversions

    template<class rhs_var_type> operator LitT<rhs_var_type>() const
    {
        if (lit_id > std::numeric_limits<rhs_var_type>::max()) {
            throw std::out_of_range(std::string("literal ") + to_string()
                + std::string(" is too big to be downcasted to ")
                + boolSAT::tools::type_to_string<rhs_var_type>());
        }

        return LitT<rhs_var_type>(static_cast<rhs_var_type>(lit_id));
    }

    std::string to_string() const
    {
        std::string retval;
        if (!sign()) {
            retval += "-";
        }

        retval += std::to_string(static_cast<var_type>(var()));
        return retval;
    }

    explicit operator var_type() const
    {
        return lit_id;
    }
}; // LitT

using Lit = LitT<unsigned>;

} // mini

namespace dimacs {

template<
    class var_type,
    class = std::enable_if<std::is_unsigned<var_type>::value>
>
class Lit {

    VarT<var_type> var;

    bool sign;

}; // Lit
} // dimacs
} // boolSAT
#endif // BOOLSATLIB_CORE_H_
