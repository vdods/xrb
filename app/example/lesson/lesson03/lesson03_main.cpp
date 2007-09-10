// ///////////////////////////////////////////////////////////////////////////
// lesson03_main.cpp by Victor Dods, created 2006/08/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////


// ///////////////////////////////////////////////////////////////////////////
// Lesson 03 - Creating Your Own Customized GUI Widgets
// ///////////////////////////////////////////////////////////////////////////


/** @page lesson03 Lesson 03 - Creating Your Own Customized GUI Widgets
@code *//* @endcode
This lesson will show you how to create custom GUI elements by subclassing
Widget.  It will involve creating a widget capable of responding to mouse
input, and a container widget which facilitates the interaction between its
children and provides other necessary control logic.  Also, we will slightly
change the game loop to add framerate control logic.

    <ul>
    <li>@ref lesson03_main.cpp "This lesson's source code"</li>
    <li>@ref lessons "Main lesson index"</li>
    </ul>

With the ability to create customized GUI, we can actually start to do something
interesting with the engine.  This lesson will be a simulation of heat
exchange on a 2 dimensional surface made up of rectangular grid cells.  Each
grid cell will indicate its temperature with colors, making a nice visual
representation of the temperature distribution throughout the material.

The cells will be implemented using a customized subclass of Widget (<tt>HeatButton</tt>),
and will be formed into a grid using the good old Layout class.  Furthermore,
we will create a custom container widget (<tt>HeatSimulation</tt>) to hold the grid,
the other widgets, and to facilitate interaction between the GUI elements.

<tt>HeatSimulation</tt> will contain all GUI elements necessary for the operation of
the application, so all that's required in the <tt>main</tt> function (besides
the previously covered initialization/game loop/shutdown stuff) is to create
an instance of HeatSimulation and set it as the main widget of Screen.

It should be noted that implementing this heat exchange simulation using a
massive grid of GUI widgets is inefficient and a retarded way to go,
but it's perfect for the purposes of this lesson.  Also note that
creating the dozens (or hundreds, if you modify <tt>GRID_WIDTH</tt> and
<tt>GRID_HEIGHT</tt>) of widgets inside the Layout will take a long time due
to the (as of August 2006) calculation-intensive Layout resizing code.

<strong>Procedural Overview</strong> -- Items in bold are additions/changes to the previous lesson.

    <ul>
    <li><strong>Global declarations</strong></li>
        <ul>
        <li><strong>Tuning values</strong></li>
            <ul>
            <li><strong><tt>g_desired_framerate</tt></strong></li>
            <li><strong><tt>g_mouse_temperature_change_rate</tt></strong></li>
            <li><strong><tt>g_temperature_retention_rate</tt></strong></li>
            </ul>
        <li><strong>HeatButton subclass of Button</strong></li>
            <ul>
            <li><strong>In the constructor, set <tt>m_accepts_mouseover</tt> to
                <tt>true</tt>.</strong></li>
            <li><strong>Specify accessors/modifiers for current and ambient
                temperature member variables.</strong></li>
            <li><strong>Override Widget::Draw for customized drawing code --
                fill the widget with a solid color representing its
                temperature.</strong></li>
            <li><strong>Override Widget::HandleFrame for customized
                off-screen, per-frame processing -- update the current
                temperature based on the ambient temperature and react to
                mouse input.</strong></li>
            </ul>
        <li><strong>HeatSimulation subclass of Widget</strong></li>
            <ul>
            <li><strong>In the constructor, create the grid of HeatButton
                widgets along with other control widgets, and make necessary
                signal connections.</strong></li>
            <li><strong>Override ContainerWidget::HandleFrame for customized
                off-screen, per-frame processing -- calculate and set the
                ambient temperature for each HeatButton.</strong></li>
            <li><strong>Define the static member
                HeatSimulation::ms_distribution_function for use in
                HeatSimulation::HandleFrame.</strong></li>
            </ul>
        </ul>
    <li>Main function</li>
        <ul>
        <li>Initialize SDL, engine singletons and create the Screen object</li>
        <li>Execute game-specific code.</li>
            <ul>
            <li>Create application-specific GUI and connect necessary signals.</li>
                <ul>
                <li><strong>Create an instance of HeatSimulation and set it as the screen's main widget.</strong></li>
                </ul>
            <li>Run the game loop</li>
                <ul>
                <li><strong>Calculate the SDL_Delay duration necessary to achieve the desired framerate.</strong></li>
                <li>Handle events (user and system-generated).</li>
                <li>Perform off-screen processing.</li>
                <li>Draw the Screen object's entire widget hierarchy.</li>
                </ul>
            </ul>
        <li>Delete the Screen object and shutdown engine singletons and SDL.</li>
        </ul>
    </ul>

Comments explaining previously covered material will be made more terse or
deleted entirely in each successive lesson.  If something is not explained
well enough, it was probably already explained in
@ref lessons "previous lessons".

<strong>Code Diving!</strong>

@code */
#include "xrb.h"                    // Must be included in every source/header file.

