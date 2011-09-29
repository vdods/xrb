// ///////////////////////////////////////////////////////////////////////////
// xrb_pal.hpp by Victor Dods, created 2009/08/15
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_XRB_PAL_HPP_)
#define _XRB_PAL_HPP_

#include "xrb.hpp"

#include "xrb_screencoord.hpp"

namespace Xrb {

class Event;
class FileSystem;
class Font;
class Screen;
class Texture;

// Platform Abstraction Layer interface.  at most one instance of this
// interface will ever exist at one time (it is used as a singleton).
class Pal
{
public:

    enum Status { SUCCESS = 0, FAILURE = 1 };

    virtual ~Pal () { }

    // for pre-video-mode setup (e.g. starting the millisecond timer).
    virtual Status Initialize () = 0;
    // for post-video-mode shutdown.
    virtual void Shutdown () = 0;

    // should return an instance of FileSystem which has at least one directory
    // in its search path (i.e. FileSystem::IsEmpty() returns false).
    virtual FileSystem *CreateFileSystem () = 0;
    
    virtual Status InitializeVideo (Uint16 width, Uint16 height, Uint8 bit_depth, bool fullscreen) = 0;
    virtual void ShutdownVideo () = 0;

    virtual void SetWindowCaption (char const *window_caption) = 0;

    virtual void GrabInput () = 0;
    virtual void ReleaseInput () = 0;

    // returns the number of milliseconds since the Pal instance was
    // initialized.  due to the size of a 32-bit int, this value will
    // wrap about every 49 days
    virtual Uint32 CurrentTime () = 0;

    // block the the process for at least the specified number of
    // milliseconds, but possibly longer due to OS process scheduling.
    virtual void Sleep (Uint32 milliseconds_to_sleep) = 0;

    // called once, immediately after the game loop is done rendering
    // (could be used for video buffer swapping for example).
    virtual void FinishFrame () = 0;

    // return the next queued (input) event.  should return NULL if no
    // events are available, otherwise should new-up and return the
    // appropriate Xrb::Event (it will be deleted by XRB).
    virtual Event *PollEvent (Screen const *screen, Float time) = 0;

    // returns true iff the specified file exists (OS path).
    virtual bool FileExists (char const *file_id) = 0;
    // returns true iff the specified directory exists (OS path).
    virtual bool DirectoryExists (char const *directory_id) = 0;
    
    // should return NULL if the load failed.  the pixel data should be loaded
    // in right-handed coordinates (i.e. y=0 is at the bottom and goes up the screen)
    // image_path is an OS path.
    virtual Texture *LoadImage (char const *image_path) = 0;
    // return value should indicate status.  the pixel data should be loaded
    // in right-handed coordinates (i.e. y=0 is at the bottom and goes up the screen).
    // image_path is an OS path.
    virtual Status SaveImage (char const *image_path, Texture const &texture) = 0;

    // should return NULL if the load failed.  font_path is an OS path.
    virtual Font *LoadFont (char const *font_path, ScreenCoord pixel_height) = 0;

    // returns the size for each new GlTextureAtlas.  if either of the dimensions
    // is nonpositive, no texture atlasing will be used -- each texture will have its
    // own atlas (and using different textures will require GL texture bind calls).
    // if both coordinates are positive, each must be a power of 2.
    virtual ScreenCoordVector2 GlTextureAtlasSize () const = 0;
    // sets the size for each new GlTextureAtlas
    virtual void GlTextureAtlasSize (ScreenCoordVector2 const &size) = 0;
}; // end of class Pal

} // end of namespace Xrb

#endif // !defined(_XRB_PAL_HPP_)

