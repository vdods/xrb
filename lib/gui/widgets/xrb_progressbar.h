// ///////////////////////////////////////////////////////////////////////////
// xrb_progressbar.h by Victor Dods, created 2005/12/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_PROGRESSBAR_H_)
#define _XRB_PROGRESSBAR_H_

#include "xrb_widget.h"

#include "xrb_color.h"

using namespace Xrb;

namespace Xrb
{

class ProgressBar : public Widget
{
public:

    enum GrowOrientation
    {
        GO_LEFT = 0,
        GO_RIGHT,
        GO_UP,
        GO_DOWN,

        GO_COUNT
    };

    ProgressBar (
        GrowOrientation grow_orientation,
        ContainerWidget *parent,
        std::string const &name = "ProgressBar");
    virtual ~ProgressBar ();

    inline Float GetProgress () const
    {
        return m_progress;
    }
    inline Color const &GetColor () const
    {
        return m_color;
    }

    inline void SetProgress (Float const progress)
    {
        ASSERT1(progress >= 0.0f && progress <= 1.0f)
        m_progress = progress;
    }
    inline void SetColor (Color const &color)
    {
        m_color = color;
    }

    inline SignalReceiver1<Float> const *ReceiverSetProgress () { return &m_receiver_set_progress; }
    
    virtual void Draw (RenderContext const &render_context) const;
    
private:

    GrowOrientation m_grow_orientation;
    Float m_progress;
    Color m_color;

    SignalReceiver1<Float> m_receiver_set_progress;
}; // end of class ProgressBar

} // end of namespace Xrb

#endif // !defined(_XRB_PROGRESSBAR_H_)