#include "xrb_containerwidget.h"    // For use of the ContainerWidget class.
#include "xrb_event.h"              // For use of the Event classes.
#include "xrb_eventqueue.h"         // For use of the EventQueue class.
#include "xrb_input.h"              // For use of the Input class (via Singletons::).
#include "xrb_label.h"              // For use of the Label class.
#include "xrb_layout.h"             // For use of the Layout widget class.
#include "xrb_render.h"             // For use of the Render namespace functions.
#include "xrb_screen.h"             // For use of the necessary Screen widget class.
#include "xrb_validator.h"          // For use of various Validator subclasses.
#include "xrb_valueedit.h"          // For use of the ValueEdit<T> template class.
#include "xrb_valuelabel.h"         // For use of the ValueLabel<T> template class.
#include "xrb_widget.h"             // For use of the Widget class.

using namespace Xrb;                // To avoid having to use Xrb:: everywhere.

/* @endcode
Define some global variables to hold the tuning parameters for the simulation.
Normally globals aren't the best way to accomplish this sort of task because it
makes for non-reentrant code, but for this lesson, it works.
<tt>g_desired_framerate</tt> is the target framerate for the game loop.  The
actual framerate may not match if the computer is not fast enough.
<tt>g_mouse_temperature_change_rate</tt> is the number of
@code */
Float g_desired_framerate = 30.0f;
Float g_mouse_temperature_change_rate = 200.0f;
Float g_temperature_retention_rate = 0.001f;

/* @endcode
Here we declare our awesome first customized Widget subclass.  It should
inherit Widget publicly.  We will override certain virtual methods from
Widget to specify our widget's custom behavior.

Our widget will behave as a single cell in a grid of heat-sensitive material,
so it will have a "temperature" value and an "ambient temperature" value.
The temperature value is the current temperature of this grid cell, and will
determine what color the cell is.  The ambient temperature value is the
effective external temperature experienced by this grid cell, and will be used
in per-frame calculations to update the temperature value.

The constructor must accept as an argument a pointer to the parent Widget,
and optionally a name (which we will assign a default value of "HeatButton").
The constructor can also accept whatever other parameters you want, though no
extra are needed here.  The convention is to put additional parameters before
the superclass' parameters (see Label, Button, ValueLabel, etc).

For this widget, there are 2 Widget methods we will override to implement
the custom behavior -- <tt>Draw</tt> and <tt>HandleFrame</tt>.  Widget
provides many many overridable virtual methods to facilitate behavior
customization.
@code */
class HeatButton : public Widget
{
public:

    /* @endcode
    Here's our lovely constructor with its awesome parameters which we just
    pass directly to the superclass constructor.  We will initialize the
    temperature and ambient temperature values to a nice dumb default of zero.
    Note how we set <tt>m_accepts_mouseover</tt> to <tt>true</tt> here.
    <tt>m_accepts_mouseover</tt> is a protected member of Widget which
    indicates if mouseover events will be caught and processed by this widget.
    If we want to use the Widget::GetIsMouseover accessor, this must be set to
    <tt>true</tt>.  If <tt>m_accepts_mouseover</tt> remains the default value
    of <tt>false</tt>, this widget will allow mouseover events to fall through
    to the widget(s) behind it, including its parent widget, which may be
    desirable if you're writing custom HUD widgets for a game.
    @code */
    HeatButton (ContainerWidget *parent, std::string const &name = "HeatButton")
        :
        Widget(parent, name)
    {
        m_temperature = 0.0f;
        m_ambient_temperature = 0.0f;

        m_accepts_mouseover = true;
    }

