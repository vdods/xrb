// ///////////////////////////////////////////////////////////////////////////
// lvd_xml.hpp by Victor Dods, created 2009/12/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(LVD_XML_HPP_)
#define LVD_XML_HPP_

#if defined(HAVE_CONFIG_H)
    #include "config.h"
#endif // defined(HAVE_CONFIG_H)

#include <cassert>
#include <ostream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include <lvd_filoc.hpp>

namespace Lvd {
namespace Xml {

inline void PrintStringLiteral (std::ostream &stream, std::string const &s)
{
    stream << '"';
    for (std::string::const_iterator it = s.begin(), it_end = s.end(); it != it_end; ++it)
        switch (*it)
        {
            case '"' : stream << "&quot;"; break;
            case '\'': stream << "&apos;"; break;
            case '&' : stream << "&amp;";  break;
            case '<' : stream << "&lt;";   break;
            case '>' : stream << "&gt;";   break;
            default  : stream << *it;      break;
        }
    stream << '"';
}

inline std::string StringLiteral (std::string const &s)
{
    std::ostringstream out;
    PrintStringLiteral(out, s);
    return out.str();
}

struct Attribute
{
    std::string m_name;
    std::string m_value;

    Attribute (std::string const &name, std::string const &value)
        :
        m_name(name), m_value(value)
    {
        assert(!m_name.empty());
    }

    bool operator == (Attribute const &a) const
    {
        return m_name == a.m_name && m_value == a.m_value;
    }

    struct Order
    {
        bool operator () (Attribute const &left, Attribute const &right)
        {
            return left.m_name < right.m_name;
        }
    }; // end of struct Xml::Attribute::Order
}; // end of struct Xml::Attribute

class DomNode;

typedef std::set<Attribute, Attribute::Order> AttributeSet;
typedef std::vector<DomNode *> DomNodeVector;

struct DomNode
{
    enum Type {
        TEXT = 0,
        CDATA,
        PROCESSING_INSTRUCTION,
        TAG,
        DOCUMENT,

        TYPE_COUNT
    }; // end of enum Xml::DomNode::Type

    FiLoc m_filoc;
    Type const m_type;

    DomNode (FiLoc const &filoc, Type type)
        :
        m_filoc(filoc),
        m_type(type)
    {
        assert(m_type >= 0 && m_type < TYPE_COUNT);
    }
    virtual ~DomNode () { }

    virtual void Print (std::ostream &stream) const = 0;
}; // end of struct Xml::DomNode

// acceptable types are TEXT and CDATA
struct Text : public DomNode
{
    std::string m_text;

    Text (FiLoc const &filoc, Type type = TEXT)
        :
        DomNode(filoc, type)
    {
        assert((type == TEXT || type == CDATA) && "acceptable types are TEXT and CDATA");
    }
    Text (std::string const &text, FiLoc const &filoc = FiLoc::ms_invalid, Type type = TEXT) : DomNode(filoc, type), m_text(text) { }
    virtual ~Text () { }

    virtual void Print (std::ostream &stream) const
    {
        if (m_type == CDATA)
            stream << "<[!CDATA[";
        stream << m_text;
        if (m_type == CDATA)
            stream << "]]>";
    }
}; // end of struct Xml::Text

// acceptable types are TAG and PROCESSING_INSTRUCTION
struct Tag : public DomNode
{
    std::string m_name;
    AttributeSet m_attribute;
    DomNodeVector m_element;

    Tag (std::string const &name, Type type = TAG, FiLoc const &filoc = FiLoc::ms_invalid)
        :
        DomNode(filoc, type),
        m_name(name)
    {
        assert((type == TAG || type == PROCESSING_INSTRUCTION) && "acceptable types are TAG and PROCESSING_INSTRUCTION");
    }
    virtual ~Tag ()
    {
        for (DomNodeVector::iterator it = m_element.begin(), it_end = m_element.end(); it != it_end; ++it)
            delete *it;
    }

    virtual void Print (std::ostream &stream) const
    {
        // a processing instruction can't have elements
        if (!m_element.empty())
            assert(m_type != PROCESSING_INSTRUCTION);

        // print attributes
        stream << '<';
        if (m_type == PROCESSING_INSTRUCTION)
            stream << '?';
        stream << m_name;
        for (AttributeSet::const_iterator it = m_attribute.begin(),
                                            it_end = m_attribute.end();
            it != it_end;
            ++it)
        {
            stream << ' ' << it->m_name << '=';
            PrintStringLiteral(stream, it->m_value);
        }
        if (m_type == PROCESSING_INSTRUCTION)
            stream << '?';
        else if (m_element.empty()) // if no elements, make the tag self-ended
            stream << " /";
        stream << '>';

        // print elements
        for (DomNodeVector::size_type i = 0; i < m_element.size(); ++i)
        {
            assert(m_element[i] != NULL);
            m_element[i]->Print(stream);
        }

        // if there were elements printed, print an end-tag
        if (!m_element.empty())
            stream << "</" << m_name << '>';
    }
}; // end of struct Xml::Tag

struct Document : public DomNode
{
    DomNodeVector m_element;

    Document () : DomNode(FiLoc::ms_invalid, DOCUMENT) { }
    virtual ~Document ()
    {
        for (DomNodeVector::iterator it = m_element.begin(), it_end = m_element.end(); it != it_end; ++it)
            delete *it;
    }

    virtual void Print (std::ostream &stream) const
    {
        for (DomNodeVector::size_type i = 0; i < m_element.size(); ++i)
        {
            assert(m_element[i] != NULL);
            m_element[i]->Print(stream);
        }
    }
}; // end of struct Xml::Document

inline std::ostream &operator << (std::ostream &stream, DomNode const &node)
{
    node.Print(stream);
    return stream;
}

} // end of namespace Xml
} // end of namespace Lvd

#endif // !defined(LVD_XML_HPP_)
