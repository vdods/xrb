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
children and provides other necessary control logic.

    <ul>
    <li>@ref lesson03_main.cpp "This lesson's source code"</li>
    <li>@ref lessons "Main lesson index"</li>
    </ul>

<strong>Procedural Overview</strong> -- Items in bold are additions/changes to the previous lesson.

    <ul>
    <li><strong>Global declarations</strong></li>
        <ul>
        <li><strong>Declare FireButton subclass of Button</strong></li>
        <li><strong>Define FireButton methods</strong></li>
        <li><strong>Declare FireGrid subclass of Widget</strong></li>
        <li><strong>Define FireGrid methods</strong></li>
        </ul>
    <li>Main function</li>
        <ul>
        <li>Initialize SDL, engine singletons and create the Screen object</li>
        <li>Execute game-specific code.</li>
            <ul>
            <li>Create application-specific GUI and connect necessary signals.</li>
            <li>Run the game loop</li>
                <ul>
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
// This header MUST be included in every source/header file.
#include "xrb.h"

#include "xrb_event.h"          // For use of the Event classes
#include "xrb_eventqueue.h"     // For use of the EventQueue class
#include "xrb_input.h"          // For use of the Input class (via Singletons::)
#include "xrb_layout.h"         // For use of the Layout widget class
#include "xrb_render.h"         // For use of the Render namespace functions
#include "xrb_screen.h"         // For use of the necessary Screen widget class
#include "xrb_valueedit.h" // TEMP
#include "xrb_valuelabel.h" // temp
#include "xrb_label.h" // temp

// Used so we don't need to qualify every library type/class/etc with Xrb::
using namespace Xrb;

/*
FireButton contains a temperature value in [0, 1] which maps onto a color
gradient.  The button's color is specified by using the temperature as the
parameter in the gradient.  Pressing the button will increase/top-out the
temperature.  There will be a distribution function which will indicate
exactly how the temperature of the buttons is dissipated to others over time.
These elements combined will cause a fire or rippling effect over the grid
of buttons.
*/

Float g_framerate = 30.0f;
Float g_mouse_temperature_change_rate = 200.0f;
Float g_temperature_retention_rate = 0.001f;

class FireButton : public Widget
{
public:

    FireButton (Widget *parent);

    inline Float GetTemperature () const { return m_temperature; }

    inline void SetAmbientTemperature (Float ambient_temperature) { m_ambient_temperature = ambient_temperature; }

    virtual void Draw (RenderContext const &render_context) const;

protected:

    virtual void ProcessFrameOverride ();

private:

    Float m_temperature;
    Float m_ambient_temperature;
};

FireButton::FireButton (Widget *const parent)
    :
    Widget(parent, "FireButton")
{
    m_accepts_mouseover = true;

    m_temperature = 0.0f;
    m_ambient_temperature = 0.0f;
}

void FireButton::Draw (RenderContext const &render_context) const
{
    // normalize the temperature from the domain [-inf, +inf] to [0, 1]
    Float normalized_temperature = 0.5f * (1.0f + Math::Atan(m_temperature) / 90.0f);
    // boring old linear grayscale color gradient
    Color button_color(
        normalized_temperature, // red component
        normalized_temperature, // green component
        normalized_temperature, // blue component
        1.0f);                  // alpha component
    // draw a rectangle which fills this widget's screen rect
    // using the calculated color.
    Render::DrawScreenRect(render_context, button_color, GetScreenRect());
}

void FireButton::ProcessFrameOverride ()
{
    ASSERT1(g_temperature_retention_rate > 0.0f)
    ASSERT1(g_temperature_retention_rate < 1.0f)
    m_temperature += (m_ambient_temperature - m_temperature) * (1.0f - Math::Pow(g_temperature_retention_rate, GetFrameDT()));

    if (GetIsMouseover() && Singletons::Input().GetIsKeyPressed(Key::LMOUSE))
    {
        m_temperature += g_mouse_temperature_change_rate * GetFrameDT();
    }

    // TEMP
    // TEMP
    if (GetIsMouseover() && Singletons::Input().GetIsKeyPressed(Key::RMOUSE))
    {
        m_temperature -= g_mouse_temperature_change_rate * GetFrameDT();
    }
    // TEMP
    // TEMP
}

class FireGrid : public Widget
{
public:

    FireGrid (Widget *parent);

protected:

    virtual void ProcessFrameOverride ();

private:

    enum
    {
        GRID_WIDTH = 12,
        GRID_HEIGHT = 15,
        DISTRIBUTION_FUNCTION_WIDTH = 5,
        DISTRIBUTION_FUNCTION_HEIGHT = 5
    };

