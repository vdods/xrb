// ///////////////////////////////////////////////////////////////////////////
// xrb_enums.h by Victor Dods, created 2005/06/12
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENUMS_H_)
#define _XRB_ENUMS_H_

#include "xrb.h"

namespace Xrb
{

/** @brief Used for pendantically self-documenting vector indexing.
  */
namespace Dim
{
    /** @brief Gives textual names to the three most common vector components,
      *        as well as the 4 color components (RGBA).
      */
    enum Component
    {
        X = 0,     ///< The X-component index
        Y,         ///< The Y-component index
        Z,         ///< The Z-component index

        R = 0,     ///< The red color component index
        G,         ///< The green color component index
        B,         ///< The blue color component index
        A,         ///< The alpha color component index

        RED = R,   ///< The red color component index
        GREEN = G, ///< The green color component index
        BLUE = B,  ///< The blue color component index
        ALPHA = A  ///< The alpha color component index
    }; // end of enum Component
} // end of namespace Dim

/** @brief Enums for text-alignment in Label (also used by CellPaddingWidget).
  */
enum Alignment
{
    TOP = 0, ///< Align the top side of the content to the top side of the widget.
    LEFT,    ///< Align the left side of the content to the left side of the widget.
    CENTER,  ///< Center the content in the widget.
    BOTTOM,  ///< Align the bottom side of the content to the bottom side of the widget.
    RIGHT,   ///< Align the right side of the content to the right side of the widget.
    SPACED   ///< The text will be spaced to the left/right and/or top/bottom.
}; // end of enum Alignment

/** @brief Enums for child widget stack priority.
  */
enum StackPriority
{
    SP_STAY_ON_BOTTOM = 0, ///< The child will stay at the bottom of the stack.
    SP_NEUTRAL,            ///< The child will stay between the bottom and top children.
    SP_STAY_ON_TOP         ///< The child will stay at the top of the stack.
}; // end of enum StackPriority

/** @brief Enums for Layout row or column specification.
  */
enum LineDirection
{
    COLUMN = 0,
    ROW
}; // end of enum LineDirection

/** @brief Enums for Layout orientation.
  */
enum Orientation
{
    HORIZONTAL = 0,
    VERTICAL
}; // end of enum Orientation

/** See Serializer and BitCache .
  * @brief Enums used for error indication in I/O routines.
  */
enum IOError
{
    IOE_NONE = 0,
    IOE_IS_AT_END,
    IOE_INSUFFICIENT_STORAGE,
    IOE_INSUFFICIENT_AVAILABLE_DATA,
    IOE_OVERSIZED_STRING,
    IOE_INVALID_FILENAME,
    IOE_UNABLE_TO_OPEN_FILE,
    IOE_INVALID_FILE_OPEN_MODE,

    IOE_COUNT,

    IOE_LOWEST_ERROR = IOE_NONE,
    IOE_HIGHEST_ERROR = IOE_INVALID_FILE_OPEN_MODE
}; // end of enum IOError

/** See Serializer and BitCache .
  * @brief Enums used for indicating I/O direction.
  */
enum IODirection
{
    IOD_NONE = 0,
    IOD_READ,
    IOD_WRITE,

    IOD_COUNT
}; // end of enum IODirection

} // end of namespace Xrb

#endif // !defined(_XRB_ENUMS_H_)

