// ///////////////////////////////////////////////////////////////////////////
// xrb_polynomial.cpp by Victor Dods, created 2006/01/21
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_polynomial.hpp"

#include "xrb_math.hpp"

namespace Xrb
{

Polynomial::Polynomial (Polynomial const &polynomial)
{
    ASSERT1(polynomial.IsHighestCoefficientNonZero());
    
    m.resize(polynomial.m.size());
    copy(polynomial.m.begin(), polynomial.m.end(), m.begin());

    ASSERT1(IsHighestCoefficientNonZero());
}

Float Polynomial::Evaluate (Float const x) const
{
    Uint32 order = GetOrder();
    Float retval = m[0];
    if (order > 0)
    {   
        Uint32 basis_size = Math::HighestBitIndex(order) + 1;
    #if defined(WIN32)
        Float basis_vector[32]; // largest possible size
    #else
        Float basis_vector[basis_size];
    #endif
        basis_vector[0] = x;
        for (Uint32 i = 1; i < basis_size; ++i)
            basis_vector[i] = basis_vector[i-1] * basis_vector[i-1];
        
        for (Uint32 i = 1, size = m.size(); i < size; ++i)
            retval += m[i] * FastPow(i, basis_vector);
    }
    return retval;
}

void Polynomial::Derive (Uint32 const order)
{
    ASSERT1(IsHighestCoefficientNonZero());
    
    // TODO: more efficient non-iterative version
    for (Uint32 d = 0; d < order; ++d)
    {
        if (GetOrder() == 0)
        {
            m[0] = 0.0f;
            break;
        }
            
        for (Uint32 i = 0, size = m.size()-1;
            i < size; 
            ++i)
        {
            m[i] = static_cast<Float>(i+1) * m[i+1];
        }
        m.resize(m.size() - 1);
    }
    
    ASSERT1(IsHighestCoefficientNonZero());
}

void Polynomial::Integrate (Uint32 const order)
{
    ASSERT1(IsHighestCoefficientNonZero());

    // TODO: more efficient non-iterative version
    for (Uint32 d = 0; d < order; ++d)
    {    
        m.resize(m.size() + 1);
        Uint32 high_order = GetOrder();
        for (Uint32 i = high_order; i > 0; --i)
        {
            m[i] = m[i-1] / static_cast<Float>(i);
        }
        m[0] = 0.0f;
    }
    
    Minimize();
    ASSERT1(IsHighestCoefficientNonZero());
}

void Polynomial::Solve (SolutionSet *const solution_set, Float const tolerance) const
{
    ASSERT1(tolerance > 0.0f);
    ASSERT1(solution_set != NULL);
    ASSERT1(solution_set->empty());
    
    if (GetOrder() == 0 && m[0] == 0.0f)
    {
        // do nothing
    }
    else if (GetOrder() == 1)
    {
        // solve the simple linear equation
        solution_set->insert(-m[0] / m[1]);
    }
    else if (GetOrder() == 2)
    {
        // solve the quadratic equation
        Float determinant = m[1] * m[1] - 4.0f * m[2] * m[0];
        if (determinant >= 0.0f)
        {
            Float base = -m[1];
            Float offset = sqrt(determinant);
            Float divisor = 2.0f * m[2];
            solution_set->insert((base - offset) / divisor);
            solution_set->insert((base + offset) / divisor);
        }
    }
    else
    {
        // take the derivative for this polynomial's local max/minima
        Polynomial derivative(Derivative());
        std::set<Float> derivative_solution_set;
        derivative.Solve(&derivative_solution_set, tolerance);
    
        Float solution;
        
        // it's possible for the derivative to have no real solutions
        if (derivative_solution_set.size() == 0)
        {
            if (NewtonsMethod(&solution, derivative, 0.0f, tolerance))
                solution_set->insert(solution);
        }
        else
        {
            // use newton's method
            if (NewtonsMethod(&solution, derivative, *derivative_solution_set.begin() - 1.0f, tolerance))
                solution_set->insert(solution);
                    
            for (std::set<Float>::iterator it_b = derivative_solution_set.begin(),
                                           it_a = it_b++,
                                           it_end = derivative_solution_set.end();
                 it_b != it_end;
                 ++it_a, ++it_b)
            {
                Float span = *it_b - *it_a;
                if (span == 0.0f)
                    continue;
                if (NewtonsMethod(&solution, derivative, *it_b - 0.5f * span, tolerance))
                    solution_set->insert(solution);
            }
            
            if (NewtonsMethod(&solution, derivative, *derivative_solution_set.rbegin() + 1.0f, tolerance))
                solution_set->insert(solution);
        }
    }
}

void Polynomial::operator = (Polynomial const &operand)
{
    ASSERT1(IsHighestCoefficientNonZero());
    ASSERT1(operand.IsHighestCoefficientNonZero());
    
    m.resize(operand.m.size());
    copy(operand.m.begin(), operand.m.end(), m.begin());

    ASSERT1(IsHighestCoefficientNonZero());
}

Polynomial Polynomial::operator * (Polynomial const &operand) const
{
    ASSERT1(IsHighestCoefficientNonZero());
    ASSERT1(operand.IsHighestCoefficientNonZero());
    
    Polynomial retval;
    if (GetOrder() == 0 && m[0] == 0.0f)
        return retval;    
    
    Uint32 left_operand_order = GetOrder();
    Uint32 right_operand_order = operand.GetOrder();
    Uint32 order = left_operand_order + right_operand_order;
    retval.m.resize(order + 1);
    
    for (Uint32 i = 0; i <= order; ++i)
    {
        retval.m[i] = 0.0f;
        
        Uint32 right_operand_term = Min(i, right_operand_order);
        Uint32 left_operand_term = i - right_operand_term;
        ASSERT1(left_operand_term <= left_operand_order);
        
        for (;
             left_operand_term <= left_operand_order && 
             right_operand_term <= right_operand_order; // the unsigned version of right_operand_term >= 0
             ++left_operand_term, --right_operand_term)
        {
            retval.m[i] += m[left_operand_term] * operand.m[right_operand_term];
        }
    }
    
    ASSERT1(retval.IsHighestCoefficientNonZero());
    return retval;
}

void Polynomial::operator += (Polynomial const &operand) 
{
    ASSERT1(IsHighestCoefficientNonZero());
    ASSERT1(operand.IsHighestCoefficientNonZero());
    
    Uint32 size = Max(m.size(), operand.m.size());
    m.resize(size);
    for (Uint32 i = 0; i < size; ++i)
        m[i] += operand.Get(i);

    Minimize();
    ASSERT1(IsHighestCoefficientNonZero());
}

void Polynomial::operator -= (Polynomial const &operand) 
{
    ASSERT1(IsHighestCoefficientNonZero());
    ASSERT1(operand.IsHighestCoefficientNonZero());
    
    Uint32 size = Max(m.size(), operand.m.size());
    m.resize(size);
    for (Uint32 i = 0; i < size; ++i)
        m[i] -= operand.Get(i);

    Minimize();
    ASSERT1(IsHighestCoefficientNonZero());
}

void Polynomial::operator *= (Polynomial const &operand)
{
    ASSERT1(IsHighestCoefficientNonZero());
    ASSERT1(operand.IsHighestCoefficientNonZero());
    
    Polynomial result;
    if (GetOrder() > 0 || m[0] != 0.0f)
    {    
        Uint32 left_operand_order = GetOrder();
        Uint32 right_operand_order = operand.GetOrder();
        Uint32 order = left_operand_order + right_operand_order;
        result.m.resize(order + 1);
        
        for (Uint32 i = 0; i <= order; ++i)
        {
            result.m[i] = 0.0f;
            
            Uint32 right_operand_term = Min(i, right_operand_order);
            Uint32 left_operand_term = i - right_operand_term;
            ASSERT1(left_operand_term <= left_operand_order);
            
            for (;
                left_operand_term <= left_operand_order && 
                right_operand_term <= right_operand_order; // the unsigned version of right_operand_term >= 0
                ++left_operand_term, --right_operand_term)
            {
                result.m[i] += m[left_operand_term] * operand.m[right_operand_term];
            }
        }
    }
    
    ASSERT1(result.IsHighestCoefficientNonZero());
    operator=(result);    
}

bool Polynomial::NewtonsMethod (
    Float *const solution,
    Polynomial const &derivative, 
    Float const x,
    Float const tolerance) const
{
    ASSERT1(solution != NULL);
    
    Float iterator[2];
    iterator[0] = x;
    
    Float epsilon;
    Uint32 i = 0;
    Uint32 iteration_count = 0;
    do
    {
        i = 1 - i;
        iterator[i] = NewtonsMethodIteration(derivative, iterator[1 - i]);
        if (iterator[i] == Math::Nan())
            return false;
            
        ++iteration_count;
        epsilon = Abs(iterator[0] - iterator[1]);
    }
    while (epsilon > tolerance && iteration_count < 50);
    
    *solution = iterator[i];
    return epsilon <= tolerance;
}

Float Polynomial::NewtonsMethodIteration (
    Polynomial const &derivative, 
    Float const x) const
{
    Float f_prime_x = derivative.Evaluate(x);
    if (f_prime_x != 0.0f)
        return x - Evaluate(x) / f_prime_x;
    else
        return Math::Nan();
}

void Polynomial::Minimize () const
{
    Uint32 highest_nonzero_coefficient = 0;
    for (Uint32 i = m.size()-1; i > 0; --i)
    {
        if (m[i] != 0.0f)
        {
            highest_nonzero_coefficient = i;
            break;
        }
    }
    m.resize(highest_nonzero_coefficient + 1);
}

std::ostream &operator << (std::ostream &stream, Polynomial const &polynomial)
{
    Uint32 order = polynomial.GetOrder();
    for (Uint32 i = 0; i <= order; ++i)
    {
        if (polynomial[i] != 0.0f)
        {
            stream << polynomial[i] << "*x^" << i;
            if (i < order)
                stream << " + ";
        }
    }

    return stream;
}

} // end of namespace Xrb