    void SetFramerate (Float framerate)
    {
        ASSERT0(framerate > 0.0f)
        g_framerate = framerate;
    }
    void SetTemperatureRetentionRate (Float temperature_retention_rate)
    {
        ASSERT0(temperature_retention_rate > 0.0f)
        ASSERT0(temperature_retention_rate < 1.0f)
        g_temperature_retention_rate = temperature_retention_rate;
    }

    static Float const ms_distribution_function[DISTRIBUTION_FUNCTION_HEIGHT][DISTRIBUTION_FUNCTION_WIDTH];

    Layout *m_grid_layout;
    FireButton *m_button_grid[GRID_HEIGHT][GRID_WIDTH];
    Float m_distribution_normalization;

    SignalReceiver1<Float> m_internal_receiver_set_framerate;
    SignalReceiver1<Float> m_internal_receiver_set_temperature_retention_rate;
};

Float const FireGrid::ms_distribution_function[DISTRIBUTION_FUNCTION_HEIGHT][DISTRIBUTION_FUNCTION_WIDTH] =
{
/*
    { 0.00f, 0.00f, 0.00f, 0.00f, 0.00f },
    { 0.00f, 0.00f, 0.00f, 0.00f, 0.00f },
    { 0.00f, 0.10f, 0.00f, 0.10f, 0.00f },
    { 0.00f, 0.05f, 0.50f, 0.05f, 0.00f },
    { 0.03f, 0.02f, 0.15f, 0.02f, 0.03f }
*/
    { 0.00f, 0.05f, 0.10f, 0.05f, 0.00f },
    { 0.05f, 0.15f, 0.30f, 0.15f, 0.05f },
    { 0.10f, 0.30f, 0.00f, 0.30f, 0.10f },
    { 0.05f, 0.15f, 0.30f, 0.15f, 0.05f },
    { 0.00f, 0.05f, 0.10f, 0.05f, 0.00f }
};

FireGrid::FireGrid (Widget *parent)
    :
    Widget(parent, "FireGrid"),
    m_internal_receiver_set_framerate(&FireGrid::SetFramerate, this),
    m_internal_receiver_set_temperature_retention_rate(&FireGrid::SetTemperatureRetentionRate, this)
{
    Layout *main_layout = new Layout(VERTICAL, this);

    m_grid_layout = new Layout(ROW, GRID_WIDTH, main_layout, "grid layout");
    m_grid_layout->SetIsUsingZeroedFrameMargins(true);
    m_grid_layout->SetIsUsingZeroedLayoutSpacingMargins(true);

    Uint32 start = SDL_GetTicks();
    for (Uint32 y = 0; y < GRID_HEIGHT; ++y)
        for (Uint32 x = 0; x < GRID_WIDTH; ++x)
            m_button_grid[y][x] = new FireButton(m_grid_layout);
    Uint32 stop = SDL_GetTicks();
    fprintf(stderr, "average button creation duration = %.5fs\n", 0.001f * (stop - start) / (GRID_WIDTH * GRID_HEIGHT));

    Layout *sub_layout = new Layout(HORIZONTAL, main_layout);
    sub_layout->SetIsUsingZeroedFrameMargins(true);
    new Label("Desired framerate:", sub_layout);
    ValueEdit<Float> *framerate_edit = new ValueEdit<Float>("%g", Util::TextToFloat, sub_layout);
    framerate_edit->SetValue(g_framerate);
    SignalHandler::Connect1(
        framerate_edit->SenderValueUpdated(),
        &m_internal_receiver_set_framerate);
    new Label("Temperature retention rate:", sub_layout);
    ValueEdit<Float> *temperature_retention_rate_edit = new ValueEdit<Float>("%g", Util::TextToFloat, sub_layout);
    temperature_retention_rate_edit->SetValue(g_temperature_retention_rate);
    SignalHandler::Connect1(
        temperature_retention_rate_edit->SenderValueUpdated(),
        &m_internal_receiver_set_temperature_retention_rate);

//     SetMainWidget(m_grid_layout);
    SetMainWidget(main_layout);

    // make sure DISTRIBUTION_FUNCTION_WIDTH and DISTRIBUTION_FUNCTION_HEIGHT are odd
    ASSERT0(DISTRIBUTION_FUNCTION_WIDTH % 2 == 1)
    ASSERT0(DISTRIBUTION_FUNCTION_HEIGHT % 2 == 1)

    ASSERT0(ms_distribution_function[DISTRIBUTION_FUNCTION_HEIGHT/2][DISTRIBUTION_FUNCTION_WIDTH/2] == 0.0f)
    m_distribution_normalization = 0.0f;
    for (Uint32 y = 0; y < DISTRIBUTION_FUNCTION_HEIGHT; ++y)
        for (Uint32 x = 0; x < DISTRIBUTION_FUNCTION_WIDTH; ++x)
            m_distribution_normalization += ms_distribution_function[y][x];

    fprintf(stderr, "FireGrid() - finished creating FireButton widgets\n");
}

