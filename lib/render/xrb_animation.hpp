// ///////////////////////////////////////////////////////////////////////////
// xrb_animation.hpp by Victor Dods, created 2009/09/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_ANIMATION_HPP_)
#define _XRB_ANIMATION_HPP_

#include "xrb.hpp"

#include <string>

#include "xrb_resourcelibrary.hpp"

namespace Xrb
{

class GlTexture;
class ResourceLoadParameters;

class Animation
{
public:

    enum AnimationType
    {
        CYCLE_FORWARD = 0, // loop forward through the sequence, one cycle per duration
        CYCLE_BACKWARD,    // loop backward through the sequence, one cycle per duration
        ONESHOT_FORWARD,   // once forward over the sequence, taking up one duration
        ONESHOT_BACKWARD,  // once backward over the sequence, taking up one duration
        PINGPONG,          // forward then back over the sequence, once per duration
        RANDOM,            // random frames, one frame per duration

        AT_COUNT
    };

    static std::string const ms_animation_type_string[AT_COUNT];

    // Resource<>-wrappable container used for storing the animation frame data
    class Sequence
    {
    public:

        Sequence (Uint32 length, AnimationType default_type, Float default_duration);
        ~Sequence ();

        static Sequence *Create (std::string const &animation_path, ResourceLoadParameters const *load_parameters);

        Uint32 Length () const { return m_length; }
        GlTexture const &Frame (Uint32 index) const { ASSERT1(index < m_length); return **m_frame[index]; }
        AnimationType DefaultType () const { return m_default_type; }
        Float DefaultDuration () const { return m_default_duration; }

    private:

        Uint32 const m_length;
        Resource<GlTexture> *const m_frame;
        AnimationType const m_default_type;
        Float const m_default_duration;
    }; // end of class Animation::Sequence

    Animation (Resource<Sequence> const &sequence, AnimationType type, Float duration, Float sequence_start_time);

    Resource<Sequence> const &GetSequence () const { return m_sequence; }
    AnimationType Type () const { return m_type; }
    Float Duration () const { return m_duration; }
    // this value will update as the sequence cycles
    Float SequenceStartTime () const { return m_sequence_start_time; }

    void SetSequence (Resource<Sequence> const &sequence) { ASSERT1(sequence.IsValid()); m_sequence = sequence; m_data = 0; }
    void SetType (AnimationType type) { ASSERT1(type >= 0 && type < AT_COUNT); m_type = type; m_data = 0; }
    void SetDuration (Float duration) { ASSERT1(duration > 0.0f); m_duration = duration; m_data = 0; }
    void SetSequenceStartTime (Float sequence_start_time) { m_sequence_start_time = sequence_start_time; m_data = 0; }

    GlTexture const &Frame (Float current_time) const;

private:

    Resource<Sequence> m_sequence;
    AnimationType m_type;
    Float m_duration;
    mutable Float m_sequence_start_time;
    mutable Uint32 m_data; // for use in the ONESHOT_* types and RANDOM
};

} // end of namespace Xrb

#endif // !defined(_XRB_ANIMATION_HPP_)

