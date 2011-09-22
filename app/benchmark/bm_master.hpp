// ///////////////////////////////////////////////////////////////////////////
// bm_master.hpp by Victor Dods, created 2009/08/24
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_BM_MASTER_HPP_)
#define _BM_MASTER_HPP_

#include "xrb_signalhandler.hpp"

// #include "xrb_eventqueue.hpp"
#include "xrb_frameratecalculator.hpp"
#include "xrb_keyrepeater.hpp"

using namespace Xrb;

namespace Xrb
{
    class Screen;
    namespace Engine2
    {
        class World;
    }
}

namespace Bm
{

class Master// : public SignalHandler, public EventHandler
{
public:

    Master (Screen *screen);
    virtual ~Master ();

    void Run ();

protected:

    virtual bool HandleEvent (Event const &e);

private:

    void ProcessKeyRepeatEvents ();

    void CreateWorld ();
    void DestroyWorld ();

//     // the owner event queue for Master
//     EventQueue m_master_event_queue;

    // the physical screen (and top-level widget)
    Screen *m_screen;
    // the KeyRepeater which generates EventKeyRepeat events
    KeyRepeater m_key_repeater;
    // calculates rendering framerate
    FramerateCalculator m_framerate_calculator;
    // minimum allowed framerate (if the actual framerate is lower
    // than this, the game will be artificially slowed)
    Float m_minimum_framerate;
    // maximum allowed framerate (if the actual framerate is higher
    // than this, Singleton::Pal().Sleep will be used to sleep the process).
    Float m_maximum_framerate;
    // current real time
    Float m_real_time;

//     // contains the WorldViewWidget which displays the game world
//     GameWidget *m_game_widget;
    // the world itself (i.e. the "document" in the "doc/view" paradigm)
    Engine2::World *m_world;
    // stores the game world time
    Float m_game_time;
}; // end of class Master

} // end of namespace Bm

#endif // !defined(_BM_MASTER_HPP_)