    // Accessor for temperature -- used in HeatSimulation::HandleFrame.
    inline Float GetTemperature () const { return m_temperature; }
    // Modifier for ambient temperature -- also used in HeatSimulation::HandleFrame.
    inline void SetAmbientTemperature (Float ambient_temperature)
    {
        m_ambient_temperature = ambient_temperature;
    }

    /* @endcode
    Here is the override of Widget::Draw.  This method is predictably used
    to specify how to draw the widget.  The single parameter is a RenderContext
    which is something which all drawing functions require (it is used by the
    drawing/rendering primitive functions in Xrb::Render).  For HeatButton in
    particular, all we want to do is draw a rectangle of a specific color,
    filling the entire widget.
    @code */
    virtual void Draw (RenderContext const &render_context) const
    {
        // Normalize the temperature from (-inf, +inf) to [0, 1] so we'll
        // have a value that can parameterize linear interpolations with to
        // calculate the color.  A temperature of 0 will translate into
        // a normalized_temperature of 0.5.
        Float normalized_temperature = 0.5f * (1.0f + Math::Atan(m_temperature) / 90.0f);
        // Calculate the red, green and blue components for the color.  Each
        // component in the RGBA color value must be within the range [0, 1],
        // with an alpha value of 0 being completely transparent and an alpha
        // value of 1 being completely opaque.  We'll just use a boring old
        // greyscale gradient for simplicity.  Because of the above
        // normalization, a temperature of 0 will be 50% grey, a temperature
        // approaching -infinity will approach pure black, and a temperature
        // approaching +infinity will approach pure white.
        Color button_color(
            normalized_temperature, // red component
            normalized_temperature, // green component
            normalized_temperature, // blue component
            1.0f);                  // alpha component (completely opaque)
        // Draw a rectangle which fills this widget's screen rect using the
        // calculated color.
        Render::DrawScreenRect(render_context, button_color, GetScreenRect());
    }

protected:

    /* @endcode
    Here is the override of Widget::HandleFrame.  This method is what
    performs all the off-screen, per-frame computation specific to the widget.
    For HeatButton specifically, it is to recalculate <tt>m_temperature</tt> using
    <tt>m_ambient_temperature</tt> and to modify <tt>m_temperature</tt> based on mouse
    input.  The HandleFrame method originally comes from FrameHandler
    (which is inherited by Widget) and there are a few notable provided methods:
    FrameHandler::GetFrameTime and FrameHandler::GetFrameDT which are only
    available during the execution of HandleFrame (or in a function
    called by it), and FrameHandler::GetMostRecentFrameTime which is available
    at any time.
    @code */
    virtual void HandleFrame ()
    {
        ASSERT1(g_temperature_retention_rate > 0.0f);
        ASSERT1(g_temperature_retention_rate < 1.0f);
        // Calculate heat transfer.  The amount is proportional to the
        // difference between m_temperature and m_ambient_temperature -- the
        // fancy looking exponential factor is used to correctly scale the
        // amount based on time. g_temperature_retention_rate is the ratio of
        // a grid cell's heat relative to the ambient temperature it retains
        // over one second. Thus, a high value means the heat spreads slowly,
        // while a low value causes heat to spread quickly.
        m_temperature +=
            (m_ambient_temperature - m_temperature) *
            (1.0f - Math::Pow(g_temperature_retention_rate, GetFrameDT()));

        // If the mouse cursor is currently over this widget and the left mouse
        // button is pressed, increase the temperature.  This allows the user
        // to manually heat up grid cells.  Note that this isn't the primary
        // method of facilitating mouse input -- Xrb::Event based mouse input
        // will be covered later.
        if (GetIsMouseover() && Singletons::Input().GetIsKeyPressed(Key::LEFTMOUSE))
            m_temperature += g_mouse_temperature_change_rate * GetFrameDT();
    }

private:

    Float m_temperature;
    Float m_ambient_temperature;
}; // end of class HeatButton

/* @endcode
Here is the declaration of our other super-awesome custom Widget subclass.
It will also inherit Widget publicly (actually I can't think of a reason you'd
ever not inherit Widget publicly).

The constructor will create and initialize all the subordinate GUI elements
and make necessary signal connections.

We will only need to override ContainerWidget::HandleFrame for the desired
custom behavior, since a container widget is only rendered to screen by proxy
through its children's Draw methods.
@code */
class HeatSimulation : public ContainerWidget
{
public:

    /* @endcode
    The constructor for <tt>HeatSimulation</tt> is similar to that of <tt>HeatButton</tt>
    -- it accepts the parent widget and a widget name as parameters, and passes
    them directly to the superclass constructor.

    Notice the constructors for the member variables
    <tt>m_desired_framerate_validator</tt> and
    <tt>m_temperature_retention_range_validator</tt>.  They are instances of
    implementations of the Validator interface class which is used to constrain
    values to preset valid ranges.  These are primarily useful in user interface
    code where the user may enter any retarded value, but must be constrained
    to a particular range.  The motivation to use these seemingly unwieldy
    objects is to avoid having to add in extra code to manually correct invalid
    values.  The goal is to have all GUI elements, signal connections, value
    validation (and really everything in general) to be as plug-and-forget
    (pronounced "hard to fuck up") as possible.  The Validator subclasses are
    templatized for your convenience.

    In the game loop, the calculations to determine the value to pass to
    SDL_Delay involves dividing by <tt>g_desired_framerate</tt>, and therefore we must
    avoid setting <tt>g_desired_framerate</tt> to zero.
    <tt>m_desired_framerate_validator</tt> is an instance of
    GreaterOrEqualValidator<Uint32> -- we will limit the desired framerate to
    be greater or equal to 1 (the parameter passed into the constructor for
    <tt>m_desired_framerate_validator</tt>).

    Note that unlike the constructor for <tt>HeatButton</tt>, we do <i>not</i>
    set <tt>m_accepts_mouseover</tt> to <tt>true</tt>.  This is because
    ContainerWidgets by themselves should not accept mouseover -- only their
    children should.  This is to prevent ContainerWidgets which contain only
    <tt>m_accepts_mouseover = false</tt> from accepting mouseover instead of
    perhaps some background widget (such as a game view widget) which may want
    it.  For example, an in-game HUD Layout containing only Labels -- you would
    not want it or its children accepting mouseover and blocking the game view
    widget from accepting mouseover.
    @code */
    HeatSimulation (ContainerWidget *parent, std::string const &name = "HeatSimulation")
        :
        ContainerWidget(parent, "HeatSimulation"),
        /* @endcode
        The single argument to this validator's constructor is the lower
        bound for GreaterOrEqualValidator<Uint32>.  Anything greator or equal
        to 1 is valid.
        @code */
        m_desired_framerate_validator(1),
        /* @endcode
        There are two classifications of range validators.  Inclusive
        (GreaterOrEqualValidator, LessOrEqualValidator and
        RangeInclusiveValidator) and Exclusive (GreaterThanValidator,
        LessThanValidator and RangeExclusiveValidator).  The inclusive range
        validators can simply define the lower and/or upper bounds, but the
        exclusive range validators must specify both lower and/or upper bounds,
        as well as minimum and/or maximum valid values (in order to bring an
        invalid value into the valid range).  Because the validator subclasses
        are templatized, one can't assume that one could just add/subtract 1
        (or some other general value) to the lower/upper bound to get the
        minimum/maximum valid value.  Thus, the necessity for a minimum and/or
        maximum valid value parameter.

        In the following constructor -- for RangeExclusive<Float> -- the first
        parameter is the lower bound of the exclusive range.  The second is
        the minimum valid value which will be used if a value-to-be-validated
        is less than or equal to the lower bound value.  The third parameter
        is the maximum valid value.  It is similarly used when a
        value-to-be-validated is greater or equal to the upper bound value.
        The final parameter is the upper bound of the exclusive range.  See
        @ref Xrb::RangeExclusiveValidator for more.
        @code */
        m_temperature_retention_range_validator(0.0f, 1e-20f, 0.999999f, 1.0f),
        m_internal_receiver_set_desired_framerate(&HeatSimulation::SetDesiredFramerate, this),
        m_internal_receiver_set_temperature_retention_rate(&HeatSimulation::SetTemperatureRetentionRate, this)
    {
        Layout *main_layout = new Layout(VERTICAL, this);
        main_layout->SetIsUsingZeroedLayoutSpacingMargins(true);
        SetMainWidget(main_layout);

        Layout *grid_layout = new Layout(ROW, GRID_WIDTH, main_layout, "grid layout");
        /* @endcode
        This next variable declaration is a little mechanism to speed up
        creation of complex (many-widget) GUI containers.  The
        ContainerWidget::ChildResizeBlocker object blocks all children of
        the ContainerWidget passed to its constructor from being resized or
        otherwise changing size properties (min/max size, etc).  The idea is
        that it's pointless to resize all the widgets in a ContainerWidget
        before they're all added.  ContainerWidget::ChildResizeBlocker's
        destructor is what unblocks the ContainerWidget and resizes the child
        widgets appropriately.

        ContainerWidget::ChildResizeBlocker must be created as a stack variable
        (no new'ing one up on the heap) so that when it goes out of scope and
        its destructor is called, the applicable ContainerWidget is unblocked.
        The rationale is again to provide a plug-and-forget mechanism for ease
        of use and to reduce the possibility of programmer error.
        @code */
        ContainerWidget::ChildResizeBlocker blocker(grid_layout);
        grid_layout->SetIsUsingZeroedFrameMargins(true);
        grid_layout->SetIsUsingZeroedLayoutSpacingMargins(true);

        // Create the HeatButton grid in a doubly-nested loop, saving pointers
        // to each grid cell in the 2 dimensional array, m_button_grid for
        // use in temperature calculations.
        for (Uint32 y = 0; y < GRID_HEIGHT; ++y)
            for (Uint32 x = 0; x < GRID_WIDTH; ++x)
                m_button_grid[y][x] = new HeatButton(grid_layout);

        // Create a layout for the controls below the grid and enable
        // the frame margins (which are zeroed-out by default).
        Layout *sub_layout = new Layout(HORIZONTAL, main_layout);
        sub_layout->SetIsUsingZeroedFrameMargins(false);

        // ValueLabel<T> is a templatized subclass of Label which contains a
        // value instead of a string.  It has GetValue and SetValue methods,
        // and corresponding SignalSenders and SignalReceivers.  It is very
        // flexible, due to its value-to-text-printf-format and
        // text-to-value-function constructor parameters.
        new Label("Actual Framerate:", sub_layout);
        m_actual_framerate_label = new ValueLabel<Uint32>("%u", Util::TextToUint32, sub_layout);
        m_actual_framerate_label->SetAlignment(Dim::X, LEFT);

        // ValueEdit<T> is a templatized subclass of LineEdit, analogous to
        // ValueLabel<T>.  You can type into it and it will attempt to use
        // the specified text-to-value-function to convert it to a value.
        // A validator may also be specified to enforce valid input values.
        new Label("Desired Framerate:", sub_layout);
        ValueEdit<Uint32> *desired_framerate_edit =
            new ValueEdit<Uint32>("%u", Util::TextToUint32, sub_layout);
        desired_framerate_edit->SetValidator(&m_desired_framerate_validator);
        desired_framerate_edit->SetValue(static_cast<Uint32>(Math::Round(g_desired_framerate)));
        // We'll connect this one to this HeatSimulation's SetDesiredFramerate
        // SignalReceiver.
        SignalHandler::Connect1(
            desired_framerate_edit->SenderValueUpdated(),
            &m_internal_receiver_set_desired_framerate);

        // Similarly create a ValueEdit for the temperature retention rate
        // and connect it up.
        new Label("Temperature Retention Rate:", sub_layout);
        ValueEdit<Float> *temperature_retention_rate_edit =
            new ValueEdit<Float>("%g", Util::TextToFloat, sub_layout);
        temperature_retention_rate_edit->SetValue(g_temperature_retention_rate);
        temperature_retention_rate_edit->SetValidator(&m_temperature_retention_range_validator);
        SignalHandler::Connect1(
            temperature_retention_rate_edit->SenderValueUpdated(),
            &m_internal_receiver_set_temperature_retention_rate);

        // Ensure DISTRIBUTION_FUNCTION_WIDTH and DISTRIBUTION_FUNCTION_HEIGHT
        // are odd, so that there is an exact center in the array.
        ASSERT0(DISTRIBUTION_FUNCTION_WIDTH % 2 == 1);
        ASSERT0(DISTRIBUTION_FUNCTION_HEIGHT % 2 == 1);
        // Make sure the center weight in the distribution function
        // (representing the target square) is zero.
        ASSERT0(ms_distribution_function[DISTRIBUTION_FUNCTION_HEIGHT/2][DISTRIBUTION_FUNCTION_WIDTH/2] == 0.0f);
        // Calculate the total of all weights in the distribution function,
        // so that later calculations can divide by this value and be ensured
        // that the adjusted total weight of the distribution function is one.
        m_distribution_normalization = 0.0f;
        for (Uint32 y = 0; y < DISTRIBUTION_FUNCTION_HEIGHT; ++y)
            for (Uint32 x = 0; x < DISTRIBUTION_FUNCTION_WIDTH; ++x)
                m_distribution_normalization += ms_distribution_function[y][x];
    }

protected:

    /* @endcode
    Our customized override of ContainerWidget::HandleFrame is where the
    ambient temperature for each grid cell is computed.  We also call the
    superclass' HandleFrame method and keep track of the framerate
    in order to display the "Actual Framerate:" value.  The reason we have to
    call ContainerWidget::HandleFrame is because that's where the child
    widgets' ProcessFrameOverrides are recursively called.
    @code */
    virtual void HandleFrame ()
    {
        // must call the superclass' HandleFrame -- this is where
        // all the child widgets' ProcessFrameOverrides are called.
        ContainerWidget::HandleFrame();

        // Keep track of the framerate and update the "Actual Framerate" label.
        m_framerate_calculator.AddFrameTime(GetFrameTime());
        m_actual_framerate_label->SetValue(
            static_cast<Uint32>(Math::Round(m_framerate_calculator.GetFramerate())));

        // Compute and set the ambient temperature for each grid cell,
        // using the distribution function.
        for (Sint32 gy = 0; gy < GRID_HEIGHT; ++gy)
            for (Sint32 gx = 0; gx < GRID_WIDTH; ++gx)
            {
                // Calculate the ambient temperature by summing the weighted
                // temperatures from adjacent grid cells indicated by the
                // distribution function.
                Float ambient_temperature = 0.0f;
                for (Sint32 dy = -DISTRIBUTION_FUNCTION_HEIGHT/2; dy <= DISTRIBUTION_FUNCTION_HEIGHT/2; ++dy)
                    for (Sint32 dx = -DISTRIBUTION_FUNCTION_WIDTH/2; dx <= DISTRIBUTION_FUNCTION_WIDTH/2; ++dx)
                        // If the array indices would go out of bounds, don't
                        // add to the running total ambient temperature.
                        if (gy+dy >= 0 && gy+dy < GRID_HEIGHT && gx+dx >= 0 && gx+dx < GRID_WIDTH)
                            ambient_temperature +=
                                ms_distribution_function[dy+DISTRIBUTION_FUNCTION_HEIGHT/2][dx+DISTRIBUTION_FUNCTION_WIDTH/2] /
                                m_distribution_normalization *
                                m_button_grid[gy+dy][gx+dx]->GetTemperature();
                m_button_grid[gy][gx]->SetAmbientTemperature(ambient_temperature);
            }
    }

private:

