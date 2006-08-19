// ///////////////////////////////////////////////////////////////////////////
// dis_optionspanel.h by Victor Dods, created 2006/02/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_OPTIONSPANEL_H_)
#define _DIS_OPTIONSPANEL_H_

#include "xrb_containerwidget.h"

#include "xrb_validator.h"

using namespace Xrb;

namespace Dis
{

/*
video:
- screen resolution X/Y
- fullscreen
audio:
- none yet
input:
- forward
- left
- back
- right
- primary weapon fire
- secondary weapon fire
- tractor mode
- engine brake
*/

class OptionsPanel : public ContainerWidget
{
public:

    OptionsPanel (ContainerWidget *parent);
    virtual ~OptionsPanel ();

    inline ScreenCoordVector2 const &GetResolution () const { return m_resolution; }
    inline bool GetFullscreen () const { return m_fullscreen; }

    void SetResolutionX (ScreenCoord resolution_x);
    void SetResolutionY (ScreenCoord resolution_y);
    void SetFullscreen (bool fullscreen);

private:

    ScreenCoordVector2 m_resolution;
    bool m_fullscreen;

    GreaterThanValidator<ScreenCoord> m_greater_than_zero_validator;

    SignalSender1<ScreenCoord> m_internal_sender_resolution_x_changed;
    SignalSender1<ScreenCoord> m_internal_sender_resolution_y_changed;
    SignalSender1<bool> m_internal_sender_fullscreen_changed;

    SignalReceiver1<ScreenCoord> m_internal_receiver_set_resolution_x;
    SignalReceiver1<ScreenCoord> m_internal_receiver_set_resolution_y;
    SignalReceiver1<bool> m_internal_receiver_set_fullscreen;
}; // end of class OptionsPanel

} // end of namespace Dis

#endif // !defined(_DIS_OPTIONSPANEL_H_)

