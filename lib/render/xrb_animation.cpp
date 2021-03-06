// ///////////////////////////////////////////////////////////////////////////
// xrb_animation.cpp by Victor Dods, created 2009/09/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb_animation.hpp"

#include <sstream>

#include "xrb_gltexture.hpp"
#include "xrb_math.hpp"
#include "xrb_parse_datafile.hpp"
#include "xrb_parse_datafile_value.hpp"
#include "xrb_util.hpp"

namespace Xrb {

using namespace Parse;

// ///////////////////////////////////////////////////////////////////////////
// Animation::Sequence::LoadParameters
// ///////////////////////////////////////////////////////////////////////////

std::string Animation::Sequence::LoadParameters::ResourceName () const
{
    return "Xrb::Animation::Sequence";
}

bool Animation::Sequence::LoadParameters::IsLessThan (ResourceLoadParameters const &p) const
{
    LoadParameters const &rhs = p.As<LoadParameters>();
    return m_path.compare(rhs.m_path) < 0;
}

bool Animation::Sequence::LoadParameters::IsFallback () const
{
    return m_path == "internal://missing";
}

void Animation::Sequence::LoadParameters::Fallback ()
{
    // sentinel value for the internal (non-filesystem based) "missing" animation
    m_path = "internal://missing";
}

void Animation::Sequence::LoadParameters::Print (std::ostream &stream) const
{
    stream << "path = \"" << m_path << '"';
}

// ///////////////////////////////////////////////////////////////////////////
// Animation::Sequence
// ///////////////////////////////////////////////////////////////////////////

Animation::Sequence::Sequence (Uint32 length, AnimationType default_type, Time::Delta default_duration)
    :
    m_length(length),
    m_frame(new Resource<GlTexture>[length]),
    m_default_type(default_type),
    m_default_duration(default_duration)
{
    ASSERT1(m_length > 0 && "can't have an empty Sequence");
    ASSERT1(m_default_type >= 0 && m_default_type < AT_COUNT && "invalid AnimationType");
    ASSERT1(m_default_duration > 0.0f);
}

Animation::Sequence::Sequence (Resource<GlTexture> const &single_frame)
    :
    m_length(1),
    m_frame(new Resource<GlTexture>[1]),
    m_default_type(CYCLE_FORWARD), // arbitrary, since it's only 1 frame anyway
    m_default_duration(1.0f)       // arbitrary, since it's only 1 frame anyway
{
    m_frame[0] = single_frame;
}

Animation::Sequence::~Sequence ()
{
    delete[] m_frame;
}

Animation::Sequence *Animation::Sequence::Create (ResourceLoadParameters const &p)
{
    LoadParameters const &load_parameters = p.As<LoadParameters>();

    // the "missing" animation is a two-frame animation consisting of "internal://missing" and "internal://missing_alternate".
    if (load_parameters.Path() == "internal://missing")
    {
        // create a 1-frame animation using the "missing" texture
        Sequence *retval = new Sequence(2, CYCLE_FORWARD, 0.5f);
        retval->m_frame[0] = GlTexture::Load("internal://missing");
        retval->m_frame[1] = GlTexture::Load("internal://missing_alternate");
        return retval;
    }

    // otherwise try to load and parse the animation descriptor file
    Resource<DataFile::Structure> root(Parse::DataFile::Load(load_parameters.Path()));
    ASSERT1(root.IsValid());
    if (root.LoadParameters().IsFallback())
    {
        load_parameters.SetErrorMessage("error while loading/parsing animation descriptor file");
        return NULL;
    }

    try
    {
        std::string default_type_string;
        try { default_type_string = root->PathElementString("|default_type"); } catch (...) { }
        AnimationType default_type = AT_COUNT;
        // if not specified, the default AnimationType is CYCLE_FORWARD
        Util::MakeUppercase(default_type_string);
        if (default_type_string == "")
            default_type = CYCLE_FORWARD;
        for (Uint32 i = 0; i < AT_COUNT; ++i)
        {
            if (default_type_string == ms_animation_type_string[i])
            {
                default_type = AnimationType(i);
                break;
            }
        }
        if (default_type == AT_COUNT) // if nothing was assigned so far, it's an error
        {
            std::string acceptable_values;
            for (Uint32 i = 0; i < AT_COUNT; ++i)
                acceptable_values += " \"" + ms_animation_type_string[i] + "\"";
            throw Exception(FORMAT("invalid default_type value \"" << default_type_string << "\" (acceptable values are" << acceptable_values << ")"));
        }

        Time::Delta default_duration = 1.0f; // the value is 1 if left unspecified
        try { default_duration = root->PathElementFloaty("|default_duration"); } catch (...) { }
        if (default_duration <= 0.0f)
            throw Exception(FORMAT("invalid default_duration value " << default_duration << " (must be positive)"));

        std::cerr << "Animation::Sequence::Create(\"" << load_parameters.Path() << "\"); default_type = " << ms_animation_type_string[default_type] << ", default_duration = " << default_duration << std::endl;

        DataFile::Array const *frames = root->PathElementArray("|frames");
        Sequence *retval = new Sequence(frames->ElementCount(), default_type, default_duration);
        for (Uint32 i = 0; i < retval->Length(); ++i)
            retval->m_frame[i] = GlTexture::Load(frames->StringElement(i), load_parameters.GlTextureFlags());
        return retval;
    }
    catch (std::string const &e)
    {
//         std::cerr << "Animation::Sequence::Create(\"" << load_parameters.Path() << "\"); error \"" << e << '"' << std::endl;
        load_parameters.SetErrorMessage(e);
        return NULL;
    }
}

// ///////////////////////////////////////////////////////////////////////////
// Animation
// ///////////////////////////////////////////////////////////////////////////

std::string const Animation::ms_animation_type_string[AT_COUNT] =
{
    "CYCLE_FORWARD",
    "CYCLE_BACKWARD",
    "ONESHOT_FORWARD",
    "ONESHOT_BACKWARD",
    "PINGPONG",
    "RANDOM"
};

Animation::Animation (Resource<Animation::Sequence> const &sequence, Time sequence_start_time)
    :
    m_sequence(sequence),
    m_sequence_start_time(sequence_start_time),
    m_last_random_frame(0),
    m_last_random_frame_time(Time::ms_beginning_of)
{
    if (m_sequence.IsValid())
    {
        SetType(m_sequence->DefaultType());
        SetDuration(m_sequence->DefaultDuration());
    }
    else
    {
        SetType(CYCLE_FORWARD);
        SetDuration(1.0f);
    }
}

Animation::Animation (Animation const &animation)
    :
    m_sequence(animation.m_sequence),
    m_type(animation.m_type),
    m_duration(animation.m_duration),
    m_sequence_start_time(animation.m_sequence_start_time),
    m_last_random_frame(animation.m_last_random_frame),
    m_last_random_frame_time(animation.m_last_random_frame_time)
{ }

GlTexture const &Animation::Frame (Time current_time) const
{
    ASSERT1(m_sequence.IsValid());
    ASSERT1(m_sequence->Length() > 0);
    ASSERT1(m_duration > 0.0f);
    ASSERT1(current_time >= m_sequence_start_time && "you probably don't want to go backward in time... or do you?");

    Time cycle_time(current_time.AsDouble() - m_sequence_start_time.AsDouble()); // keep it in double
    Float cycle_parameter = CalculateCycleParameter(cycle_time, m_duration);
    bool passed_at_least_one_cycle = cycle_time >= Time(m_duration);

    // this is the value which will be passed to Sequence::Frame and must
    // be determined based on the animation type (see the descriptions in
    // AnimationType).
    Uint32 current_frame_index = 0;
    switch (m_type)
    {
        case CYCLE_FORWARD:
            current_frame_index = Uint32(Math::Floor(cycle_parameter * m_sequence->Length()));
            break;

        case CYCLE_BACKWARD:
            current_frame_index = m_sequence->Length()-1 - Uint32(Math::Floor(cycle_parameter * m_sequence->Length()));
            break;

        case ONESHOT_FORWARD:
            // if one cycle has completed, stay at the last frame
            if (passed_at_least_one_cycle)
                current_frame_index = m_sequence->Length()-1;
            // otherwise progress forward through the sequence
            else
                current_frame_index = Uint32(Math::Floor(cycle_parameter * m_sequence->Length()));
            break;

        case ONESHOT_BACKWARD:
            // if one cycle has completed, stay at the first frame
            if (passed_at_least_one_cycle)
                current_frame_index = 0;
            // otherwise progress backward through the sequence
            else
                current_frame_index = m_sequence->Length()-1 - Uint32(Math::Floor(cycle_parameter * m_sequence->Length()));
            break;

        case PINGPONG:
            if (m_sequence->Length() == 1)
                current_frame_index = 0; // there is only ZUUL
            else
            {
                // for the first half of the cycle, progress forward
                if (cycle_parameter < 0.5f)
                    current_frame_index = Uint32(Math::Floor(2.0f * cycle_parameter * (m_sequence->Length()-2)));
                // for the second half, progress backward
                else
                    current_frame_index = m_sequence->Length()-1 - Uint32(Math::Floor(2.0f * (cycle_parameter-0.5f) * (m_sequence->Length()-2)));
            }
            break;

        case RANDOM:
            // if the duration has expired, use a new random frame
            if (current_time - m_last_random_frame_time >= m_duration)
            {
                m_last_random_frame = Math::RandomUint16(0, m_sequence->Length()-1);
                m_last_random_frame_time = current_time - cycle_parameter*m_duration;
            }
            current_frame_index = m_last_random_frame;
            break;

        default:
            ASSERT1(false && "invalid AnimationType");
            current_frame_index = 0;
            break;
    }

    return m_sequence->Frame(current_frame_index);
}

Float Animation::CalculateCycleParameter (Time cycle_time, Time::Delta cycle_duration)
{
    ASSERT1(cycle_time >= Time::ms_beginning_of);
    ASSERT1(cycle_duration > 0.0f);

    Float cycle_parameter = Float(fmod(cycle_time.AsDouble(), cycle_duration) / cycle_duration);
    // this seems impossible, but is necessary because fmod can return (has returned) a full divisor.
    // this defies the mathematical definition of the modulo operation, but it's what happens.
    if (cycle_parameter == 1.0f) 
        cycle_parameter = 0.0f; 
    ASSERT1(cycle_parameter >= 0.0f);
    ASSERT1(cycle_parameter < 1.0f);
    return cycle_parameter;
}

} // end of namespace Xrb
