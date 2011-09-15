// ///////////////////////////////////////////////////////////////////////////
// lesson02_main.cpp by Victor Dods, created 2006/08/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////


// ///////////////////////////////////////////////////////////////////////////
// Lesson 02 - SignalHandler, SignalSender and SignalReceiver
// ///////////////////////////////////////////////////////////////////////////


/** @page lesson02 Lesson 02 - SignalHandler, SignalSender and SignalReceiver
@code *//* @endcode
This lesson will show you how to use the SignalHandler system -- a fancy
abstraction for object communication.  The GUI system is the primary user of
this system to signal when certain events happen, such as a Button being
pressed, or the text of a LineEdit being changed.

    <ul>
    <li>@ref lesson02_main.cpp "This lesson's source code"</li>
    <li>@ref lessons "Main lesson index"</li>
    </ul>

The SignalHandler system can be thought of a way to plug objects into each
other to handle communication.  For instance, when a Button is pressed, it
emits a signal indicating that it was pressed.  This signal can be connected
to other objects so that when said Button is pressed, particular methods in
the other objects are called.  The advantages are that the signal sender
doesn't have to check or even care if there is or isn't anything connected to
it, and that when an object is destructed, it disconnects all its connections
automatically, making SignalHandler code very simple.

The system is facilitated by three classes:

    <ul>
    <li>SignalHandler must be inherited by any class which contains one or
        more instances of SignalSender or SignalReceiver.  It is what
        disconnects all its own remaining connections upon destruction.</li>
    <li>SignalSender (actually SignalSender0, SignalSender1 and SignalSender2,
        whose numbers indicate the number of parameters they broadcast)
        is what broadcasts signals out into space, to be received only by
        those objects connected to it.</li>
    <li>SignalReceiver (actually SignalReceiver0, SignalReceiver1 and
        SignalReceiver2, whose numbers indicate the number of parameters
        their callbacks accept) is what implements a handler method to be
        called when a signal is received.</li>
    </ul>

A signal can be composed of any number or type of arguments (including none).
This is accomplished by the templatization of SignalSender1, SignalSender2,
SignalReceiver1 and SignalReceiver2.  Emitting a signal is syntactically
identical to making a simple function call.  If you still don't know what the
fuck this is all about, looking at the example code should make things clear.

<strong>Procedural Overview</strong> -- Items in bold are additions/changes to the previous lesson.

    <ul>
    <li>Main function</li>
        <ul>
        <li>Initialize the platform abstraction layer (Pal) and game engine singletons.
            Create the Screen object. This was covered in previous lesson(s).</li>
        <li>Execute game-specific code.</li>
            <ul>
            <li>Create formatted layouts of GUI widgets.</li>
            <li><strong>Connect SignalSenders up to SignalReceivers.</strong></li>
            <li>Run the game loop</li>
                <ul>
                <li>Handle events (user and system-generated).</li>
                <li>Perform off-screen processing.</li>
                <li>Draw the Screen object's entire widget hierarchy.</li>
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

#include "xrb_button.hpp"         // For use of the Button widget class.
#include "xrb_event.hpp"          // For use of the Event classes.
#include "xrb_eventqueue.hpp"     // For use of the EventQueue class.
#include "xrb_inputstate.hpp"     // For use of the InputState class (via Singleton::).
#include "xrb_label.hpp"          // For use of the Label widget class.
#include "xrb_layout.hpp"         // For use of the Layout widget class.
#include "xrb_lineedit.hpp"       // For use of the LineEdit widget class.
#include "xrb_screen.hpp"         // For use of the necessary Screen widget class.
#include "xrb_sdlpal.hpp"         // For use of the SDLPal platform abstraction layer.
#include "xrb_transformation.hpp" // For use of Transformation::Lowercase and Uppercase.

// Used so we don't need to qualify every library type/class/etc with Xrb::
using namespace Xrb;

// This is just a helper function to group all the shutdown code together.
void CleanUp ()
{
    fprintf(stderr, "CleanUp();\n");

    // Shutdown the platform abstraction layer.
    Singleton::Pal().Shutdown();
    // Shutdown the game engine singletons.  Do this OR PERISH.
    Singleton::Shutdown();
}

int main (int argc, char **argv)
{
    fprintf(stderr, "main();\n");

    // Initialize the game engine singleton facilities.
    Singleton::Initialize(SDLPal::Create, "none");
    // Initialize the Pal.
    if (Singleton::Pal().Initialize() != Pal::SUCCESS)
        return 1;
    // Set the window caption.
    Singleton::Pal().SetWindowCaption("XuqRijBuh Lesson 02");
    // Create Screen object and initialize given video mode.
    Screen *screen = Screen::Create(800, 600, 32, false);
    // If the Screen failed to initialize, print an error message and quit.
    if (screen == NULL)
    {
        CleanUp();
        return 2;
    }

    /* @endcode
    At this point, the singletons and the Pal have been initialized, the video
    mode has been set, and the engine is ready to go.  Here is where the
    application-specific code begins.  We will create widgets different from
    the ones created in @ref lesson01 "lesson01".
    @code */
    {
        // Create a Layout widget to contain everything within the Screen.
        Layout *main_layout = new Layout(VERTICAL, "main layout");
        // Attach the main layout to the screen as a child widget.
        screen->AttachChild(main_layout);
        // Cause @c main_layout to always fill the Screen.
        screen->SetMainWidget(main_layout);

        /* @endcode
        Here we'll create the grid-type Layout widget mentioned in
        @ref lesson01 "lesson01".  A grid Layout can be row-major
        (successively added widgets constitute left-to-right rows) or
        column-major (successively added widgets constitute top-to-bottom
        columns). The size of the major dimension must be specified, so
        the Layout knows when to wrap the rows/columns.  Attaching
        @c button_signal_demo_layout to @c main_layout is deferred until
        after adding everything to @c button_signal_demi_layout in order
        to generally lessen the layout calculation.
        @code */
        Layout *button_signal_demo_layout = new Layout(ROW, 2, "button signal demo layout");
        
        /* @endcode
        Create four Buttons which will change different properties of the
        Label which will be created below.  Two of the Buttons, when pressed,
        will disable/enable the Label, while the other two Buttons, when
        pressed <i>and</i> released, will disable/enable the Label.
        @code */
        Button *disable_label_via_press_button =
            new Button(
                "Press to disable Label below",
                "disable label via press button");
        button_signal_demo_layout->AttachChild(disable_label_via_press_button);

        Button *enable_label_via_press_button =
            new Button(
                "Press to enable Label below",
                "enable label via press button");
        button_signal_demo_layout->AttachChild(enable_label_via_press_button);

        Button *disable_label_via_release_button =
            new Button(
                "Press and release to disable Label below",
                "disable label via release button");
        button_signal_demo_layout->AttachChild(disable_label_via_release_button);

        Button *enable_label_via_release_button =
            new Button(
                "Press and release to enable Label below",
                "enable label via release button");
        button_signal_demo_layout->AttachChild(enable_label_via_release_button);

        main_layout->AttachChild(button_signal_demo_layout);

        Label *mabel_the_disabled_label =
            new Label(
                "MY NAME IS MABEL THE LABEL\nIf the text is white, it is enabled.\nIf the text is darkened, it is disabled.",
                "mabel the disabled label");
        main_layout->AttachChild(mabel_the_disabled_label);
        /* @endcode
        Create a "QUIT" button in the vertical @c main_layout which we will
        later hook up to the Screen's Screen::RequestQuit SignalReceiver.
        We'll also double the font size to make it look "HELLA TUFF."
        @code */
        Button *quit_button = new Button("Press and release this button to QUIT", "quit button");
        //quit_button->SetFontHeight(2 * quit_button->GetFont()->PixelHeight()); // HIPPO
        main_layout->AttachChild(quit_button);
        /* @endcode
        Create another grid Layout for another set of demo widgets.
        @code */
        Layout *text_signal_demo_layout = new Layout(ROW, 2, "text signal demo layout");
        /* @endcode
        Create four pairs of widgets -- a Label/LineEdit, and three Label/Label
        pairs.  The LineEdit will be used to enter text, and will emit signals
        when its text is changed.  The second Labels in each pair will set their
        own values to the received text.  The signal sent to the second and third
        labels -- <tt>lowercase_label</tt> and <tt>uppercase_label</tt> -- will
        be transformed en route to lowercase and uppercase text respectively.
        The signal connections and transformations will be handled after we
        create all our widgets.
        @code */
        Label *generic_label = new Label("Enter text here:");
        generic_label->SetAlignment(Dim::X, RIGHT);
        text_signal_demo_layout->AttachChild(generic_label);

        LineEdit *enter_text_line_edit = new LineEdit(40, "enter text lineedit");
        text_signal_demo_layout->AttachChild(enter_text_line_edit);

        generic_label = new Label("Verbatim text:");
        generic_label->SetAlignment(Dim::X, RIGHT);
        text_signal_demo_layout->AttachChild(generic_label);

        Label *verbatim_label = new Label("", "verbatim label");
        verbatim_label->SetAlignment(Dim::X, LEFT);
        text_signal_demo_layout->AttachChild(verbatim_label);

        generic_label = new Label("Text in lowercase:");
        generic_label->SetAlignment(Dim::X, RIGHT);
        text_signal_demo_layout->AttachChild(generic_label);

        Label *lowercase_label = new Label("", "lowercase label");
        lowercase_label->SetAlignment(Dim::X, LEFT);
        text_signal_demo_layout->AttachChild(lowercase_label);

        generic_label = new Label("Text in UPPERCASE:");
        generic_label->SetAlignment(Dim::X, RIGHT);
        text_signal_demo_layout->AttachChild(generic_label);

        Label *uppercase_label = new Label("", "UPPERCASE label");
        uppercase_label->SetAlignment(Dim::X, LEFT);
        text_signal_demo_layout->AttachChild(uppercase_label);
        
        // Don't forget to add the subordinate layout to main_layout.
        main_layout->AttachChild(text_signal_demo_layout);

        // Remember that we don't need to worry about the apparently dangling
        // pointers left by all the above calls to new, because when a Widget
        // is destroyed, it deletes all its children.  Thus, when the Screen
        // is deleted, its entire widget hierarchy is pulverized.

        /* @endcode
        Here is where we'll hook up all the signals that were being talked up
        so much earlier.  First, we'll hook up the simpler Button signals.

        Signal connections are made through a series of static SignalHandler
        methods prefixed with <tt>Connect</tt>, suffixed with the number of
        parameters used by the signal being connected.  The first parameter
        to this method is a pointer to the SignalSender -- these are available
        through public accessor methods in the object being connected --
        Button::SenderPressed for example.  The second parameter is the
        SignalReceiver -- also available through accessor methods --
        Widget::ReceiverDisable for
        example.

        Once a connection is made, sender signals from the connected object
        will cause the receiver object's connected receiver handler to be
        called.  This is a direct function call, as if the sender object
        was calling the receiver object's handler itself -- there is no
        asynchronous behavior or event passing going on.

        Note that there is more than one SignalSender being connected to
        <tt>mabel_the_disabled_label</tt>'s ReceiverDisable and ReceiverEnable.
        You can connect as many SignalSenders to a single SignalReceiver as
        you'd like.  We'll see below that the converse is also true.
        @code */
        SignalHandler::Connect0(
            disable_label_via_press_button->SenderPressed(),
            mabel_the_disabled_label->ReceiverDisable());
        SignalHandler::Connect0(
            enable_label_via_press_button->SenderPressed(),
            mabel_the_disabled_label->ReceiverEnable());
        SignalHandler::Connect0(
            disable_label_via_release_button->SenderReleased(),
            mabel_the_disabled_label->ReceiverDisable());
        SignalHandler::Connect0(
            enable_label_via_release_button->SenderReleased(),
            mabel_the_disabled_label->ReceiverEnable());

        /* @endcode
        Here is where we hook up the quit Button.  We use Button::SenderReleased
        instead of Button::SenderPressed, because the defacto behavior for GUI
        buttons is to activate when they're released after being pressed.
        @code */
        SignalHandler::Connect0(
            quit_button->SenderReleased(),
            screen->ReceiverRequestQuit());

        /* @endcode
        We've only dealt with zero-parameter signals so far, but what use is a
        fancy signal-connection system without the ability to pass values
        around?  The SignalSender and Receiver classes are templatized, so
        there is no restriction on the type of parameters that can be passed.

        We will connect the text signal demo signals now.  Each signal involved
        here will pass a single parameter -- either a <tt>std::string const &</tt>
        or a <tt>std::string</tt>.  The difference is subtle but important: we
        want to avoid passing strings around by value if possible (to avoid
        unnecessary calls to <tt>std::string</tt>'s copy constructor), but in
        certain situations which will be explained shortly, we <i>must</i> pass
        by value.

        LineEdit provides us with several options in terms of SignalSenders:

            <ul>
            <li>LineEdit::SenderTextUpdated passes the LineEdit's text content
                by const reference and is signaled when the text has been
                changed via LineEdit::SetText, when the user changes the text
                and hits enter, or when the user changes the text and removes
                GUI focus from the LineEdit.</li>
            <li>LineEdit::SenderTextUpdatedV is identical to
                LineEdit::SenderTextUpdated but it passes the text
                <i>by value</i> instead.</li>
            <li>LineEdit::SenderTextSetByEnterKey is identical to
                LineEdit::SenderTextUpdated but is only signaled when the user
                hits the enter key.</li>
            <li>LineEdit::SenderTextSetByEnterKeyV is identical to
                LineEdit::SenderTextSetByEnterKey but it passes the text
                <i>by value</i> instead.</li>
            </ul>

        In each of SignalHandler's Connect methods, there are optional parameters
        to specify transformations to the values being passed in the signal.
        Specifically, in one instance we will be transforming the text to
        lowercase, and another analogously for uppercase.  There is no limit
        on the type or purpose for the transformations.  They're simply regular
        global functions (or static methods) which take one parameter -- using
        the exact signal parameter type as the parameter type and return value
        type.  For signals with more than one parameter, the transformations
        are the same -- one parameter, one return value -- the difference is
        that you specify as many transformations as there are parameters (or
        NULL if you do not want to transform the respective value).

        For connecting <tt>enter_text_line_edit</tt> to <tt>verbatim_label</tt>,
        we do not need to employ a transformation, so we can use the preferred
        <tt>std::string const &</tt> (pass by const reference) signal,
        LineEdit::SenderTextUpdated.

        For connecting <tt>enter_text_line_edit</tt> to <tt>lowercase_label</tt>
        and <tt>uppercase_label</tt>, we must use Transformation::Lowercase and
        Transformation::Uppercase respectively.  Here is where we <i>must</i>
        use the <tt>std::string</tt> (pass by value) signal,
        LineEdit::SenderTextUpdatedV, and here's why: say we pass a const
        reference into a transformation function.  That function can't modify
        the value passed in because it's const, so it must make its own copy
        of the value on the stack (or the heap, but I wouldn't touch that
        scandalousness with a 20 foot poo-stick).  Transformations must accept
        and return the exact type used by the signal, so in this case the
        transformation function would have to also return a const reference.
        A const reference to what?  The locally declared stack value which
        will be destructed upon the function returning.  Now we've got a const
        reference to whatever abomination is left where the stack variable
        used to be, which by the way, is an invalid address.  QED, motherfucker.

        Note that we're connecting a single SignalSender to many
        SignalReceivers.  There is no limit on the number of connections.
        When a sender signals, it will call the connected receivers in the
        order they were connected.
        @code */
        SignalHandler::Connect1(
            enter_text_line_edit->SenderTextUpdated(),
            verbatim_label->ReceiverSetText());
        SignalHandler::Connect1(
            enter_text_line_edit->SenderTextUpdatedV(),
            &Transformation::Lowercase,
            lowercase_label->ReceiverSetTextV());
        SignalHandler::Connect1(
            enter_text_line_edit->SenderTextUpdatedV(),
            &Transformation::Uppercase,
            uppercase_label->ReceiverSetTextV());
        /* @endcode
        From here on out is identical to the previous lesson -- the game loop
        will not change very much over the course of these lessons.

        Notice that we don't have to do anything to shutdown or destruct the
        signal connections we made above.  They are automatically disconnected
        when the respective objects are destroyed.  Thus you can even manually
        delete an object with active connections and it will disconnect itself
        automatically from everything it's connected to, making maintenance
        of SignalHandler code very much fire-and-forget.
        @code */
        // Run the game loop until the Screen feels like committing seppuku.
        while (!screen->IsQuitRequested())
        {
            // Sleep for 33 milliseconds to limit the framerate and avoid
            // hogging up too much CPU just for this crappy little GUI app.
            Singleton::Pal().Sleep(33);
            // Retrieve the current time in seconds as a Float.
            Float time = 0.001f * Singleton::Pal().CurrentTime();
            // Process events until there are no more.
            Event *event = NULL;
            while ((event = Singleton::Pal().PollEvent(screen, time)) != NULL)
            {
                // Let the InputState singleton "have a go" at keyboard/mouse events.
                if (event->IsKeyEvent() || event->IsMouseButtonEvent())
                    Singleton::InputState().ProcessEvent(event);
                // Hand the event to the top of the GUI hierarchy for
                // delegatory (is that a real word?) processing.
                screen->ProcessEvent(event);
                // Must delete the event ourselves here.
                Delete(event);
            }

            // Turn the crank on the EventQueue.
            screen->OwnerEventQueue()->ProcessFrame(time);
            // Perform all off-screen GUI hierarchy processing.
            screen->ProcessFrame(time);
            // Turn the crank on the EventQueue again.
            screen->OwnerEventQueue()->ProcessFrame(time);
            // Draw the whole mu'erfucking thing, passing real time in for
            // real time and world time (since there's still no world yet).
            screen->Draw(time);
        }
    }

    // Delete the Screen (and GUI hierarchy).
    Delete(screen);
    // this shuts down the Pal and the singletons.
    CleanUp();
    // return with success value.
    return 0;
}
/* @endcode

<strong>Exercises</strong>

    <ul>
    <li>Once again change the video mode flag parameter of the call to
        Screen::Create to <tt>true</tt>.  You will be able to quit the
        app because of the AWESOME quit button we created and hooked up.</li>
    <li>Change the major direction (first parameter) and the major count
        (second parameter) of the Layout constructor for
        <tt>text_signal_demo_layout</tt> to different values (<tt>ROW</tt> or
        <tt>COLUMN</tt> for major direction, and a positive integer for the
        major count) and see what effect it has on the locations of each of
        its child widgets.</li>
    <li>Screw around with the enabling/disabling buttons by pressing
        each and dragging the mouse cursor off the button before releasing,
        or clicking somewhere outside a button and dragging onto it before
        releasing, and see how they react.  Also note the visual
        highlighting while doing this.</li>
    <li>Add four more buttons analogous to the ones which disable/enable the
        label below them.  Hook them up to the hide/show receivers of said Label
        -- using ReceiverHide and ReceiverShow in place of ReceiverDisable and
        ReceiverEnable.  See what these new buttons do to the Label.</li>
    <li>Add another pair of Labels and signal connection analogous to that of
        uppercase_label.  Write your own custom <tt>std::string</tt>
        transformation which performs ROT13 encryption on the passed-in value
        (see http://en.wikipedia.org/wiki/ROT13).  Use this transformation in
        the newly added signal connection.</li>
    </ul>

Thus concludes lesson02.  Three down, countless more to go.
*/