void FireGrid::ProcessFrameOverride ()
{
    Widget::ProcessFrameOverride();

    for (Sint32 gy = 0; gy < GRID_HEIGHT; ++gy)
    {
        for (Sint32 gx = 0; gx < GRID_WIDTH; ++gx)
        {
            Float temperature = 0.0f;
            for (Sint32 dy = -DISTRIBUTION_FUNCTION_HEIGHT/2; dy <= DISTRIBUTION_FUNCTION_HEIGHT/2; ++dy)
            {
                for (Sint32 dx = -DISTRIBUTION_FUNCTION_WIDTH/2; dx <= DISTRIBUTION_FUNCTION_WIDTH/2; ++dx)
                {
                    if (gy+dy >= 0 && gy+dy < GRID_HEIGHT && gx+dx >= 0 && gx+dx < GRID_WIDTH)
                    {
                        ASSERT1(dy+DISTRIBUTION_FUNCTION_HEIGHT/2 >= 0)
                        ASSERT1(dy+DISTRIBUTION_FUNCTION_HEIGHT/2 < DISTRIBUTION_FUNCTION_HEIGHT)
                        ASSERT1(dx+DISTRIBUTION_FUNCTION_WIDTH/2 >= 0)
                        ASSERT1(dx+DISTRIBUTION_FUNCTION_WIDTH/2 < DISTRIBUTION_FUNCTION_WIDTH)
                        temperature +=
                            ms_distribution_function[dy+DISTRIBUTION_FUNCTION_HEIGHT/2][dx+DISTRIBUTION_FUNCTION_WIDTH/2] /
                            m_distribution_normalization *
                            m_button_grid[gy+dy][gx+dx]->GetTemperature();
                    }
                }
            }
            m_button_grid[gy][gx]->SetAmbientTemperature(temperature);
        }
    }
}

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
    Screen *screen = Screen::Create(800, 1000, 32, 0);
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
        // Create a Layout widget to contain everything within the Screen.
        Layout *main_layout = new Layout(VERTICAL, screen, "main layout");
        // Cause @c main_layout to always fill the Screen.
        screen->SetMainWidget(main_layout);

        // TODO: create app-specific widgets and signal connections
        new FireGrid(main_layout);

        // TODO: add a button bar at the bottom with a quit button

        // Run the game loop until the Screen no longer has the will to live.
        while (!screen->GetIsQuitRequested())
        {
            // Limit the framerate to 1000/33 frames per second.
            ASSERT1(g_framerate > 0.0f)
            Uint32 delay = static_cast<Uint32>(1000.0f / g_framerate);
            if (delay > 0)
                SDL_Delay(delay);
            // Retrieve the current time in seconds as a Float.
            Float time = 0.001f * SDL_GetTicks();
            // Process SDL events until there are no more.
            SDL_Event sdl_event;
            while (SDL_PollEvent(&sdl_event))
            {
                // Repackage SDL_Event into Xrb::Event subclasses.
                Event *event = Event::CreateEventFromSDLEvent(&sdl_event, screen, time);
                // If it was a dud, skip this event-handling loop.
                if (event == NULL)
                    continue;
                // Let the Input singleton "have a go" at keyboard/mouse events.
                if (event->GetIsKeyEvent() || event->GetIsMouseButtonEvent())
                    Singletons::Input().ProcessEvent(event);
                // Hand the event to the top of the GUI hierarchy for processing.
                screen->ProcessEvent(event);
                // Must delete the event ourselves here.
                Delete(event);
            }

            // Turn the crank on the EventQueue.
            screen->GetOwnerEventQueue()->ProcessFrame(time);
            // Perform all off-screen GUI hierarchy processing.
            screen->ProcessFrame(time);
            // Turn the crank on the EventQueue again.
            screen->GetOwnerEventQueue()->ProcessFrame(time);
            // Draw the whole mu'erfucking thing.
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
    <li>fuck with SetIsUsingZeroedFrameMargins and SetIsUsingZeroedLayoutSpacingMargins</li>
    <li>make it so the right mouse button decreases the temperature</li>
    <li>change the color gradient to some non-linear thing</li>
    <li>change FireGrid::m_button_grid to be a 1-dimensional array, and perform the row-major array indexing yourself.  m_button_grid is currently indexed with the Y component first to make this switch easier.</li>
    <li>along the bottom row of the grid, simulate fluctuating embers via random temperatures</li>
    <li>make the temperature dissipation wrap top/bottom and left/right on the grid</li>
    <li>use different dissipation functions</li>
    <li>add a reset button next to the quit button</li>
    <li>add a pause button next to the previously added reset button</li>
    <li>extra credit: make the temperature dissipation independent of the grid dimensions -- the dissipation should appear the same size on the screen for any grid dimensions.</li>
    </ul>

Thus concludes the fourth lesson.  Somehow you're actually dumber now from it.
*/
