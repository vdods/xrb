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
#include <string>
#include <vector>

#include <lvd_filoc.hpp>

namespace Lvd {
namespace Xml {

void PrintStringLiteral (std::ostream &stream, std::string const &s);
std::string StringLiteral (std::string const &s);

struct Attribute
{
    std::string m_name;
    std::string m_value;

    Attribute (std::string const &name, std::string const &value)
        :
        m_name(name),
        m_value(value)
    {
        assert(!m_name.empty());
    }

    bool operator == (Attribute const &a) const
    {
        return m_name == a.m_name && m_value == a.m_value;
    }

    struct Order
    {
        bool operator () (Attribute const &left, Attribute const &right) const
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
        CDATA = 0,
        DOCUMENT,
        ELEMENT,
        PROCESSING_INSTRUCTION,
        TEXT,

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
    Text (std::string const &text, FiLoc const &filoc = FiLoc::ms_invalid, Type type = TEXT)
        :
        DomNode(filoc, type), m_text(text)
    { }
    virtual ~Text () { }

    virtual void Print (std::ostream &stream) const;
}; // end of struct Xml::Text

struct Element;

struct Document : public DomNode
{
    DomNodeVector m_element;

    Document () : DomNode(FiLoc::ms_invalid, DOCUMENT) { }
    virtual ~Document ();

    DomNodeVector::const_iterator FirstElement (
        Element const *&element,
        std::string const &element_name = "",
        std::string const &attribute_name = "",
        bool specify_attribute_value = false,
        std::string const &attribute_value = "") const;
    void NextElement (
        DomNodeVector::const_iterator &it,
        Element const *&element,
        std::string const &element_name = "",
        std::string const &attribute_name = "",
        bool specify_attribute_value = false,
        std::string const &attribute_value = "") const;

    virtual void Print (std::ostream &stream) const;

protected:

    // may only be used by Element
    Document (FiLoc const &filoc, Type type)
        :
        DomNode(filoc, type)
    {
        assert((type == ELEMENT || type == PROCESSING_INSTRUCTION) && "acceptable types are ELEMENT and PROCESSING_INSTRUCTION");
    }

private:

    void RetrieveElement (
        DomNodeVector::const_iterator &it,
        Element const *&element,
        std::string const &element_name,
        std::string const &attribute_name,
        bool specify_attribute_value,
        std::string const &attribute_value) const;
}; // end of struct Xml::Document

// acceptable types are ELEMENT and PROCESSING_INSTRUCTION
struct Element : public Document
{
    std::string m_name;
    AttributeSet m_attribute;

    Element (std::string const &name, Type type = ELEMENT, FiLoc const &filoc = FiLoc::ms_invalid)
        :
        Document(filoc, type),
        m_name(name)
    {
        assert((type == ELEMENT || type == PROCESSING_INSTRUCTION) && "acceptable types are ELEMENT and PROCESSING_INSTRUCTION");
    }
    virtual ~Element () { }

    bool HasAttribute (std::string const &attribute_name) const;
    std::string const &AttributeValue (std::string const &attribute_name) const;

    virtual void Print (std::ostream &stream) const;
}; // end of struct Xml::Element

std::ostream &operator << (std::ostream &stream, DomNode const &node);

} // end of namespace Xml
} // end of namespace Lvd

#endif // !defined(LVD_XML_HPP_)
