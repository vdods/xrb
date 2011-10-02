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
#include "xrb_time.hpp"

namespace Xrb {

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

        class LoadParameters : public ResourceLoadParameters
        {
        public:

            // specifying "internal://missing" will give the internally generated "missing" animation sequence.
            LoadParameters (std::string const &path, Uint32 gltexture_flags)
                :
                m_path(path),
                m_gltexture_flags(gltexture_flags)
            { }

            std::string const &Path () const { return m_path; }
            Uint32 GlTextureFlags () const { return m_gltexture_flags; }

            virtual std::string ResourceName () const;
            virtual bool IsLessThan (ResourceLoadParameters const &p) const;
            virtual bool IsFallback () const;
            virtual void Fallback ();
            virtual void Print (std::ostream &stream) const;

        private:

            std::string m_path;
            Uint32 m_gltexture_flags;
        }; // end of class Animation::Sequence::LoadParameters

        Sequence (Uint32 length, AnimationType default_type, Time::Delta default_duration);
        Sequence (Resource<GlTexture> const &single_frame);
        ~Sequence ();

        static Resource<Sequence> Load (std::string const &path, Uint32 gltexture_flags = 0) // 0 is GlTexture::NONE
        {
            return Singleton::ResourceLibrary().Load<Sequence>(Sequence::Create, new LoadParameters(path, gltexture_flags));
        }
        static Resource<Sequence> LoadMissing ()
        {
            LoadParameters *load_parameters = new LoadParameters("", 0); // arbitrary, since Fallback changes this.
            load_parameters->Fallback();
            return Singleton::ResourceLibrary().Load<Sequence>(Sequence::Create, load_parameters);
        }

        static Sequence *Create (ResourceLoadParameters const &p);

        Uint32 Length () const { return m_length; }
        GlTexture const &Frame (Uint32 index) const { ASSERT1(index < m_length); return **m_frame[index]; }
        AnimationType DefaultType () const { return m_default_type; }
        Time::Delta DefaultDuration () const { return m_default_duration; }

    private:

        Uint32 const m_length;
        Resource<GlTexture> *const m_frame;
        AnimationType const m_default_type;
        Time::Delta const m_default_duration;
    }; // end of class Animation::Sequence

    Animation (Resource<Sequence> const &sequence, Time sequence_start_time = Time::ms_beginning_of);
    Animation (Animation const &animation);

    Resource<Sequence> const &GetSequence () const { return m_sequence; }
    AnimationType Type () const { return m_type; }
    Time::Delta Duration () const { return m_duration; }
    Time SequenceStartTime () const { return m_sequence_start_time; }

    void SetSequence (Resource<Sequence> const &sequence) { ASSERT1(sequence.IsValid()); m_sequence = sequence; m_last_random_frame = 0; }
    void SetType (AnimationType type) { ASSERT1(type >= 0 && type < AT_COUNT); m_type = type; m_last_random_frame = 0; }
    void SetDuration (Time::Delta duration) { ASSERT1(duration > 0.0f); m_duration = duration; m_last_random_frame = 0; m_last_random_frame_time = Time::ms_beginning_of; }
    void SetSequenceStartTime (Time sequence_start_time) { m_sequence_start_time = sequence_start_time; m_last_random_frame = 0; m_last_random_frame_time = Time::ms_beginning_of; }

    GlTexture const &Frame (Time current_time) const;

private:

    static Float CalculateCycleParameter (Time cycle_time, Time::Delta cycle_duration);

    Resource<Sequence> m_sequence;
    AnimationType m_type;
    Time::Delta m_duration;
    // indicates the time the animation cycle starts (this is updated as the sequence cycles)
    Time m_sequence_start_time;
    // for use in the RANDOM type
    mutable Uint32 m_last_random_frame;
    mutable Time m_last_random_frame_time;
};

} // end of namespace Xrb

#endif // !defined(_XRB_ANIMATION_HPP_)