    // Various enums for named constants (array dimension sizes in this case).
    enum
    {
        GRID_WIDTH = 16,
        GRID_HEIGHT = 12,
        DISTRIBUTION_FUNCTION_WIDTH = 5,
        DISTRIBUTION_FUNCTION_HEIGHT = 5
    };

    /* @endcode
    This is a method use solely by <tt>m_internal_receiver_set_desired_framerate</tt>
    -- when said receiver is signaled, it sets <tt>g_desired_framerate</tt> with the
    specified value.  Specifically, the receiver will be hooked up to the
    desired framerate ValueEdit<Uint32>::SignalValueUpdated signal.
    @code */
    void SetDesiredFramerate (Uint32 desired_framerate)
    {
        ASSERT1(desired_framerate > 0);
        g_desired_framerate = static_cast<Float>(desired_framerate);
    }
    /* @endcode
    This method is similarly used solely by
    <tt>m_internal_receiver_set_temperature_retention_rate</tt> -- to set
    <tt>g_temperature_retention_rate</tt>, and will be hooked up to the
    temperature retention rate ValueEdit<Float>::SignalValueUpdated signal.
    @code */
    void SetTemperatureRetentionRate (Float temperature_retention_rate)
    {
        ASSERT1(temperature_retention_rate > 0.0f);
        ASSERT1(temperature_retention_rate < 1.0f);
        g_temperature_retention_rate = temperature_retention_rate;
    }

