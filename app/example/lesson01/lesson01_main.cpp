// ///////////////////////////////////////////////////////////////////////////
// lesson01_main.cpp by Victor Dods, created 2006/07/25
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////


// ///////////////////////////////////////////////////////////////////////////
// Lesson 01 - GUI widgets and the game loop
// ///////////////////////////////////////////////////////////////////////////


// this header MUST be included in every source/header file, as it contains
// definitions necessary for the correct usage and operation of libxrb.
#include "xrb.h"

#include "xrb_button.h"     // for use of the Button widget class
#include "xrb_event.h"      // for use of the Event classes
#include "xrb_eventqueue.h" // for use of the EventQueue class
#include "xrb_input.h"      // for use of the Input class (via Singletons::)
#include "xrb_label.h"      // for use of the Label widget class
#include "xrb_layout.h"     // for use of the Layout widget class
#include "xrb_lineedit.h"   // for use of the LineEdit widget class
#include "xrb_screen.h"     // for use of the necessary Screen widget class

// this using statement is useful so that we don't need to qualify every
// library type/class/etc with Xrb::
using namespace Xrb;

// cleans stuff up.  see below
void CleanUp ()
{
    fprintf(stderr, "CleanUp();\n");

    // shutdown the singletons
    Singletons::Shutdown();
    // make sure the application doesn't still have the mouse grabbed,
    // or you'll have a hard time pointy-clickying at stuff.
    SDL_WM_GrabInput(SDL_GRAB_OFF);
    // call SDL's cleanup function.
    SDL_Quit();
}

