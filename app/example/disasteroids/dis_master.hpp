// ///////////////////////////////////////////////////////////////////////////
// dis_master.hpp by Victor Dods, created 2006/03/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#if !defined(_DIS_MASTER_HPP_)
#define _DIS_MASTER_HPP_

#include "xrb_signalhandler.hpp"

#include "dis_highscores.hpp"
#include "xrb_eventqueue.hpp"
#include "xrb_frameratecalculator.hpp"
#include "xrb_keyrepeater.hpp"

using namespace Xrb;

/*

ui flow design:

title screen
    start: goto game screen
    controls: goto control screen
    quit: issue quit request

game screen
    ESC: pop up inventory screen

inventory screen
    end game button: go back to title screen
    quit game button: issue quit request

option screen
    UI to change the various UI controls and resolution and such

//////////////////////////////////////////////////////////////////////////////

title screen contents:

a letterbox game widget with disasteroids logo above and control buttons
and various credits below.  the game widget will have a demo/AI game playing
for visual effect.  the high scores will periodically appear above the game
widget, arcade-style.

*/

namespace Xrb {
    
class Screen;

}

namespace Dis {

class GameWidget;
class TitleScreenWidget;
class World;

class Master : public SignalHandler, public EventHandler
{
public:

    Master (Screen *screen);
    virtual ~Master ();

    void Run ();

protected:

    virtual bool HandleEvent (Event const &e);

private:

    void AcceptScore (Score const &score);
    void AcceptName (std::string const &name);

    void StartGame ();
    void QuitGame ();
    void EndGame ();

    void ActivateTitleScreen ();
    void DeactivateTitleScreen ();

    void ActivateGame ();
    void DeactivateGame ();

    void ProcessKeyRepeatEvents ();

    // the owner event queue for Master
    EventQueue m_master_event_queue;

    // the physical screen (and top-level widget)
    Screen *m_screen;
    // indicates if quit has been requested and the game loop should be exited
    bool m_is_quit_requested;
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
    Time m_real_time;

    // contains start-game, controls and quit buttons
    TitleScreenWidget *m_title_screen_widget;
    // indicates if the high scores should be shown immediately
    // upon the title screen being shown.
    bool m_show_high_scores_immediately;
    // indicates if the best points high scores should be shown before
    // the best wave count high scores.
    bool m_show_best_points_high_scores_first;

    // contains the WorldViewWidget which displays the game world
    GameWidget *m_game_widget;
    // the game world itself (i.e. the "document" in the "doc/view" paradigm)
    World *m_game_world;
    // stores the game world time
    Time m_game_time;
    // high scores
    HighScores m_high_scores;
    // saved score for while the high score name dialog is up
    Score m_saved_score;

    SignalReceiver1<Score const &> m_internal_receiver_accept_score;
    SignalReceiver1<std::string const &> m_internal_receiver_accept_name;

    SignalReceiver0 m_internal_receiver_start_game;
    SignalReceiver0 m_internal_receiver_quit_game;
    SignalReceiver0 m_internal_receiver_end_game;
}; // end of class Master

} // end of namespace Dis

#endif // !defined(_DIS_MASTER_HPP_)

