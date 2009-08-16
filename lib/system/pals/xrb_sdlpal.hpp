// ///////////////////////////////////////////////////////////////////////////
// xrb_sdlpal.hpp by Victor Dods, created 2009/08/16
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_SDLPAL_HPP_)
#define _XRB_SDLPAL_HPP_

#include "xrb.hpp"

#include "xrb_pal.hpp"

namespace Xrb
{

class SDLPal : public Pal
{
public:

    virtual Status Initialize ();
    virtual void Shutdown ();

    virtual Status InitializeVideo (Uint16 width, Uint16 height, Uint8 bit_depth, bool fullscreen);
    virtual void ShutdownVideo ();

    virtual Uint32 CurrentTimeInMilliseconds ();

    virtual void SleepForMilliseconds (Uint32 milliseconds_to_sleep);

    virtual void FinishFrame ();

    virtual Event *PollEvent ();

    virtual Texture *LoadImage (char const *image_path);
    virtual Status SaveImage (char const *image_path, Texture const &texture);

private:

    
}; // end of class SDLPal

} // end of namespace Xrb

#endif // !defined(_XRB_SDLPAL_HPP_)

