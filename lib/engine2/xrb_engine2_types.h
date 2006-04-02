// ///////////////////////////////////////////////////////////////////////////
// xrb_engine2_types.h by Victor Dods, created 2006/04/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ENGINE2_TYPES_H_)
#define _XRB_ENGINE2_TYPES_H_

#include "xrb.h"

namespace Xrb
{

class Serializer;

namespace Engine2
{

class Entity;

typedef Sint32 EntityWorldIndex;
typedef Entity *(*CreateEntityFunction)(Serializer &);

} // end of namespace Engine2

} // end of namespace Xrb

#endif // !defined(_XRB_ENGINE2_TYPES_H_)

