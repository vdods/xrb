// ///////////////////////////////////////////////////////////////////////////
// lvd_xml.cpp by Victor Dods, created 2009/12/13
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include <lvd_xml.hpp>

#include <sstream>

namespace Lvd {
namespace Xml {

void PrintStringLiteral (std::ostream &stream, std::string const &s)
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

std::string StringLiteral (std::string const &s)
{
    std::ostringstream out;
    PrintStringLiteral(out, s);
    return out.str();
}

void Text::Print (std::ostream &stream) const
{
    if (m_type == CDATA)
        stream << "<[!CDATA[";
    stream << m_text;
    if (m_type == CDATA)
        stream << "]]>";
}

Document::~Document ()
{
    for (DomNodeVector::iterator it = m_element.begin(), it_end = m_element.end(); it != it_end; ++it)
        delete *it;
}

void Document::FirstElement (
    DomNodeVector::const_iterator &it,
    Element const *&element,
    std::string const &element_name) const
{
    it = m_element.begin();
    RetrieveElement(it, element, element_name);
}

void Document::NextElement (
    DomNodeVector::const_iterator &it,
    Element const *&element,
    std::string const &element_name) const
{
    if (it != m_element.end());
        ++it;
    RetrieveElement(it, element, element_name);
}

void Document::Print (std::ostream &stream) const
{
    for (DomNodeVector::size_type i = 0; i < m_element.size(); ++i)
    {
        assert(m_element[i] != NULL);
        m_element[i]->Print(stream);
    }
}

void Document::RetrieveElement (
    DomNodeVector::const_iterator &it,
    Element const *&element,
    std::string const &element_name) const
{
    while (it != m_element.end())
    {
        DomNode const *dom_node = *it;
        assert(dom_node != NULL);
        if (dom_node->m_type == ELEMENT || dom_node->m_type == PROCESSING_INSTRUCTION)
        {
            assert(dynamic_cast<Element const *>(dom_node) != NULL);
            Element const *el = static_cast<Element const *>(dom_node);
            if (element_name.empty() || el->m_name == element_name)
            {
                element = el;
                return;
            }
        }
        ++it;
    }
    element = NULL;
}

bool Element::HasAttribute (std::string const &attribute_name) const
{
    return m_attribute.find(Attribute(attribute_name, "")) != m_attribute.end();
}

std::string const &Element::AttributeValue (std::string const &attribute_name) const
{
    static std::string const s_empty_string;
    AttributeSet::const_iterator it = m_attribute.find(Attribute(attribute_name, ""));
    if (it != m_attribute.end())
        return it->m_value;
    else
        return s_empty_string;
}

void Element::Print (std::ostream &stream) const
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

    // print the elements (done by Document)
    Document::Print(stream);

    // if there were elements printed, print an end-tag
    if (!m_element.empty())
        stream << "</" << m_name << '>';
}

std::ostream &operator << (std::ostream &stream, DomNode const &node)
{
    node.Print(stream);
    return stream;
}

} // end of namespace Xml
} // end of namespace Lvd
