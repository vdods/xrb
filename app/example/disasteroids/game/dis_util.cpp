// ///////////////////////////////////////////////////////////////////////////
// dis_util.cpp by Victor Dods, created 2006/03/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_util.hpp"

#include "dis_entity.hpp"
#include "xrb_engine2_circle_physicshandler.hpp"

using namespace Xrb;

namespace Dis {

void RadiusDamage (
    Engine2::Circle::PhysicsHandler &physics_handler,
    Engine2::ObjectLayer &object_layer,
    Entity *damager,
    Entity *damage_medium,
    Float damage_amount,
    FloatVector2 const &damage_area_center,
    Float damage_area_radius,
    Mortal::DamageType damage_type,
    EntityReference<Mortal> const &ignore_this_mortal,
    Time time,
    Time::Delta frame_dt)
{
    ASSERT1(damage_amount >= 0.0f);
    ASSERT1(damage_area_radius > 0.0f);
    if (damage_amount == 0.0f)
        return;

    Engine2::Circle::AreaTraceList area_trace_list;
    physics_handler.AreaTrace(
        object_layer,
        damage_area_center,
        damage_area_radius,
        false,
        area_trace_list);

    for (Engine2::Circle::AreaTraceList::iterator it = area_trace_list.begin(), it_end = area_trace_list.end(); it != it_end; ++it)
    {
        ASSERT1(*it != NULL);
        Entity &entity = *DStaticCast<Entity *>(*it);

        // damage mortals, unless it is the one to ignore.
        if (entity.IsMortal() && &entity != *ignore_this_mortal)
        {
            // the damage tapers off with distance, so calculate how much
            // damage to apply (taking into account the size of the mortal)
            Float distance =
                Max(0.0f,
                    (entity.Translation() - damage_area_center).Length() -
                     entity.PhysicalRadius());
            Float distance_ratio = distance / damage_area_radius;
            ASSERT1(distance_ratio >= 0.0f);
            Float damage_to_apply;
            if (distance_ratio >= 1.0f)
                damage_to_apply = 0.0f;
            else if (distance_ratio > 0.5f)
                damage_to_apply = damage_amount * (1.0f - distance_ratio);
            else
                damage_to_apply = damage_amount;

            DStaticCast<Mortal *>(&entity)->Damage(
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
    Engine2::Circle::PhysicsHandler &physics_handler,
    Engine2::ObjectLayer &object_layer,
    FloatVector2 const &knockback_area_center,
    Float knockback_area_radius,
    Float power,
    Time time,
    Time::Delta frame_dt)
{
    ASSERT1(knockback_area_radius > 0.0f);
    ASSERT1(power > 0.0f);

    Engine2::Circle::AreaTraceList area_trace_list;
    physics_handler.AreaTrace(
        object_layer,
        knockback_area_center,
        knockback_area_radius,
        false,
        area_trace_list);

    // iterate through the trace set and apply forces
    for (Engine2::Circle::AreaTraceList::iterator it = area_trace_list.begin(), it_end = area_trace_list.end(); it != it_end; ++it)
    {
        ASSERT1(*it != NULL);
        Entity &entity = *DStaticCast<Entity *>(*it);

        if (entity.GetCollisionType() == Engine2::Circle::CT_NONSOLID_COLLISION)
            continue;

        // center_to_center points towards the collider
        FloatVector2 center_to_center = entity.Translation() - knockback_area_center;
        Float distance = center_to_center.Length() - entity.PhysicalRadius();
        if (distance < 0.0f)
            distance = 0.0f;
        Float distance_factor;
        if (distance < 1.0f)
            distance_factor = 1.0f;
        else
            distance_factor = 1.0f / distance;

        // knockback forces
        if (!center_to_center.IsZero())
        {
            static Float const s_knockback_factor = 20.0f;
            Float knockback_momentum = s_knockback_factor * power * distance_factor;
            entity.AccumulateMomentum(
                knockback_momentum *
                center_to_center.Normalization() *
                Math::Sqrt(entity.PhysicalRadius()));
        }
    }
}

std::string FormattedTimeString (Time time)
{
    Uint32 game_time_seconds = static_cast<Uint32>(time.AsDouble());
    Uint32 minutes = game_time_seconds / 60;
    Uint32 seconds = game_time_seconds % 60;
    Uint32 centiseconds = static_cast<Uint32>(100.0 * time.AsDouble()) % 100;
    return Util::StringPrintf("%02u:%02u.%02u", minutes, seconds, centiseconds);
}

} // end of namespace Dis
