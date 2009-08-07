// ///////////////////////////////////////////////////////////////////////////
// dis_areatracelist.hpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_AREATRACELIST_HPP_)
#define _DIS_AREATRACELIST_HPP_

#include "xrb.hpp"

#include <list>

using namespace Xrb;

namespace Dis
{

class Entity;

typedef std::list<Entity *> AreaTraceList;

} // end of namespace Dis

#endif // !defined(_DIS_AREATRACELIST_HPP_)