    HeatButton *m_button_grid[GRID_HEIGHT][GRID_WIDTH];
    ValueLabel<Uint32> *m_actual_framerate_label;
    FramerateCalculator m_framerate_calculator;
    GreaterOrEqualValidator<Uint32> m_desired_framerate_validator;
    RangeExclusiveValidator<Float> m_temperature_retention_range_validator;
    Float m_distribution_normalization;

    SignalReceiver1<Uint32> m_internal_receiver_set_desired_framerate;
    SignalReceiver1<Float> m_internal_receiver_set_temperature_retention_rate;

    static Float const ms_distribution_function[DISTRIBUTION_FUNCTION_HEIGHT][DISTRIBUTION_FUNCTION_WIDTH];
}; // end of class HeatSimulation

Float const HeatSimulation::ms_distribution_function[DISTRIBUTION_FUNCTION_HEIGHT][DISTRIBUTION_FUNCTION_WIDTH] =
{
    { 0.00f, 0.05f, 0.10f, 0.05f, 0.00f },
    { 0.05f, 0.15f, 0.30f, 0.15f, 0.05f },
    { 0.10f, 0.30f, 0.00f, 0.30f, 0.10f },
    { 0.05f, 0.15f, 0.30f, 0.15f, 0.05f },
    { 0.00f, 0.05f, 0.10f, 0.05f, 0.00f }
};

void CleanUp ()
{
    fprintf(stderr, "CleanUp();\n");
    // shutdown engine singletons, ungrab the mouse, and shutdown SDL.
    Singletons::Shutdown();
    SDL_WM_GrabInput(SDL_GRAB_OFF);
    SDL_Quit();
}

