// ///////////////////////////////////////////////////////////////////////////
// xrb_sizeproperties.hpp by Victor Dods, created 2005/06/12
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_SIZEPROPERTIES_HPP_)
#define _XRB_SIZEPROPERTIES_HPP_

#include "xrb.hpp"

#include "xrb_ntuple.hpp"
#include "xrb_screencoord.hpp"

namespace Xrb
{

/** Is also used in various methods of Layout for optimal area allocation.
  * @brief Stores the minimum and maximum size properties of a Widget.
  */
class SizeProperties
{
public:

    /** The properties are minimum size, minimum size enabled, maximum size,
      * and maximum size enabled.  This enum denotes half of the necessary
      * qualifiers for all properties.
      * @brief Enumeration to specify which half of the properties to change.
      */
    enum Property
    {
        MIN = 0,
        MAX
    }; // end of enum Property

    /** An ordered pair of booleans, applying to the X and Y dimensions,
      * respectively.
      * @brief Indicates if there is a minimum size (for each dimension).
      */
    Bool2 m_min_size_enabled;
    /** If the corresponding component in m_min_size_enabled is false, then
      * the size is not used (but it is saved for if/when the min size
      * is re-enabled).
      * @brief Contains the minimum size for each dimension in screen
      *        coordinates.
      */
    ScreenCoordVector2 m_min_size;
    /** An ordered pair of booleans, applying to the X and Y dimensions,
      * respectively.
      * @brief Indicates if there is a maximum size (for each dimension).
      */
    Bool2 m_max_size_enabled;
    /** If the corresponding component in m_max_size_enabled is false, then
      * the size is not used (but it is saved for if/when the max size
      * is re-enabled).
      * @brief Contains the maximum size for each dimension in screen
      *        coordinates.
      */
    ScreenCoordVector2 m_max_size;
    /** @brief A pointer to a piece of data used in widget layout code.
      */
    ScreenCoord const *m_data;

    /** Sets the min/max size enabled flags to false, and the min/max
      * sizes to their neutral defaults (see DefaultMinSizeComponent
      * and DefaultMaxSizeComponent).
      * @brief Default constructor.
      */
    SizeProperties ();
    /** Does nothing.
      * @brief Destructor.
      */
    inline ~SizeProperties () { }

    /** This is used more extensively in Layout for optimal area allocation.
      * @brief Returns the smallest valid minimum size component.
      */
    static inline ScreenCoord DefaultMinSizeComponent ()
    {
        return 0;
    }
    /** This is used more extensively in Layout for optimal area allocation.
      * @brief Returns the largest valid maximum size component.
      */
    static inline ScreenCoord DefaultMaxSizeComponent ()
    {
        return SINT32_UPPER_BOUND;
    }
    /** @brief Returns the given size coordinate, adjusted by the constraints
      *        of this SizeProperties object.
      * @param index Indicates which dimension the component value applies to.
      * @param component The screen coordinate to adjust.
      */
    ScreenCoord AdjustedSizeComponent (
        Uint32 index,
        ScreenCoord component) const;
    /** @brief Returns the given size vector, adjusted by the constraints of
      *        this SizeProperties object.
      * @param size The size vector to adjust.
      */
    ScreenCoordVector2 AdjustedSize (
        ScreenCoordVector2 const &size) const;

    /** @brief In-place version of AdjustedSizeComponent.
      * @param index Indicates which dimension the component value applies to.
      * @param component A pointer to the screen coordinate to adjust.
      */
    void AdjustSizeComponent (Uint32 index, ScreenCoord *component) const;
    /** @brief In-place version of AdjustedSize.
      * @param size A pointer to the size vector to adjust.
      */
    void AdjustSize (ScreenCoordVector2 *size) const;
}; // end of class SizeProperties

} // end of namespace Xrb

#endif // !defined(_XRB_SIZEPROPERTIES_HPP_)

