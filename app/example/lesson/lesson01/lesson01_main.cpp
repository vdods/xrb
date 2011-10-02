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
// Lesson 01 - GUI Widgets And The Game Loop
// ///////////////////////////////////////////////////////////////////////////


/** @page lesson01 Lesson 01 - GUI Widgets And The Game Loop
@code *//* @endcode
This lesson will show you how to begin creating use graphical user interfaces
and how to design and run a game loop -- the heartbeat of a game engine.

    <ul>
    <li>@ref lesson01_main.cpp "This lesson's source code"</li>
    <li>@ref lessons "Main lesson index"</li>
    </ul>

<strong>Procedural Overview</strong> -- Items in bold are additions/changes to the previous lesson.

    <ul>
    <li>Main function</li>
        <ul>
        <li>Initialize the platform abstraction layer (Pal) and game engine singletons.
            Create the Screen object.  This was covered in previous lesson(s).</li>
        <li>Execute game-specific code.</li>
            <ul>
            <li><strong>Create formatted layouts of GUI widgets.</strong></li>
            <li><strong>Run the game loop</strong></li>
                <ul>
                <li><strong>Handle events (user and system-generated).</strong></li>
                <li><strong>Perform off-screen processing.</strong></li>
                <li><strong>Draw the Screen object's entire widget hierarchy.</strong></li>
                </ul>
            </ul>
        <li>Delete the Screen object.  Shutdown the Pal and game engine singletons.
            This was covered in previous lesson(s).</li>
        </ul>
    </ul>

Comments explaining previously covered material will be made more terse or
deleted entirely in each successive lesson.  If something is not explained
well enough, it was probably already explained in
@ref lessons "previous lessons".

<strong>Code Diving!</strong>

@code */
// This header MUST be included in every source/header file.
#include "xrb.hpp"

#include "xrb_button.hpp"        // For use of the Button widget class.
#include "xrb_event.hpp"         // For use of the Event classes.
#include "xrb_eventqueue.hpp"    // For use of the EventQueue class.
#include "xrb_inputstate.hpp"    // For use of the InputState class (via Singleton::).
#include "xrb_label.hpp"         // For use of the Label widget class.
#include "xrb_layout.hpp"        // For use of the Layout widget class.
#include "xrb_lineedit.hpp"      // For use of the LineEdit widget class.
#include "xrb_screen.hpp"        // For use of the necessary Screen widget class.
#include "xrb_sdlpal.hpp"        // For use of the SDLPal platform abstraction layer.
#include "xrb_widgetcontext.hpp" // For use of the WidgetContext class.

// Used so we don't need to qualify every library type/class/etc with Xrb::
using namespace Xrb;

// This is just a helper function to group all the shutdown code together.
void CleanUp ()
{
    std::cerr << "CleanUp();" << std::endl;

    // Shutdown the platform abstraction layer.
    Singleton::Pal().Shutdown();
    // Shutdown the game engine singletons.  This is necessary for the
    // game engine to shutdown cleanly.
    Singleton::Shutdown();
}

