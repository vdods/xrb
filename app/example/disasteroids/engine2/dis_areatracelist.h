// ///////////////////////////////////////////////////////////////////////////
// dis_areatracelist.h by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_AREATRACELIST_H_)
#define _DIS_AREATRACELIST_H_

#include "xrb.h"

#include <list>

using namespace Xrb;

namespace Dis
{

class GameObject;

typedef std::list<GameObject *> AreaTraceList;
typedef AreaTraceList::iterator AreaTraceListIterator;
     
} // end of namespace Dis

#endif // !defined(_DIS_AREATRACELIST_H_)