int main (int argc, char **argv)
{
    fprintf(stderr, "main();\n");

    // first attempt to initialize SDL.  currently this just involves
    // initializing the video, but will later initialize sound, once
    // sound code is actually written.  the SDL_INIT_NOPARACHUTE flag
    // is used because we want crashes that will be useful in debug mode
    // (meaning, in unix, it will core dump).
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE) < 0)
    {
        fprintf(stderr, "unable to initialize video.  error: %s\n", SDL_GetError());
        // return with an error value.
        return 1;
    }

    // this call initializes libxrb's singleton facilities.  this must
    // be done, or the engine will just not work.  the singletons include:
    // - ResourceLibrary: ensures that we only load one copy of certain
    //                    resources (textures, fonts, sounds, etc) into memory.
    // - Input: accessor for the immediate state of the keyboard and mouse
    //          (and eventually joysticks, etc).  this is not the primary/only
    //          means for user input, but we'll get to that later.
    // - KeyMap: performs keyboard layout mapping (e.g. Dvorak), which is
    //           necessary only on windows builds because the windows version
    //           of SDL lacks proper key mapping.
    // - FTLibrary: this is used by the font system to use the FreeType font
    //              rendering facilities.  you shouldn't need to worry about it.
    Singletons::Initialize(NULL);

    // set the caption for the application's window.  i haven't figured out
    // what the icon string is, maybe it's supposed to be the filename for
    // a BMP file or something.
    SDL_WM_SetCaption("XuqRijBuh Lesson 01", "icon thingy");

    // this call creates the Screen object and initializes the given video
    // mode (800x600, 32 bit color).  there is no constraint on the size
    // or aspect ratio of the screen, apart from the ability of your video
    // hardware to handle it.  the Screen object is the root widget of the
    // GUI widget hierarchy, and does a bunch of special handling to draw
    // its child widgets properly.
    //
    // In this example, do not set fullscreen mode, because we will need
    // to hit Alt+F4 or click the X button in the window frame to quit
    // the app.
    Screen *screen = Screen::Create(
        800,                    // video mode/screen width
        600,                    // video mode/screen height
        32,                     // video mode pixel bitdepth
        0);                     // SDL_SetVideomode flags (none currently)
    // if the Screen failed to initialize for whatever reason (probably because
    // the system was unable to set the video mode), screen will be NULL.  if
    // this happens, print an error message and quit with an error code.
    if (screen == NULL)
    {
        fprintf(stderr, "unable to initialize video mode\n");
        // this shuts down libxrb's singletons, and shuts down SDL.
        CleanUp();
        // return with an error value.
        return 2;
    }

    // at this point, SDL has been initialized, the engine is ready to go and
    // the video mode has been set.  here is where the application-specific
    // code begins.
    {
        // everything that is visible in this game engine is a Widget.
        // therefore, we need to instantiate some widgets before anything
        // interesting can happen.

        // there are custom subclasses of Widget (such as Label, LineEdit,
        // Button, Layout, etc) to perform various functions.  Widgets are
        // organized in a hierarchy (i.e. parent/child relationship) which
        // also dictates the spatial organisation of widgets onscreen -- a
        // child widget is completely contained within its parent, and cannot
        // draw anything outside itself.

        // instead of having to worry about the exact screen coordinates to
        // place our widgets at, there is a Widget subclass called Layout which
        // automatically handles widget sizing and placement for us.  Layouts
        // can place widgets in horizontal/vertical lines, or in grids.

        // we will now create a Layout widget as a child of our Screen (which,
        // not coincidentally, is itself a Widget subclass).  this Layout will
        // be the parent to the widgets we place inside.  The first parameter
        // is the orientation of the Layout (a vertical column of widgets).
        // The second parameter is the Widget to assign as its parent -- in
        // this case, the Screen itself.  The third parameter, which is present
        // in almost all Widget subclasses after the parent widget parameter,
        // is the "name" of the widget.  This isn't used directly by the engine,
        // but is useful in debugging GUI problems (being able to identify
        // which widget you're dealing with in a big confusing call stack).
        Layout *main_layout = new Layout(VERTICAL, screen, "main layout");
        // This call causes main_layout to fill out the entire space of
        // screen (SetMainWidget is a Widget method, so can be used on any
        // parent widget for a child).  If the parent widget's size or location
        // change, its main widget will be resized or moved accordingly.
        screen->SetMainWidget(main_layout);

        // now to create some directly useful widgets.

        // a Label is a simple, unmoving widget which draws text or a picture
        // (we'll get to picture labels later).  the default justification for
        // a Label is centered both horizontally and vertically, but this, among
        // other properties, can be changed.
        new Label("I LIKE ZOMBIES.", main_layout, "awesome zombie text label");
        // a Button can be clicked upon to signal some other piece of code to
        // do something.  This button isn't connected to anything, but we'll get
        // to that later.
        new Button("This button does nothing", main_layout, "do-nothing button");

        // Layouts, just like any Widget subclass, can be contained within
        // other Layouts.  This layout will be used to place a text Label
        // and a LineEdit side-by-side.  The code block is only being used
        // to indicate creation of the layout and its child widgets.
        {
            Layout *sub_layout = new Layout(HORIZONTAL, main_layout, "label and line-edit layout");

            // Create a text Label to indicate what to do with the following LineEdit.
            new Label("You can enter up to 30 characters in this LineEdit ->", sub_layout, "indicator label");
            // a LineEdit is a text-entry box.  the first parameter indicates the
            // maximum number of characters that can be entered.
            new LineEdit(30, sub_layout, "30-char line edit");
        }

        // add another horizontal Layout so we can have a row of text Labels
        // to demonstrate the word wrapping and alignment properties.  Again,
        // the code block is only being used to indicate creation of the layout
        // and its child widgets.
        {
            Layout *sub_layout = new Layout(HORIZONTAL, main_layout, "note label layout");

            // Another text Label.  It notes that holding down a key will not cause
            // the character to repeat.  Doing this will be discussed later.  This
            // Label demonstrates word wrapping and justification.
            Label *note_label =
                new Label(
                    "Note that holding a key down while typing into the\n"
                    "LineEdit does not cause more than one character to\n"
                    "be entered (i.e. no keyboard repeating).  Doing so\n"
                    "complicates the event-polling loop, and it was\n"
                    "omitted to keep this example as simple as possible.\n"
                    "Notice how this text Label is wider than the other\n"
                    "three adjacent Labels.  This is because by default,\n"
                    "text Labels' minimum width is fixed to the text\n"
                    "width, so none of the text is cut off.  The other\n"
                    "three Labels can be resized horizontally because\n"
                    "they each have word wrapping enabled, so the text\n"
                    "formatting is dictated by the width of the Label.",
                    sub_layout,
                    "left-aligned note label");
            // All text will be aligned with the left edge of the Label.  By default,
            // a Label's alignment is CENTER.
            note_label->SetAlignment(Dim::X, LEFT);

            // Add another label, this time, with centered alignment.
            note_label =
                new Label(
                    "This Label widget is using word wrapping with center alignment.  "
                    "The width of the Label dictates where the words will be wrapped.",
                    sub_layout,
                    "center-aligned note label with word wrapping");
            // This call does exactly what it looks like it does.
            note_label->SetWordWrap(true);
            // The default alignment is already CENTER, so we don't need to do anything.

            // Add another label, this time, with right alignment.
            note_label =
                new Label(
                    "This Label widget is using word wrapping with right alignment. "
                    "Any Label (not just word wrapped Labels) can use the alignment "
                    "property, including aspect-ratio-preserving picture Labels.",
                    sub_layout,
                    "right-aligned note label with word wrapping");
            // This call does exactly what it looks like it does.
            note_label->SetWordWrap(true);
            // All text will be aligned with the right edge of the Label.
            note_label->SetAlignment(Dim::X, RIGHT);

            // Add another label, this time, with right alignment.
            note_label =
                new Label(
                    "This Label widget uses word wrapping with character spacing. "
                    "Spacing is a special type of alignment that only applies to word "
                    "wrapped Labels, and will attempt to space the characters out to "
                    "fill out the entire width of the Label.  The extra spacing between "
                    "characters seems to sometimes screw up the font's kerning (the "
                    "space between specific glyph pairs to make the text look more "
                    "natural).",
                    sub_layout,
                    "spaced note label with word wrapping");
            // This call does exactly what it looks like it does.
            note_label->SetWordWrap(true);
            // see note_label's text for a description.
            note_label->SetAlignment(Dim::X, SPACED);
        }

        // Note the apparently dangling pointers that the above calls to "new"
        // returned.  We don't have to ever worry about deleting widgets
        // manually (except for the Screen), because when a Widget is destroyed,
        // it deletes all its children.

        // We're done creating widgets for this example.  Next up is the game
        // loop, where all the computation, screen-rendering, event-processing,
        // user-input-processing, etc takes place.

        // The screen keeps track of if SDL detected a quit request (i.e. Alt+F4
        // or clicking the little X in the corner of the window pane).  We want
        // to loop until the user requests to quit the app.
        while (!screen->GetIsQuitRequested())
        {
            // Sleep for 33 milliseconds so we don't suck up too much CPU time.
            // This will result in the game loop running at about 30 frames per
            // second (because there is one screen-rendering per game loop
            // iteration).
            SDL_Delay(33);

            // Certain facilities of the game engine require the current time.
            // This value should be represented as a Float (notice the
            // capitalization), and should measure the number of seconds since
            // the application started.  SDL_GetTicks() returns the number of
            // milliseconds since the app started, so take one thousandth of that.
            Float time = 0.001f * SDL_GetTicks();

            // SDL is an abstraction layer on top of the operating system to
            // homogenize various facilities a game may need to use (specifically,
            // here, we're interested in keyboard and mouse events).  This loop
            // sucks up all pending events, creates Xrb::Event objects out of
            // them, and hands them to the top of the widget hierarchy (the
            // Screen).  SDL_Event is the structure provided by SDL to house
            // event data.  SDL_PollEvent will return true while there are still
            // events in the queue to process.  It will place the event data
            // in the SDL_Event pointer passed to it.

            SDL_Event sdl_event;
            while (SDL_PollEvent(&sdl_event))
            {
                // Here is where we repackage the SDL_Event into a (subclass of)
                // Xrb::Event, for use by the Screen.  Events require the current
                // time (this will be discussed later for EventQueue).
                Event *event = Event::CreateEventFromSDLEvent(&sdl_event, screen, time);

                // if it was a dud, skip this loop
                if (event == NULL)
                    continue;

                // this step is necessary so that the Input singleton (the
                // state of the user-input devices such as the keyboard and
                // mouse) is updated.
                if (event->GetIsKeyEvent() || event->GetIsMouseButtonEvent())
                    Singletons::Input().ProcessEvent(event);

                // All events are delegated to the proper widgets via the top of
                // the widget hierarchy (the Screen).  Events must go through
                // the widget hierarchy because certain events are handled based
                // on locations of widgets (e.g. mousewheel events always go to
                // the widget(s) directly under the mouse cursor).
                screen->ProcessEvent(event);
                // We don't want to rely on widgets deleting events themselves,
                // as it would create a maintainence nightmare, so we insist
                // that events must be deleted at whatever code scope they were
                // created.
                Delete(event);
            }

            // Events can be enqueued for later processing (which will be
            // discussed later), and so processing them on a time-basis
            // must be done each game loop iteration.
            screen->GetOwnerEventQueue()->ProcessFrame(time);
            // This call is where all the invisible (non-rendering) computation
            // for the widget hierarchy takes place.  Widget subclasses can
            // override a particular method so that they can do per-frame
            // computation.  The widget hierarchy is traversed in an undefined
            // order, calling each widget's "think" method.
            screen->ProcessFrame(time);
            // Turn the crank on the event queue again, since there may have
            // been new events enqueued during the previous call, and we want
            // to handle these delayed events at the earliest possible time.
            screen->GetOwnerEventQueue()->ProcessFrame(time);

            // Here is where the visual magic happens.  All the (visible)
            // widgets are drawn in this call (if a widget is invisible --
            // if it is not on-screen, hidden, of zero area, etc -- its
            // Draw method is not called.
            screen->Draw();
        }
    }

    // delete the Screen object, and with it the entire GUI widget hierarchy.
    // this call doesn't reset the video mode however, that is done by
    // calling SDL_Quit(), which we have stashed away in CleanUp().
    Delete(screen);

    // this shuts down libxrb's singletons, and shuts down SDL.
    CleanUp();
    // return with success value.
    return 0;
}

