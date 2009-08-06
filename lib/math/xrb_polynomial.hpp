// ///////////////////////////////////////////////////////////////////////////
// xrb_polynomial.hpp by Victor Dods, created 2006/01/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_POLYNOMIAL_HPP_)
#define _XRB_POLYNOMIAL_HPP_

#include "xrb.hpp"

#include <set>
#include <sstream>
#include <vector>

namespace Xrb
{

class Polynomial
{
public:

    typedef std::set<Float> SolutionSet;
    typedef SolutionSet::iterator SolutionSetIterator;

    inline Polynomial ()
    {
        m.resize(1);
        m[0] = 0.0f;
        ASSERT1(IsHighestCoefficientNonZero());
    }
    inline Polynomial (Float root)
    {
        m.resize(2);
        m[0] = -root;
        m[1] = 1.0f;
        ASSERT1(IsHighestCoefficientNonZero());
    }
    Polynomial (Polynomial const &polynomial);
    ~Polynomial () { }

    inline Uint32 GetOrder () const
    {
        ASSERT1(m.size() > 0);
        return m.size() - 1;
    }
    inline Float Get (Uint32 power) const
    {
        if (power < m.size())
            return m[power];
        else
            return 0.0f;
    }
    inline Polynomial Derivative (Uint32 order = 1) const
    {
        Polynomial retval(*this);
        retval.Derive(order);
        return retval;
    }
    inline Polynomial Integral (Uint32 order = 1) const
    {
        Polynomial retval(*this);
        retval.Integrate(order);
        return retval;
    }
    inline void Set (Uint32 power, Float coefficient)
    {
        if (power >= m.size())
            m.resize(power + 1);
        m[power] = coefficient;
        Minimize();
        ASSERT1(IsHighestCoefficientNonZero());
    }
    inline Float operator [] (Uint32 power) const
    {
        ASSERT1(power < m.size());
        return m[power];
    }

    Float Evaluate (Float x) const;
    void Derive (Uint32 order = 1);
    void Integrate (Uint32 order = 1);
    void Solve (SolutionSet *solution_set, Float tolerance) const;
    
    void operator = (Polynomial const &operand);
    
    inline Polynomial operator + (Polynomial const &operand) const
    {
        Polynomial retval(*this);
        retval += operand;
        return retval;
    }
    inline Polynomial operator - (Polynomial const &operand) const
    {
        Polynomial retval(*this);
        retval -= operand;
        return retval;
    }
    Polynomial operator * (Polynomial const &operand) const;

    void operator += (Polynomial const &operand);
    void operator -= (Polynomial const &operand);
    void operator *= (Polynomial const &operand);
        
private:

    inline bool IsHighestCoefficientNonZero () const
    {
        if (m.size() == 1)
            return true;
        else
            return *m.rbegin() != 0.0f;
    }
    static inline Float FastPow (Uint32 power, Float *basis_vector)
    {
        Float retval = 1.0f;
        while (power != 0)
        {
            if ((power & 1) != 0)
                retval *= *basis_vector;
            power >>= 1;
            ++basis_vector;
        }
        return retval;
    }
    
    bool NewtonsMethod (
        Float *solution,
        Polynomial const &derivative, 
        Float x,
        Float tolerance) const;
    Float NewtonsMethodIteration (Polynomial const &derivative, Float x) const;
    void Minimize () const;

    mutable std::vector<Float> m;
}; // end of class Polynomial

std::ostream &operator << (std::ostream &stream, Polynomial const &polynomial);
  
} // end of namespace Xrb
    
#endif // !defined(_XRB_POLYNOMIAL_HPP_)
