// ///////////////////////////////////////////////////////////////////////////
// dis_powerup.hpp by Victor Dods, created 2005/12/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_POWERUP_HPP_)
#define _DIS_POWERUP_HPP_

#include "dis_entity.hpp"

#include "dis_item.hpp"

using namespace Xrb;

namespace Dis
{

class Powerup : public Entity
{
public:

    Powerup (ItemType const item_type)
        :
        Entity(ET_POWERUP, CT_SOLID_COLLISION)
    {
        m_item_type = item_type;
        m_item = NULL;
        m_effective_coefficient = 1.0f;
        m_delete_upon_next_think = false;
        m_has_been_picked_up = false;
    }
    Powerup (Item *const item)
        :
        Entity(ET_POWERUP, CT_SOLID_COLLISION)
    {
        ASSERT1(item != NULL);
        m_item_type = IT_COUNT;
        m_item = item;
        m_effective_coefficient = 1.0f;
        m_has_been_picked_up = false;
    }
    virtual ~Powerup ()
    {
        Delete(m_item);
    }

    // Entity interface method
    virtual bool IsPowerup () const { return true; }

    inline ItemType GetItemType () const
    {
        if (m_item != NULL)
            return m_item->GetItemType();
        else
            return m_item_type;
    }
    inline Item *GetItem () const { return m_item; }
    inline Float EffectiveCoefficient () const { return m_effective_coefficient; }
    inline Float EffectiveValue () const { return m_effective_coefficient * GetFirstMoment(); }

    // this sets the effective coefficient directly (and indirectly sets the effective value)
    void SetEffectiveCoefficient (Float effective_coefficient);
    // this sets the effective value directly (and indirectly sets the effective coefficient)
    void SetEffectiveValue (Float effective_value);

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

private:

    ItemType m_item_type;
    Item *m_item;
    Float m_effective_coefficient;
    bool m_delete_upon_next_think;
    bool m_has_been_picked_up;
}; // end of class Powerup

} // end of namespace Dis

#endif // !defined(_DIS_POWERUP_HPP_)