int main (int argc, char **argv)
{
    fprintf(stderr, "main();\n");

    // Attempt to initialize SDL.
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE) < 0)
    {
        fprintf(stderr, "unable to initialize video.  error: %s\n", SDL_GetError());
        return 1;
    }

    // Initialize engine singletons, set window caption and create the Screen.
    Singletons::Initialize("none");
    SDL_WM_SetCaption("XuqRijBuh Lesson 03", "");
    Screen *screen = Screen::Create(800, 600, 32, 0);
    // If the Screen failed to initialize, print an error message and quit.
    if (screen == NULL)
    {
        fprintf(stderr, "unable to initialize video mode\n");
        CleanUp();
        return 2;
    }

    // SDL has been initialized, the video mode is set and the engine is ready.
    // Here is where the application-specific code begins.
    {
        // this is the only child of Screen
        HeatSimulation *main_widget = new HeatSimulation(screen);
        screen->SetMainWidget(main_widget);

        // These values will be used below in the framerate control code.
        Float current_real_time = 0.0f;
        Float next_real_time = 0.0f;
        // Run the game loop until the Screen no longer has the will to live.
        while (!screen->GetIsQuitRequested())
        {
            /* @endcode
            Here is the newly added framerate control code.  We keep track of
            the desired next frame time, and use it in combination with the
            real time as returned by <tt>SDL_GetTicks</tt> to calculate how
            long to sleep to attempt to achieve the exact desired framerate.
            @code */
            // Retrieve the current real time in seconds as a Float.
            current_real_time = 0.001f * SDL_GetTicks();
            // figure out how much time to sleep before processing the next frame
            Sint32 milliseconds_to_sleep = Max(0, static_cast<Sint32>(1000.0f * (next_real_time - current_real_time)));
            // Delay for the calculated number of milliseconds.
            SDL_Delay(milliseconds_to_sleep);
            // Calculate the desired next game loop time (which should never
            // fall below current_real_time.
            next_real_time = Max(current_real_time, next_real_time + 1.0f / g_desired_framerate);

            // Process SDL events until there are no more.
            SDL_Event sdl_event;
            while (SDL_PollEvent(&sdl_event))
            {
                // Repackage SDL_Event into Xrb::Event subclasses, skipping it
                // if it was a dud.
                Event *event = Event::CreateEventFromSDLEvent(&sdl_event, screen, current_real_time);
                if (event == NULL)
                    continue;
                // Let the Input singleton "have a go" at keyboard/mouse events.
                if (event->GetIsKeyEvent() || event->GetIsMouseButtonEvent())
                    Singletons::Input().ProcessEvent(event);
                // Give the GUI hierarchy a chance at the event and then delete it.
                screen->ProcessEvent(event);
                Delete(event);
            }

            // Turn the EventQueue crank, Perform off-screen GUI processing,
            // turn the EventQueue crank again, and then draw everything.
            screen->GetOwnerEventQueue()->ProcessFrame(current_real_time);
            screen->ProcessFrame(current_real_time);
            screen->GetOwnerEventQueue()->ProcessFrame(current_real_time);
            screen->Draw();
        }
    }

    // Delete the Screen (and GUI hierarchy), "SHUT IT DOWN", and return success.
    Delete(screen);
    CleanUp();
    return 0;
}
/* @endcode

<strong>Exercises</strong>

    <ul>
    <li>Screw with <tt>grid_layout</tt>'s SetIsUsingZeroedFrameMargins and
        SetIsUsingZeroedLayoutSpacingMargins and observe the visual change.</li>
    <li>Change the color gradient code to make a fire-like effect.  Let a
        normalized temperature of 0.5 be black, 0.7 be red, 0.9 be yellow
        and 1.0 be white.  Tune these values to your liking.</li>
    <li>Make it so the right mouse button decreases a grid cell's temperature.</li>
    <li>Modify the color gradient code to make temperatures below zero appear
        using ice-like colors, while preserving the previously added fire-like
        colors.  Let a normalized temperature of 0.3 be dark blue, 0.1 be light
        blue and 0.0 be white.  Again, tune these values to your liking.</li>
    <li>Change <tt>HeatSimulation::m_button_grid</tt> and
        <tt>HeatSimulation::ms_distribution_function</tt> to be 1-dimensional
        arrays, and perform the row-major array indexing yourself.  They are
        both currently indexed with the Y component first to make this switch
        easier.</li>
    <li>Change <tt>HeatSimulation::ms_distribution_function</tt> to cause the
        temperature changes to propogate up the screen like fire.  Along the
        bottom row of the grid, simulate fluctuating embers via random
        temperatures.  You may need to add additional methods to <tt>HeatButton</tt>
        and/or <tt>HeatSimulation</tt>.</li>
    <li>Make the heat dissipation wrap top/bottom and left/right on the
        grid (you might want to change
        <tt>HeatSimulation::ms_distribution_function</tt> back to its original
        value and disable the fluctuating ember code before you do this).</li>
    <li>Add a reset button to the row of widgets at the bottom of the screen
        to reset all the grid cells' temperatures to 0</li>
    <li>Add a quit button next to the reset button (recall the
        Screen::ReceiverRequestQuit method from @ref lesson02 "lesson02").</li>
    </ul>

Thus concludes lesson03.  Somehow you're actually dumber now from it.
*/
