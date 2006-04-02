// ///////////////////////////////////////////////////////////////////////////
// dis_powerup.h by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_POWERUP_H_)
#define _DIS_POWERUP_H_

#include "dis_entity.h"

#include "dis_item.h"

using namespace Xrb;

namespace Dis
{

class Powerup : public Entity
{
public:

    // bit flags for what sorts of ships can pick up powerups
    enum
    {
        PU_NONE          = 0x00,
    
        PU_SOLITARY      = 0x01,
        
        PU_INTERLOPER    = 0x02,
        PU_SHADE         = 0x04,
        PU_REVULSION     = 0x08,
        PU_DEVOURMENT    = 0x10,
        PU_DEMI          = 0x20,
        
        PU_ALL_ENEMIES   = PU_INTERLOPER|PU_SHADE|PU_REVULSION|PU_DEVOURMENT|PU_DEMI,
        PU_ALL_SHIPS     = PU_SOLITARY|PU_ALL_ENEMIES 
    };

    Powerup (
        ItemType const item_type,
        Uint8 const pickup_flags)
        :
        Entity(ET_POWERUP, CT_SOLID_COLLISION)
    {
        m_delete_upon_next_think = false;
        m_item_type = item_type;
        m_item = NULL;
        SetPickupFlags(pickup_flags);
    }
    Powerup (
        Item *const item,
        Uint8 const pickup_flags)
        :
        Entity(ET_POWERUP, CT_SOLID_COLLISION)
    {
        ASSERT1(item != NULL)
        m_item_type = IT_COUNT;
        m_item = item;
        SetPickupFlags(pickup_flags);
    }
    virtual ~Powerup () { }

    // Entity interface method
    virtual bool GetIsPowerup () const { return true; }
    
    inline ItemType GetItemType () const
    {
        if (m_item != NULL)
            return m_item->GetType();
        else
            return m_item_type;
    }
    inline Item *GetItem () const { return m_item; }
    inline Uint8 GetPickupFlags () const { return m_pickup_flags; }

    inline void ClearItem ()
    {
        m_item_type = IT_COUNT;
        m_item = NULL;
    }

    virtual void Think (Float time, Float frame_dt);
    virtual void Collide (
        Entity *collider,
        FloatVector2 const &collision_location,
        FloatVector2 const &collision_normal,
        Float collision_force,
        Float time,
        Float frame_dt);

protected:

    inline void SetPickupFlags (Uint8 const pickup_flags)
    {
        ASSERT1(pickup_flags != 0)
        ASSERT1((pickup_flags & ~PU_ALL_SHIPS) == 0)
        m_pickup_flags = pickup_flags;
    }
            
private:

    bool m_delete_upon_next_think;
    ItemType m_item_type;
    Item *m_item;
    Uint8 m_pickup_flags;
}; // end of class Powerup
        
} // end of namespace Dis

#endif // !defined(_DIS_POWERUP_H_)

