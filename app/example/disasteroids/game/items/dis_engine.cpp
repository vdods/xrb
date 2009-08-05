// ///////////////////////////////////////////////////////////////////////////
// dis_engine.cpp by Victor Dods, created 2005/11/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "dis_engine.hpp"

#include "dis_entity.hpp"
#include "dis_ship.hpp"
#include "xrb_math.hpp"

using namespace Xrb;

namespace Dis
{

Float const Engine::ms_max_primary_power_output_rate[UPGRADE_LEVEL_COUNT] = { 66.6f, 90.0f, 125.0f, 225.0f };
Float const Engine::ms_max_auxiliary_power_output_rate[UPGRADE_LEVEL_COUNT] = { 80.0f, 160.0f, 240.0f, 320.0f };
Float const Engine::ms_max_thrust_force[UPGRADE_LEVEL_COUNT] = { 15000.0f, 40000.0f, 100000.0f, 250000.0f };

void Engine::SetInputs (
    Float const right_left_input,
    Float const up_down_input,
    Float const auxiliary_input)
{
    ASSERT1(right_left_input >= -1.0f && right_left_input <= 1.0f);
    ASSERT1(up_down_input >= -1.0f && up_down_input <= 1.0f);
    ASSERT1(auxiliary_input >= 0.0f && auxiliary_input <= 1.0f);

    m_right_left_input = right_left_input;
    m_up_down_input = up_down_input;
    m_auxiliary_input = auxiliary_input;
}

Float Engine::GetPowerToBeUsedBasedOnInputs (
    Float const time,
    Float const frame_dt) const
{
    // the auxiliary function overrides the right/left/up/down function
    if (m_auxiliary_input > 0.0f)
        return frame_dt * ms_max_auxiliary_power_output_rate[GetUpgradeLevel()] * m_auxiliary_input;

    // normalize from the square input domain of [-1, 1] [-1, 1] to a circle
        
    FloatVector2 input_vector(m_up_down_input, -m_right_left_input);
    Float input_vector_angle = Math::Atan(input_vector);
    ASSERT1(input_vector_angle >= -180.0f && input_vector_angle <= 180.0f);
    
    Float input_vector_max_length;
    if (input_vector_angle >= -180.0f && input_vector_angle <= -135.0f)
        input_vector_max_length = -1.0f / Math::Cos(input_vector_angle);
    else if (input_vector_angle <= -45.0f)
        input_vector_max_length = -1.0f / Math::Sin(input_vector_angle);
    else if (input_vector_angle <= 45.0f)
        input_vector_max_length =  1.0f / Math::Cos(input_vector_angle);
    else if (input_vector_angle <= 135.0f)
        input_vector_max_length =  1.0f / Math::Sin(input_vector_angle);
    else
        input_vector_max_length = -1.0f / Math::Cos(input_vector_angle);

    return frame_dt * ms_max_primary_power_output_rate[GetUpgradeLevel()] * input_vector.GetLength() / input_vector_max_length;
}

bool Engine::Activate (
    Float const power,
    Float const time,
    Float const frame_dt)
{
    // the auxiliary function overrides the right/left/up/down function
    if (m_auxiliary_input > 0.0f)
    {
        ASSERT1(power <= frame_dt * ms_max_auxiliary_power_output_rate[GetUpgradeLevel()] * m_auxiliary_input + 0.001f);
    
        static Float const s_survey_area_radius = 100.0f;
        Float const max_thrust_force = ms_max_thrust_force[GetUpgradeLevel()] * m_auxiliary_input;

        // store the ambient velocity, ignoring the presence of the owner ship
        FloatVector2 ambient_velocity = GetOwnerShip()->GetAmbientVelocity(s_survey_area_radius, NULL);
        // calculate what thrust is required to match the ambient velocity
        FloatVector2 velocity_differential =
            ambient_velocity -
            (GetOwnerShip()->GetVelocity() +
            frame_dt / GetOwnerShip()->GetFirstMoment() * GetOwnerShip()->GetForce());
        FloatVector2 thrust_vector = GetOwnerShip()->GetFirstMoment() * velocity_differential / frame_dt;
        // if the thrust isn't zero, cap it to the max thrust, and accumulate
        if (!thrust_vector.IsZero())
        {
            Float thrust_force = thrust_vector.GetLength();
            if (thrust_force > max_thrust_force)
                thrust_vector = max_thrust_force * thrust_vector.GetNormalization();
    
            GetOwnerShip()->AccumulateForce(thrust_vector);
        }
    }
    else
    {
        // normalize from the square input domain of [-1, 1] [-1, 1] to a circle

        FloatVector2 input_vector(m_up_down_input, -m_right_left_input);
        Float input_vector_angle = Math::Atan(input_vector);
        ASSERT1(input_vector_angle >= -180.0f && input_vector_angle <= 180.0f);
        
        Float input_vector_max_length;
        if (input_vector_angle >= -180.0f && input_vector_angle <= -135.0f)
            input_vector_max_length = -1.0f / Math::Cos(input_vector_angle);
        else if (input_vector_angle <= -45.0f)
            input_vector_max_length = -1.0f / Math::Sin(input_vector_angle);
        else if (input_vector_angle <= 45.0f)
            input_vector_max_length =  1.0f / Math::Cos(input_vector_angle);
        else if (input_vector_angle <= 135.0f)
            input_vector_max_length =  1.0f / Math::Sin(input_vector_angle);
        else
            input_vector_max_length = -1.0f / Math::Cos(input_vector_angle);

        ASSERT1(power <= frame_dt * ms_max_primary_power_output_rate[GetUpgradeLevel()] * input_vector_max_length + 0.001f);
            
        Float output_ratio = power / (frame_dt * ms_max_primary_power_output_rate[GetUpgradeLevel()]);
        Float thrust_force =
            output_ratio * ms_max_thrust_force[GetUpgradeLevel()] *
            input_vector.GetLength() / input_vector_max_length;
        FloatVector2 thrust_vector(thrust_force * Math::UnitVector(input_vector_angle + GetOwnerShip()->GetAngle()));

        GetOwnerShip()->AccumulateForce(thrust_vector);
    }

    return true;
}

} // end of namespace Dis
