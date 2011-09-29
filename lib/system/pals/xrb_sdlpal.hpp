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

#if XRB_PLATFORM == XRB_PLATFORM_SDL

// it's critical that SDL.h is included here and that this file is included
// in the cpp file which contains main(), because SDL.h actually redefines
// main to SDL_main and does some other hidden stuff.
#include "SDL.h"
#include "xrb_pal.hpp"

struct FT_LibraryRec_;

class SDLPal : public Xrb::Pal
{
public:

    // returns an instance of SDLPal.  to be used in Singleton::Initialize()
    static Xrb::Pal *Create ();

    virtual ~SDLPal ();

    virtual Status Initialize ();
    virtual void Shutdown ();

    virtual Xrb::FileSystem *CreateFileSystem ();
    
    virtual Status InitializeVideo (Xrb::Uint16 width, Xrb::Uint16 height, Xrb::Uint8 bit_depth, bool fullscreen);
    virtual void ShutdownVideo ();

    virtual void SetWindowCaption (char const *window_caption);

    virtual void GrabInput ();
    virtual void ReleaseInput ();

    virtual Xrb::Uint32 CurrentTime ();

    virtual void Sleep (Xrb::Uint32 milliseconds_to_sleep);

    virtual void FinishFrame ();

    virtual Xrb::Event *PollEvent (Xrb::Screen const *screen, Xrb::Float time);

    virtual bool FileExists (char const *file_id);
    virtual bool DirectoryExists (char const *directory_id);
    
    virtual Xrb::Texture *LoadImage (char const *image_path);
    virtual Status SaveImage (char const *image_path, Xrb::Texture const &texture);

    virtual Xrb::Font *LoadFont (char const *font_path, Xrb::ScreenCoord pixel_height);

    virtual Xrb::ScreenCoordVector2 GlTextureAtlasSize () const;
    virtual void GlTextureAtlasSize (Xrb::ScreenCoordVector2 const &size);

private:

    // the constructor is private so you must use SDLPal::Create.
    // it is acceptable for ft_library to be NULL, as it indicates
    // the freetype library will not be used, and font loading must
    // rely on cached data.
    SDLPal (FT_LibraryRec_ *ft_library, Xrb::ScreenCoordVector2 const &gltexture_atlas_size)
        :
        m_ft_library(ft_library),
        m_gltexture_atlas_size(gltexture_atlas_size)
    { }

    FT_LibraryRec_ *m_ft_library;
    Xrb::ScreenCoordVector2 m_gltexture_atlas_size;
}; // end of class SDLPal

#endif // XRB_PLATFORM == XRB_PLATFORM_SDL

#endif // !defined(_XRB_SDLPAL_HPP_)

