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
#include "xrb_parse_datafile_parser.hpp"
#include "xrb_parse_datafile_value.hpp"
#include "xrb_util.hpp"

namespace Xrb
{

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
    LoadParameters const &rhs = *DStaticCast<LoadParameters const *>(&p);
    return m_path.compare(rhs.m_path) < 0;
}

void Animation::Sequence::LoadParameters::Fallback ()
{
    // an empty path indicates the "missing" animation should be loaded.
    m_path.clear();
}

void Animation::Sequence::LoadParameters::Print (FILE *fptr) const
{
    fprintf(fptr, "path = \"%s\"", m_path.c_str());
}

// ///////////////////////////////////////////////////////////////////////////
// Animation::Sequence
// ///////////////////////////////////////////////////////////////////////////

Animation::Sequence::Sequence (Uint32 length, AnimationType default_type, Float default_duration)
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

Animation::Sequence::~Sequence ()
{
    delete[] m_frame;
}

Animation::Sequence *Animation::Sequence::Create (ResourceLoadParameters const &p)
{
    LoadParameters const &load_parameters = *DStaticCast<LoadParameters const *>(&p);

    // an empty path indicates that the "missing" animation should be created.
    if (load_parameters.Path().empty())
    {
        // create a 1-frame animation using the "missing" texture
        Sequence *retval = new Sequence(1, CYCLE_FORWARD, 1.0f);
        retval->m_frame[0] = GlTexture::LoadMissing();
        return retval;
    }

    // otherwise try to load and parse the animation descriptor file
    DataFile::Parser parser;
    DataFile::Parser::ReturnCode return_code = parser.Parse(load_parameters.Path());
    if (return_code == DataFile::Parser::RC_SUCCESS)
    {
        DataFile::Structure const *root = parser.AcceptedStructure();
        try
        {
            std::string default_type_string;
            try { default_type_string = root->PathElementString("|default_type"); } catch (...) { }
            AnimationType default_type = AT_COUNT;
            // if not specified, the default AnimationType is CYCLE_FORWARD
            Util::MakeUppercase(&default_type_string);
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
                THROW_STRING("invalid default_type value \"" << default_type_string << "\" (acceptable values are" << acceptable_values << ")");
            }

            Float default_duration = 1.0f; // the value is 1 if left unspecified
            try { default_duration = root->PathElementFloaty("|default_duration"); } catch (...) { }
            if (default_duration <= 0.0f)
                THROW_STRING("invalid default_duration value " << default_duration << " (must be positive)");

            fprintf(stderr, "Animation::Sequence::Create(\"%s\"); default_type = %s, default_duration = %g\n", load_parameters.Path().c_str(), ms_animation_type_string[default_type].c_str(), default_duration);

            DataFile::Array const *frames = root->PathElementArray("|frames");
            Sequence *retval = new Sequence(frames->ElementCount(), default_type, default_duration);
            for (Uint32 i = 0; i < retval->Length(); ++i)
                retval->m_frame[i] = GlTexture::Load(frames->StringElement(i));
            return retval;
        }
        catch (std::string const &e)
        {
            fprintf(stderr, "Animation::Sequence::Create(\"%s\"); error \"%s\"\n", load_parameters.Path().c_str(), e.c_str());
        }
    }
    else
    {
        fprintf(stderr, "Animation::Sequence::Create(\"%s\"); error ", load_parameters.Path().c_str());
        switch (return_code)
        {
            case DataFile::Parser::RC_INVALID_FILENAME: fprintf(stderr, "RC_INVALID_FILENAME\n"); break;
            case DataFile::Parser::RC_FILE_OPEN_FAILURE: fprintf(stderr, "RC_FILE_OPEN_FAILURE\n"); break;
            case DataFile::Parser::RC_PARSE_ERROR: fprintf(stderr, "RC_PARSE_ERROR\n"); break;
            case DataFile::Parser::RC_ERRORS_ENCOUNTERED: fprintf(stderr, "RC_ERRORS_ENCOUNTERED\n"); break;
            default: ASSERT1(false && "unhandled DataFile::Parser::ReturnCode"); break;
        }
    }
    return NULL;
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

Animation::Animation (
    Resource<Animation::Sequence> const &sequence,
    AnimationType type,
    Float duration,
    Float sequence_start_time)
    :
    m_sequence(sequence),
    m_sequence_start_time(sequence_start_time),
    m_data(0)
{
    ASSERT1(m_sequence.IsValid() && "can't specify an invalid animation sequence");
    SetType(type);
    SetDuration(duration);
}

GlTexture const &Animation::Frame (Float current_time) const
{
    ASSERT1(m_sequence.IsValid());
    ASSERT1(m_sequence->Length() > 0);
    ASSERT1(m_duration > 0.0f);
    ASSERT1(current_time >= m_sequence_start_time && "you probably don't want to go backward in time");

    Float cycle_time = current_time - m_sequence_start_time;
    Uint32 passed_cycle_count = Uint32(Math::Floor(cycle_time / m_duration));
    Float cycle_parameter = (cycle_time - m_duration * passed_cycle_count) / m_duration;
    ASSERT1(cycle_parameter >= 0.0f);
    ASSERT1(cycle_parameter < 1.0f);
    // don't let m_sequence_start_time lag too much behind, because eventually
    // it could have floating point errors if cycle_time is too big.
    m_sequence_start_time += m_duration * passed_cycle_count;

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
            if (passed_cycle_count > 0)
                m_data = 1;
            if (m_data != 0)
                current_frame_index = m_sequence->Length()-1;
            else
                current_frame_index = Uint32(Math::Floor(cycle_parameter * m_sequence->Length()));
            break;

        case ONESHOT_BACKWARD:
            if (passed_cycle_count > 0)
                m_data = 1;
            if (m_data != 0)
                current_frame_index = 0;
            else
                current_frame_index = m_sequence->Length()-1 - Uint32(Math::Floor(cycle_parameter * m_sequence->Length()));
            break;

        case PINGPONG:
            if (m_sequence->Length() == 1)
                current_frame_index = 0; // there is only ZUUL
            else
            {
                if (cycle_parameter < 0.5f)
                    current_frame_index = Uint32(Math::Floor(2.0f * cycle_parameter * (m_sequence->Length()-2)));
                else
                    current_frame_index = m_sequence->Length()-1 - Uint32(Math::Floor(2.0f * (cycle_parameter-0.5f) * (m_sequence->Length()-2)));
            }
            break;

        case RANDOM:
            if (passed_cycle_count > 0)
                m_data = Math::RandomUint16(0, m_sequence->Length()-1);
            current_frame_index = m_data;
            break;

        default:
            ASSERT1(false && "invalid AnimationType");
            current_frame_index = 0;
            break;
    }

    return m_sequence->Frame(current_frame_index);
}

} // end of namespace Xrb
