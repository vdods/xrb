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

using namespace std;

namespace Lvd {
namespace Xml {

void PrintStringLiteral (ostream &stream, string const &s)
{
    stream << '"';
    for (string::const_iterator it = s.begin(), it_end = s.end(); it != it_end; ++it)
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

string StringLiteral (string const &s)
{
    ostringstream out;
    PrintStringLiteral(out, s);
    return out.str();
}

void Text::Print (ostream &stream) const
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

DomNodeVector::const_iterator Document::FirstElement (
    Element const *&element,
    string const &element_name,
    string const &attribute_name,
    bool specify_attribute_value,
    string const &attribute_value) const
{
    DomNodeVector::const_iterator it = m_element.begin();
    RetrieveElement(it, element, element_name, attribute_name, specify_attribute_value, attribute_value);
    return it;
}

void Document::NextElement (
    DomNodeVector::const_iterator &it,
    Element const *&element,
    string const &element_name,
    string const &attribute_name,
    bool specify_attribute_value,
    string const &attribute_value) const
{
    if (it != m_element.end());
        ++it;
    RetrieveElement(it, element, element_name, attribute_name, specify_attribute_value, attribute_value);
}

void Document::Print (ostream &stream) const
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
    string const &element_name,
    string const &attribute_name,
    bool specify_attribute_value,
    string const &attribute_value) const
{
    bool specify_element_name = !element_name.empty();
    bool specify_attribute_name = !attribute_name.empty();
    if (specify_attribute_value)
        assert(specify_attribute_name && "can't specify_attribute_value without non-empty attribute_name");

    for ( ; it != m_element.end(); ++it)
    {
        DomNode const *dom_node = *it;
        assert(dom_node != NULL);
        if (dom_node->m_type == ELEMENT || dom_node->m_type == PROCESSING_INSTRUCTION)
        {
            assert(dynamic_cast<Element const *>(dom_node) != NULL);
            Element const *e = static_cast<Element const *>(dom_node);

            // if we're looking for an element name and it doesn't match, skip this element.
            if (specify_element_name && e->m_name != element_name)
                continue;

            // if we're looking for an attribute name and it doesn't have it, skip.
            if (specify_attribute_name && !e->HasAttribute(attribute_name))
                continue;

            // if we're looking for an attribute value and it doesn't match, skip.
            if (specify_attribute_value && e->AttributeValue(attribute_name) != attribute_value)
                continue;

            // otherwise everything matched, so use it.
            element = e;
            return;
        }
    }
    element = NULL;
}

bool Element::HasAttribute (string const &attribute_name) const
{
    return m_attribute.find(attribute_name) != m_attribute.end();
}

string const &Element::AttributeValue (string const &attribute_name) const
{
    static string const s_empty_string;
    AttributeMap::const_iterator it = m_attribute.find(attribute_name);
    if (it != m_attribute.end())
        return it->second;
    else
        return s_empty_string;
}

void Element::Print (ostream &stream) const
{
    // a processing instruction can't have elements
    if (!m_element.empty())
        assert(m_type != PROCESSING_INSTRUCTION);

    // print attributes
    stream << '<';
    if (m_type == PROCESSING_INSTRUCTION)
        stream << '?';
    stream << m_name;
    for (AttributeMap::const_iterator it = m_attribute.begin(),
                                      it_end = m_attribute.end();
            it != it_end;
            ++it)
    {
        stream << ' ' << it->first << '=';
        PrintStringLiteral(stream, it->second);
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

ostream &operator << (ostream &stream, DomNode const &node)
{
    node.Print(stream);
    return stream;
}

} // end of namespace Xml
} // end of namespace Lvd