int main (int argc, char **argv)
{
    std::cerr << "main();" << std::endl;

    // Initialize the game engine singleton facilities.
    Singleton::Initialize(SDLPal::Create);
    // Initialize the platform abstraction layer (Pal).
    if (Singleton::Pal().Initialize() != Pal::SUCCESS)
        return 1;
    // Set the window caption.
    Singleton::Pal().SetWindowCaption("XuqRijBuh Lesson 01");

    // This call creates the Screen object and initializes the given video mode.
    // The Screen object is the root widget of the GUI widget hierarchy, and
    // does a bunch of special handling to draw its child widgets properly.
    Screen *screen = Screen::Create(
        800,    // video mode/screen width
        600,    // video mode/screen height
        32,     // video mode pixel bitdepth
        false); // not fullscreen -- none for now.
    // If the Screen failed to initialize, print an error message and quit.
    if (screen == NULL)
    {
        std::cerr << "unable to initialize video mode" << std::endl;
        // this shuts down the Pal and singletons.
        CleanUp();
        // return with an error value.
        return 2;
    }
    /* @endcode
    At this point, the singletons and the Pal have been initialized, the video
    mode has been set, and the engine is ready to go.  Here is where the
    application-specific code begins.

    Everything that is visible in this game engine happens inside an instance
    of a Widget subclass.  Therefore, we need to instantiate some widgets
    before anything interesting can happen.

    The Screen class defines the "root" widget, which you can think of
    as corresponding to the physical screen device (though in the future there
    will be Screens which render to textures, which can then be used to implement
    game-based computer terminals for example.  To each Screen belongs a
    WidgetContext, which stores "global" context data, and which each Widget
    requires during construction.  Each Widget therefore belongs to exactly
    one Screen, and (for now) can't be moved between Screens.

    A StyleSheet specifies the look of the various widgets we will use to create
    our GUI interface.  StyleSheet properties include fonts and font sizes, margin
    sizes, various textures and backgrounds.  Each WidgetContext has exactly one
    StyleSheet, which applies to all Widgets associated to that context.
    Specifying a StyleSheet is optional, but if none is specified, very austere
    default skin properties will be used.

    Create a StyleSheet, populate it with default values, and set the StyleSheet
    property of the WidgetContext associated to the Screen.  The WidgetContext
    takes ownership of the StyleSheet, so we don't need to worry about deleting it.
    @code */
    StyleSheet *style_sheet = new StyleSheet();
    style_sheet->PopulateUsingDefaults();
    screen->Context().SetStyleSheet(style_sheet);
    /* @endcode
    There are custom subclasses of Widget (such as Label, LineEdit, Button,
    Layout, etc) to perform various functions.  Widgets are organized in a
    hierarchy (i.e. parent/child relationship) which also dictates the spatial
    organization of widgets onscreen -- a child widget is completely contained
    within its parent, and cannot draw anything outside itself.  This is the
    same widget paradigm as used by the FLTK, MFC, QT (and many other)
    toolkits.  The GUI system in this game engine is primarily modeled
    after Trolltech's excellent QT GUI toolkit.

    Instead of having to worry about the exact screen coordinates at which to
    place our widgets, there is a Widget subclass called Layout which
    automatically handles widget sizing and placement.  Layouts can place
    widgets in horizontal/vertical lines or in grids, and can be nested
    within one another to form complicated formatting of widgets.
    @code */
    {
        /* @endcode
        We will now create a Layout widget as a child of our Screen (which,
        not coincidentally, is itself a Widget subclass).  This Layout will be
        the parent to the widgets we place inside.  The first parameter is the
        orientation of the Layout (a vertical column of widgets).  The second
        parameter is the aforementioned WidgetContext, which comes from the
        Screen instance.  The third parameter, which is present in almost all
        Widget subclasses as the last parameter, is the "name" of the widget.
        This parameter is optional, but its use is recommended.  It isn't used
        directly by the engine, but is valuable in debugging GUI problems --
        the ability to identify which widget you're dealing with in a mostly
        meaningless call stack within some event loop.

        After a widget is created, it must be attached to a parent widget
        (unless it is Screen, which is by definition the top-level Widget).
        The reason the child doesn't attach itself to its parent during
        construction (as was used in the past -- modeled on the Qt GUI toolkit
        design) is because there are certain key virtual method calls that
        need to be made during child attachment.  This can't be done during
        construction in C++.
        @code */
        Layout *main_layout = new Layout(VERTICAL, screen->Context(), "main layout");
        screen->AttachChild(main_layout);
        /* @endcode
        This call causes main_layout to fill out the entire space of screen.
        SetMainWidget is a Widget method, so can be used on any parent widget
        for a child.  If the parent widget's size or location change, its
        main widget will be resized and/or moved accordingly.
        @code */
        screen->SetMainWidget(main_layout);

        /* @endcode
        Now to create some directly useful widgets.

        A Label is a simple, non-interactive widget which draws text or a
        picture (we'll get to picture labels later).  The default
        justification for a Label's contents is centered both horizontally
        and vertically, but this, among other properties, can be changed.
        @code */
        main_layout->AttachChild(new Label("I LIKE ZOMBIES.", screen->Context(), "awesome zombie text label"));
        /* @endcode
        A Button can be clicked upon to signal some other piece of code to do
        something.  This button isn't connected to anything, but we'll get to
        that later.
        @code */
        main_layout->AttachChild(new Button("This button does nothing", screen->Context(), "do-nothing button"));

        /* @endcode
        Layouts, just like any Widget subclass, can be contained within other
        Layouts.  This layout will be used to place a text Label and a LineEdit
        side-by-side.  The code block is only being used to indicate creation
        of the layout and its child widgets.  Generally it is a good idea to
        wait until the Layout is filled with its child widgets before adding it
        to a parent widget.

        A LineEdit is a text-entry box.  The first parameter in its
        constructor indicates the maximum number of characters that can be
        entered into it.  It derives from the same baseclass as Label, so it
        shares many of the same properties as Label, such as alignment, text
        color, and so forth.
        @code */
        {
            Layout *sub_layout = new Layout(HORIZONTAL, screen->Context(), "label and line-edit layout");

            // Create a text Label to indicate what to do with the following LineEdit.
            sub_layout->AttachChild(new Label("You can enter up to 30 characters in this LineEdit ->", screen->Context(), "indicator label"));
            // Create the LineEdit after the Label, and it will be placed next
            // in the horizontal layout.
            sub_layout->AttachChild(new LineEdit(30, screen->Context(), "30-char line edit"));

            main_layout->AttachChild(sub_layout);
        }

        /* @endcode
        Add another horizontal Layout so we can have a row of text Labels to
        demonstrate the word wrapping and alignment properties.  Again, the
        code block is only being used to indicate creation of the layout and
        its child widgets.  Make sure to read the text contained in each Label,
        as each contains useful information.
        @code */
        {
            Layout *sub_layout = new Layout(HORIZONTAL, screen->Context(), "note label layout");

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
                    screen->Context(),
                    "left-aligned note label");
            // All text will be aligned with the left edge of the Label.  By default,
            // a Label's alignment is CENTER.
            note_label->SetAlignment(Dim::X, LEFT);
            sub_layout->AttachChild(note_label);

            // Add another label, this time, with centered alignment.
            note_label =
                new Label(
                    "This Label widget is using word wrapping with center alignment.  "
                    "The width of the Label dictates where the words will be wrapped.",
                    screen->Context(),
                    "center-aligned note label with word wrapping");
            // This call does exactly what it looks like it does.
            note_label->SetWordWrap(true);
            // The default alignment is already CENTER, so we don't need to do anything.
            sub_layout->AttachChild(note_label);

            // Add another label, this time, with right alignment.
            note_label =
                new Label(
                    "This Label widget is using word wrapping with right alignment. "
                    "Any Label (not just word wrapped Labels) can use the alignment "
                    "property, including aspect-ratio-preserving picture Labels.",
                    screen->Context(),
                    "right-aligned note label with word wrapping");
            // This call does exactly what it looks like it does.
            note_label->SetWordWrap(true);
            // All text will be aligned with the right edge of the Label.
            note_label->SetAlignment(Dim::X, RIGHT);
            sub_layout->AttachChild(note_label);
            
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
                    screen->Context(),
                    "spaced note label with word wrapping");
            // This call does exactly what it looks like it does.
            note_label->SetWordWrap(true);
            // see note_label's text for a description.
            note_label->SetAlignment(Dim::X, SPACED);
            sub_layout->AttachChild(note_label);

            main_layout->AttachChild(sub_layout);
        }

        /* @endcode
        Note the apparently dangling pointers that the above calls to "new"
        returned.  We don't have to ever worry about deleting widgets manually
        (except for the Screen), because when a Widget is destroyed, it deletes
        all its children (pretty heartless, huh?).  Thus, when the Screen is
        deleted, its entire widget hierarchy is massacred.

        We're done creating widgets for this example.  Next up is the game
        loop, where all the computation, screen-rendering, event-processing,
        user-input-processing, etc takes place.

        The screen keeps track of if a quit request was detected (i.e. Alt+F4
        or clicking the little X in the corner of the window pane).  We want
        to loop until the user requests to quit the app.
        @code */
        while (!screen->IsQuitRequested())
        {
            /* @endcode
            Sleep for 1/30 seconds so we don't suck up too much CPU time. This will
            result in limiting the game loop framerate to about 30 frames per second
            (because there is one screen-rendering per game loop iteration).
            @code */
            Singleton::Pal().Sleep(1.0f / 30.0f);
            /* @endcode
            Certain facilities of the game engine require the current time.
            This value should have type Time, and should measure the number of seconds
            since the application started.  Singleton::Pal().CurrentTime() returns
            this value.
            @code */
            Time time = Singleton::Pal().CurrentTime();
            /* @endcode
            This loop sucks up all pending events, produces Xrb::Event objects
            out of them, and craps them onto the top of the widget hierarchy (the
            Screen object).  Singleton::Pal().PollEvent will return non-NULL while
            there are still events to process.  We will loop until there are no
            more events left.
            @code */
            Event *event = NULL;
            while ((event = Singleton::Pal().PollEvent(screen, time)) != NULL)
            {
                /* @endcode
                If the event is of the keyboard or mouse, let the InputState
                singleton process it.  This step is necessary so that the
                state of said user-input devices is updated.
                @code */
                if (event->IsKeyEvent() || event->IsMouseButtonEvent())
                    Singleton::InputState().ProcessEvent(*event);
                /* @endcode
                All events are delegated to the proper widgets via the top of
                the widget hierarchy (the Screen object).  Events must go
                through the widget hierarchy because certain events are handled
                based on locations of widgets (e.g. mousewheel events always
                go to the widget(s) directly under the mouse cursor).
                @code */
                screen->ProcessEvent(*event);
                /* @endcode
                We don't want to rely on widgets deleting events themselves, as
                it would create a maintenance nightmare, so we insist that
                events must be deleted at whatever code scope they were
                created.  This also allows events that were created on the
                stack to be passed in without fear that they will be illegally
                deleted.
                @code */
                Delete(event);
            }

            /* @endcode
            Events can be enqueued for asynchronous/delayed processing (which
            will be discussed later), and so processing them on a time-basis
            must be done each game loop iteration.
            @code */
            screen->OwnerEventQueue()->ProcessFrame(time);
            /* @endcode
            This call is where all the off-screen (strictly non-rendering)
            computation for the widget hierarchy takes place.  Widget
            subclasses can override a particular method so that they can do
            per-frame computation.  The widget hierarchy is traversed in an
            undefined order (not strictly prefix, infix or postfix), calling
            each widget's "think" method.
            @code */
            screen->ProcessFrame(time);
            /* @endcode
            Turn the crank on the event queue again, since there may have been
            new events enqueued during the previous call, and we want to handle
            these delayed events at the earliest possible time.
            @code */
            screen->OwnerEventQueue()->ProcessFrame(time);
            /* @endcode
            Here is where the visual magic happens.  All the visible widgets
            in this hierarchy are drawn in this call.  If a widget is invisible
            -- if it is hidden, of zero area, or it is not on-screen -- its
            Draw method is not called.  The arguments are for real time and
            world time, but since we have no game world (yet), we'll just pass
            in real time for both.
            @code */
            screen->Draw(time);
        }
    }

    // Delete the Screen object, and with it the entire GUI widget hierarchy.
    Delete(screen);
    // this shuts down the Pal and the singletons.
    CleanUp();
    // return with success value.
    return 0;
}
/* @endcode

<strong>Exercises</strong>

    <ul>
    <li>Do NOT set a fullscreen video mode, because Alt+F4 and the window
        pane's X button will be unavailable.  Fullscreen-able apps must
        provide their own facilities for detecting the user's desire to quit
        (such as a quit button, or a certain keypress).</li>
    <li>Change the value of the @c Singleton::Pal().Sleep call in the game loop, and see
        what effect it has on the responsiveness of the Button and
        LineEdit.</li>
    <li>Add more text Label widgets to @c main_layout after the do-nothing
        button.  Note that their on-screen position in the vertical
        @c main_layout is directly reflected by the order they're created
        as children of <tt>main_layout</tt>.</li>
    <li>Enable word wrapping for the Label with the name
        <tt>"left-aligned note label"</tt>.  Notice how each newline in the
        Label's text starts a new paragraph.</li>
    <li>Change the event-polling from a while-loop to a single statement -- only
        polling one event per frame -- and see what effect it has on the
        responsiveness of the application.</li>
    </ul>

Thus concludes lesson01.  Do you feel smarter?  Well, you aren't.
*/
