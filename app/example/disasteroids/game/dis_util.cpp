// ///////////////////////////////////////////////////////////////////////////
// dis_util.cpp by Victor Dods, created 2006/03/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_util.h"

#include "dis_gameobject.h"
#include "dis_physicshandler.h"

using namespace Xrb;

namespace Dis
{

void RadiusDamage (
    PhysicsHandler *const physics_handler,
    Engine2::ObjectLayer *const object_layer,
    GameObject *const damager,
    GameObject *const damage_medium,
    Float const damage_amount,
    FloatVector2 const &damage_area_center,
    Float const damage_area_radius,
    Mortal::DamageType const damage_type,
    GameObjectReference<Mortal> const &ignore_this_mortal,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(physics_handler != NULL)
    ASSERT1(object_layer != NULL)
    ASSERT1(damage_amount >= 0.0f)
    ASSERT1(damage_area_radius > 0.0f)
    if (damage_amount == 0.0f)
        return;

    AreaTraceList area_trace_list;
    physics_handler->AreaTrace(
        object_layer,
        damage_area_center,
        damage_area_radius,
        false,
        &area_trace_list);
        
    for (AreaTraceListIterator it = area_trace_list.begin(),
                               it_end = area_trace_list.end();
         it != it_end;
         ++it)
    {
        GameObject *game_object = *it;
        ASSERT1(game_object != NULL)

        // damage mortals, unless it is the one to ignore.
        if (game_object->GetIsMortal() && game_object != *ignore_this_mortal)
        {
            // the damage tapers off with distance, so calculate how much
            // damage to apply (taking into account the size of the mortal)
            Float distance =
                Max(0.0f,
                    (game_object->GetTranslation() - damage_area_center).GetLength() -
                     game_object->GetScaleFactor());
            Float distance_ratio = distance / damage_area_radius;
            ASSERT1(distance_ratio >= 0.0f)
            Float damage_to_apply;
            if (distance_ratio >= 1.0f)
                damage_to_apply = 0.0f;
            else if (distance_ratio > 0.5f)
                damage_to_apply = damage_amount * (1.0f - distance_ratio);
            else
                damage_to_apply = damage_amount;

            static_cast<Mortal *>(game_object)->Damage(
                damager,
                damage_medium,
                damage_to_apply,
                NULL,
                damage_area_center, // maybe calculate this for real
                FloatVector2::ms_zero, // maybe calculate this for real
                0.0f,
                damage_type,
                time,
                frame_dt);
        }
    }
}

void RadiusKnockback (
    PhysicsHandler *const physics_handler,
    Engine2::ObjectLayer *const object_layer,
    FloatVector2 const &knockback_area_center,
    Float const knockback_area_radius,
    Float const power,
    Float const time,
    Float const frame_dt)
{
    ASSERT1(knockback_area_radius > 0.0f)
    ASSERT1(power > 0.0f)

    AreaTraceList area_trace_list;
    physics_handler->AreaTrace(
        object_layer,
        knockback_area_center,
        knockback_area_radius,
        false,
        &area_trace_list);

    // iterate through the trace set and apply forces
    for (AreaTraceListIterator it = area_trace_list.begin(),
                               it_end = area_trace_list.end();
         it != it_end;
         ++it)
    {
        GameObject *game_object = *it;

        if (game_object == NULL)
            continue;
            
        if (game_object->GetCollisionType() == Engine2::CT_NONSOLID_COLLISION)
            continue;
    
        // center_to_center points towards the collider
        FloatVector2 center_to_center = game_object->GetTranslation() - knockback_area_center;
        Float distance = center_to_center.GetLength() - game_object->GetScaleFactor();
        if (distance < 0.0f)
            distance = 0.0f;
        Float distance_factor;
        if (distance < 1.0f)
            distance_factor = 1.0f;
        else
            distance_factor = 1.0f / distance;
    
        // knockback forces
        if (!center_to_center.GetIsZero())
        {
            static Float const s_knockback_factor = 20.0f;
            Float knockback_momentum = s_knockback_factor * power * distance_factor;
            game_object->AccumulateMomentum(
                knockback_momentum *
                center_to_center.GetNormalization() *
                Math::Sqrt(game_object->GetScaleFactor()));
        }
    }
}

std::string GetFormattedTimeString (Float const time)
{
    Uint32 game_time_seconds = static_cast<Uint32>(time);
    Uint32 minutes = game_time_seconds / 60;
    Uint32 seconds = game_time_seconds % 60;
    Uint32 centiseconds = static_cast<Uint32>(100.0f * time) % 100;
    return Util::StringPrintf("%02u:%02u.%02u", minutes, seconds, centiseconds);
}

} // end of namespace Dis
