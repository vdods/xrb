// ///////////////////////////////////////////////////////////////////////////
// xrb_characterfilter.hpp by Victor Dods, created 2005/02/23
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_CHARACTERFILTER_HPP_)
#define _XRB_CHARACTERFILTER_HPP_

#include "xrb.hpp"

#include <string>

namespace Xrb
{

/** The filtering functionality operates by allowing or denying characters
  * on a character-by-character basis.  If a character is allowed, its
  * value is returned unchanged by GetFilteredCharacter.  If it is denied,
  * then '\0' is returned.
  *
  * There are two types of operation for the filter; allow-only-chars and
  * deny-only-chars.  Allow-only-chars indicates that only the characters
  * found in the filter string should be passed through the filter.
  * Deny-only-chars indicates only the characters <strong>not</strong>
  * found in the filter string should be passed through the filter.
  *
  * @brief Facility for filtering characters (e.g. for @ref Xrb::LineEdit).
  */
class CharacterFilter
{
public:

    /** @brief Enumerates the two filter types.
      */
    enum FilterType
    {
        ALLOW = 0, ///< Allow-only-chars - pass only the chars found in the filter string.
        DENY       ///< Deny-only-chars - pass only the chars <strong>not</strong> found in the filter string.
    }; // end of enum FilterType

    /** The default filter mode is deny-only-chars, with the filter string
      * being empty, meaning that no characters are filtered out.
      * @brief Default constructor.
      */
    inline CharacterFilter ()
    {
        m_filter_type = DENY;
    }
    /** @brief Construct a filter with the given type and filter string.
      * @param filter_type The filter type to use.
      * @param filter The string containing the filter characters.
      */
    inline CharacterFilter (
        FilterType const filter_type,
        std::string const &filter)
    {
        m_filter_type = filter_type;
        m_filter = filter;
    }
    /** @brief Boring old destructor.
      */
    inline ~CharacterFilter () { }

    /** @brief Returns the filter type.
      */
    inline FilterType GetFilterType () const
    {
        return m_filter_type;
    }
    /** @brief Returns the string containing the filter characters.
      */
    inline std::string const &GetFilter () const
    {
        return m_filter;
    }
    /** If the given character is filtered out, '\0' will be returned.
      * @brief Returns the filtered version of the given character.
      * @param c The character to filter.
      */
    char GetFilteredCharacter (char c) const;

    /** @brief Sets the filter type.
      * @brief param filter_type The filter type to use.
      */
    inline void SetFilterType (FilterType const filter_type)
    {
        m_filter_type = filter_type;
    }
    /** @brief Sets the string containing the filter chars.
      * @param filter The new filter string to use.
      */
    inline void SetFilter (std::string const &filter)
    {
        m_filter = filter;
    }

private:

    // indicates if the characters in the filter will be allowed or denied
    FilterType m_filter_type;
    // contains the filtered characters
    std::string m_filter;
}; // end of class CharacterFilter

// convenience defines for character filtering
#define SIGNED_INTEGER_CHARACTER_FILTER           "0123456789-"
#define SIGNED_INTEGER_CHARACTER_FILTER_TYPE      (CharacterFilter::ALLOW)
#define UNSIGNED_INTEGER_CHARACTER_FILTER         "0123456789"
#define UNSIGNED_INTEGER_CHARACTER_FILTER_TYPE    (CharacterFilter::ALLOW)
#define DECIMAL_NOTATION_CHARACTER_FILTER         "0123456789-."
#define DECIMAL_NOTATION_CHARACTER_FILTER_TYPE    (CharacterFilter::ALLOW)
#define SCIENTIFIC_NOTATION_CHARACTER_FILTER      "0123456789-.eE+"
#define SCIENTIFIC_NOTATION_CHARACTER_FILTER_TYPE (CharacterFilter::ALLOW)

} // end of namespace Xrb

#endif // !defined(_XRB_CHARACTERFILTER_HPP_)

