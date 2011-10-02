// ///////////////////////////////////////////////////////////////////////////
// lesson04_main.cpp by Victor Dods, created 2006/08/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////


// ///////////////////////////////////////////////////////////////////////////
// Lesson 04 - Creating A Game World With Static Objects
// ///////////////////////////////////////////////////////////////////////////


/** @page lesson04 Lesson 04 - Creating A Game World With Static Objects
@code *//* @endcode
This lesson will show you how to create and populate the world in which games
are set.  This lesson will only introduce static (as in unmoving) game objects
and the structure thereof, as well as the structure of the classes which make
up the game world.

    <ul>
    <li>@ref lesson04_main.cpp "This lesson's source code"</li>
    <li>@ref lessons "Main lesson index"</li>
    </ul>

This game engine provides a set of classes to facilitate creation of
interactive 2D game worlds.  The intention is to provide only common
functionality in these base classes (which will grow to include
semi-game-specific functionality if/when it proves to be common to most game
implementations), and leave game-specific details up to each game's
implementation. The important classes are:

    <ul>
    <li>@ref Xrb::Engine2::World is what contains everything and is the main
        point of control for non-rendering game logic.  Games may subclass
        it to provide game flow logic such as enemy spawning or victory
        condition detection.  World contains multiple instances of
        ObjectLayer, all instances of Entity, and optionally an instance of
        PhysicsHandler.</li>
    <li>@ref Xrb::Engine2::ObjectLayer contains all instances of Object.
        Depending on how the game world is rendered, ObjectLayer can be used
        to represent different depths in a view-parallaxed scene (i.e. foreground,
        midground and background layers).</li>
    <li>@ref Xrb::Engine2::Object is the baseclass for the physical, tangible
        game objects -- Sprite and Compound.  Itself, an Object can't move;
        see Entity.  An Object not paired with an Entity is referred to as a
        "static object" because of its lack of movement/interaction.  An Object
        has physical properties such as position, scale, and angle.</li>
    <li>@ref Xrb::Engine2::Sprite is a subclass of Object and implements
        rectangular, single-texture game objects.</li>
    <li>@ref Xrb::Engine2::Compound is a subclass of Object and implements
        conglomerates of arbitrary textured polygons.  Compound will be covered
        in later lessons.</li>
    <li>@ref Xrb::Engine2::Entity is the baseclass for game-specific interactive
        objects.  An Entity is intangible by itself, and must be attached to
        an Object.  An Object with an attached Entity is referred to as a
        "dynamic object".  Dynamic objects are specially treated by the World
        to allow them to move within the game world and interact with other
        dynamic objects.  Entity will be subclassed in each game's implementation
        to provide game-specific behavior (example subclasses could be
        SpaceShip, Missile, or even invisible things such as DamageArea).
        Entity will be covered in depth in later lessons.</li>
    <li>@ref Xrb::Engine2::PhysicsHandler is the baseclass for game-specific
        collision detection and resolution.  When a dynamic object is added to
        World, its Entity is passed to the PhysicsHandler.  The game-specific
        implementation of PhysicsHandler may then track each Entity however it
        chooses.  PhysicsHandler will be covered in detail in later lessons.</li>
    </ul>

There are two additional classes which facilitate rendering and integration
into the GUI widget hierarchy.

    <ul>
    <li>@ref Xrb::Engine2::WorldView is what actually renders the game world.
        The reason rendering is kept separate from World itself is for two
        reasons.
        <ul>
        <li>This design follows the document/view paradigm.  Therefore, a
            single document (World) may have multiple views (WorldView).</li>
        <li>This allows a separation of off-screen World processing and
            WorldView rendering which is necessary for real-time network games;
            the server performs off-screen World processing at some relatively low,
            fixed framerate, while the clients render WorldView at a relatively high
            framerate (interpolating the position/scale/angle of the game objects
            to create the illusion of arbitrarily smooth movement despite the low
            "real" framerate dictated by the server).</li>
        </ul>
        As of September 2006, independent WorldView rendering is not implemented;
        rendering must run synchronously with off-screen World processing.  Also
        as of September 2006, only top-down, parallaxed world rendering is
        implemented, though there's nothing stopping you from writing your own
        implementation.  Other implementations could include isometric (e.g.
        Starcraft).  In the future, WorldView will be made into an interface
        class, and specific implementations of it will be branched off (e.g.
        ParallaxedWorldView, IsometricWorldView, RadarView etc).</li>
    <li>@ref Xrb::Engine2::WorldViewWidget is a subclass of Widget which contains
        a single instance of WorldView.  In @ref lesson01 "an earlier lesson"
        I mentioned that everything visible in the game engine happens via use
        of a Widget or subclass thereof.  This particular Widget is what ties
        the world rendering into the GUI hierarchy.  It's very simple and
        straightforward, so you should probably never need to subclass or change
        it.</li>
    </ul>

<!-- TODO: graphical representation of Engine2 design -->

In this lesson we will create a simple game world consisting of a random mess of
sprites, and a customized WorldView subclass in which to implement view zooming,
rotation and movement.

<strong>Procedural Overview</strong> -- Items in bold are additions/changes to the previous lesson.

    <ul>
    <li>Global declarations</li>
        <ul>
        <li><strong>Declare subclass of Engine2::WorldView specific to this app.</strong></li>
        <li><strong>CreateAndPopulateWorld function which will dynamically
            allocate the World and Object instances which inhabit said
            World.</strong></li>
        </ul>
    <li>Main function</li>
        <ul>
        <li>Initialize the Pal and game engine singletons.  Create the Screen object.</li>
        <li>Execute game-specific code.</li>
            <ul>
            <li>Create application-specific objects and GUI elements, and make necessary signals.</li>
                <ul>
                <li><strong>Create the game world via CreateAndPopulateWorld.</strong></li>
                <li><strong>Create the WorldViewWidget and set it as screen's main widget.</strong></li>
                <li><strong>Create the game-specific WorldView.</strong></li>
                <li><strong>Attach the WorldView to the WorldViewWidget.</strong></li>
                <li><strong>Attach the WorldView to the World.</strong></li>
                </ul>
            <li>Run the game loop</li>
                <ul>
                <li>Calculate the Singleton::Pal().Sleep duration necessary to achieve the desired framerate.</li>
                <li>Handle events (user and system-generated).</li>
                <li>Perform off-screen processing, <strong>including game world processing.</strong></li>
                <li>Draw the Screen object's entire widget hierarchy.</li>
                </ul>
            <li><strong>Destroy application-specific objects.</strong></li>
                <ul>
                <li><strong>Destroy WorldViewWidget object, which will destroy WorldView object.</strong></li>
                <li><strong>Destroy World object, which will destroy all its ObjectLayers and Objects.</strong></li>
                </ul>
            </ul>
        <li>Delete the Screen object.  Shutdown the Pal and game engine singletons.</li>
        </ul>
    </ul>

Comments explaining previously covered material will be made more terse or
deleted entirely in each successive lesson.  If something is not explained
well enough, it was probably already explained in
@ref lessons "previous lessons".

<strong>Code Diving!</strong>

@code */
#include "xrb.hpp"                         // Must be included in every source/header file.

#include "xrb_engine2_objectlayer.hpp"     // For use of the Engine2::ObjectLayer class.
#include "xrb_engine2_sprite.hpp"          // For use of the Engine2::Sprite class.
#include "xrb_engine2_world.hpp"           // For use of the Engine2::World class.
#include "xrb_engine2_worldview.hpp"       // For use of the Engine2::WorldView class.
#include "xrb_engine2_worldviewwidget.hpp" // For use of the Engine2::WorldViewWidget class.
#include "xrb_event.hpp"                   // For use of the Event classes.
#include "xrb_eventqueue.hpp"              // For use of the EventQueue class.
#include "xrb_inputstate.hpp"              // For use of the InputState class (via Singleton::).
#include "xrb_input_events.hpp"            // For use of the EventMouseWheel class.
#include "xrb_math.hpp"                    // For use of the functions in the Math namespace.
#include "xrb_screen.hpp"                  // For use of the necessary Screen widget class.
#include "xrb_sdlpal.hpp"                  // For use of the SDLPal platform abstraction layer.
#include "xrb_widgetcontext.hpp"           // For use of the WidgetContext class.

using namespace Xrb;                       // To avoid having to use Xrb:: everywhere.

/* @endcode
Our customized WorldView class will implement view zooming, rotation and
movement.  The view will zoom in and out by mouse-wheeling-up and
mouse-wheeling-down respectively.  The view will rotate clockwise and
counterclockwise by holding an ALT key and mouse-wheeling-up and
mouse-wheeling-down respectively.  View movement will be done by holding
the left mouse button and dragging.

In order to process these mouse events, we will need to override a couple
of methods in WorldView -- Xrb::Engine2::WorldView::ProcessMouseWheelEvent
and Xrb::Engine2::WorldView::ProcessMouseMotionEvent.  These methods do
exactly what you think they do.
@code */
class AwesomeWorldView : public Engine2::WorldView
{
public:

    // Trivial constructor which is just a frontend for WorldView's constructor.
    AwesomeWorldView (Engine2::WorldViewWidget *parent_world_view_widget)
        :
        Engine2::WorldView(parent_world_view_widget)
    { }

    // This method is called with all mouse wheel events received by this
    // WorldView object.  These aren't inherited from Widget (as WorldView
    // does not inherit Widget), but are called by their counterparts in
    // WorldViewWidget.
    virtual bool ProcessMouseWheelEvent (EventMouseWheel const &e)
    {
        /* @endcode
        Note that the accessor for ALT key state is on the event, and not
        on the @ref Xrb::Singleton::InputState "Xrb::InputState singleton".  This is
        because since events can be handled asynchronously, they must retain
        the key modifier states (e.g. ALT, CTRL) themselves.
        @code */
        // If either ALT key is pressed, we will rotate the view depending
        // on which of mouse-wheel-up or mouse-wheel-down this event indicates.
        if (e.IsEitherAltKeyPressed())
        {
            if (e.ButtonCode() == Key::MOUSEWHEELUP)
                RotateView(-15.0f); // Rotate 15 degrees clockwise.
            else
            {
                ASSERT1(e.ButtonCode() == Key::MOUSEWHEELDOWN);
                RotateView(15.0f); // Rotate 15 degrees counterclockwise.
            }
        }
        // Otherwise, we will zoom the view depending on which of
        // mouse-wheel-up or mouse-wheel-down this event indicates.
        else
        {
            if (e.ButtonCode() == Key::MOUSEWHEELUP)
                ZoomView(1.2f); // Zoom in by a factor of 1.2f
            else
            {
                ASSERT1(e.ButtonCode() == Key::MOUSEWHEELDOWN);
                ZoomView(1.0f / 1.2f); // Zoom out by a factor of 1.2f
            }
        }
        // Indicates that the event was used by this method.
        return true;
    }
    // This method is the mouse motion analog of ProcessMouseWheelEvent.
    virtual bool ProcessMouseMotionEvent (EventMouseMotion const &e)
    {
        // Only do stuff if the left mouse button was pressed for this event.
        if (e.IsLeftMouseButtonPressed())
        {
            // This transforms the screen-coordinate movement delta of the
            // mouse motion event into world-coordinates.
            FloatVector2 position_delta(
                ParallaxedScreenToWorld() * e.Delta().StaticCast<Float>() -
                ParallaxedScreenToWorld() * FloatVector2::ms_zero);
            // Move the view using the calculated world-coordinate delta.  We
            // negate the delta because by dragging the view down, the view
            // should move up; while dragging, the mouse cursor should always
            // stay on the same spot relative to the game world.
            MoveView(-position_delta);
            // Indicates that the event was used by this method.
            return true;
        }
        else
            // Event not used.
            return false;
    }
}; // end of class AwesomeWorldView

/* @endcode
This is a helper function which will create our World instance and populate it
with Sprites.  The return value is the created World object.
@code */
Engine2::World *CreateAndPopulateWorld ()
{
    // Create the world via the static method Engine2::World::Create.
    // The first parameter is a pointer to a PhysicsHandler object, however
    // we will not implement a PhysicsHandler in this lesson.
    Engine2::World *world = Engine2::World::CreateEmpty(NULL);
    // Decide some size for the ObjectLayer (arbitrary).
    static Float const s_object_layer_side_length = 1000.0f;
    /* @endcode
    Using the static method @ref Xrb::Engine2::ObjectLayer::Create we will
    create the ObjectLayer in which we'll add all the Sprites.  The parameters
    are:
        <ul>
        <li>The World to which it belongs.<li>
        <li>A boolean value indicating wether or not positional wrapping is
            enabled.  A wrapped ObjectLayer will repeat itself as if its
            opposing edges wrapped around and attached to themselves.
            Topologically speaking, a wrapped ObjectLayer maps to the
            surface of a 3 dimensional torus.</li>
        <li>The side length of the ObjectLayer's square domain.</li>
        <li>Depth of the QuadTree used to store the subordinate Objects.
            Don't worry about this one yet, it will be explained further in
            later lessons.</li>
        <li>The Z depth of the ObjectLayer.  The meaning of this value is
            specific to the WorldView which is doing the rendering.  In the
            case of parallaxed WorldViews, the ObjectLayers are stacked on
            top of one another, with their depths given by this value.  The
            WorldView will use this value to calculate how large on-screen to
            render the Objects in each ObjectLayer.</li>
        </ul>
    We will then add the created ObjectLayer to the World object and set its
    main ObjectLayer (the meaning of this will be explained in later lessons).
    @code */
    Engine2::ObjectLayer *object_layer =
        Engine2::ObjectLayer::Create(
            world,                      // owner world
            false,                      // not wrapped
            s_object_layer_side_length, // side length
            6,                          // visibility quad tree depth
            0.0f);                      // z depth
    world->AddObjectLayer(object_layer);
    world->SetMainObjectLayer(object_layer);

    static Uint32 const s_object_count = 100;
    // Create a random mess of objects
    for (Uint32 i = 0; i < s_object_count; ++i)
    {
        // Create the sprite using the texture with given path
        Engine2::Sprite *sprite = Engine2::Sprite::Create("fs://interloper_2.png");
        // Place the sprite randomly on the 1000x1000 ObjectLayer.  The
        // ObjectLayer is centered on the origin, so the valid range of
        // coordinates are [-500,500] for both X and Y.
        sprite->SetTranslation(
            FloatVector2(Math::RandomFloat(-500.0f, 500.0f), Math::RandomFloat(-500.0f, 500.0f)));
        // Size the sprite between 1% and 10% (arbitrary) of the ObjectLayer
        sprite->SetScaleFactor(s_object_layer_side_length * Math::RandomFloat(0.01f, 0.1f));
        // Set the angle of the sprite randomly
        sprite->SetAngle(Math::RandomFloat(0.0f, 360.0f));
        // Add the sprite to the object layer
        world->AddStaticObject(sprite, object_layer);
    }
    // Return the created World object.
    return world;
}

void CleanUp ()
{
    std::cerr << "CleanUp();" << std::endl;
    // Shutdown the Pal and singletons.
    Singleton::Pal().Shutdown();
    Singleton::Shutdown();
}

int main (int argc, char **argv)
{
    std::cerr << "main();" << std::endl;

    // Initialize engine singletons.
    Singleton::Initialize(SDLPal::Create);
    // Initialize the Pal.
    if (Singleton::Pal().Initialize() != Pal::SUCCESS)
        return 1;
    // Set the window caption.
    Singleton::Pal().SetWindowCaption("XuqRijBuh Lesson 04");
    // Create Screen object and initialize given video mode.
    Screen *screen = Screen::Create(800, 600, 32, false);
    // If the Screen failed to initialize, print an error message and quit.
    if (screen == NULL)
    {
        std::cerr << "unable to initialize video mode" << std::endl;
        CleanUp();
        return 2;
    }
    // Create and use a default-valued StyleSheet.  The WidgetContext takes
    // ownership of the StyleSheet, so we don't need to worry about deleting it.
    StyleSheet *style_sheet = new StyleSheet();
    style_sheet->PopulateUsingDefaults();
    screen->Context().SetStyleSheet(style_sheet);

    // Here is where the application-specific code begins.
    {
        /* @endcode
        Our application specific code consists of creating the game world and
        the two classes necessary to draw the game world via the GUI hierarchy.
        @code */
        // Create our sweet game world via a call to CreateAndPopulateWorld.
        Engine2::World *world = CreateAndPopulateWorld();
        // Create the WorldViewWidget as a child of screen.  This is what will
        // contain an instance of WorldView and will cause it to be rendered.
        Engine2::WorldViewWidget *world_view_widget = new Engine2::WorldViewWidget(screen->Context());
        screen->AttachChild(world_view_widget);
        screen->SetMainWidget(world_view_widget);
        // Create an instance of our AwesomeWorldView, using the newly created
        // WorldViewWidget as its "parent".  Set the zoom factor so something
        // reasonable (though arbitrary).
        AwesomeWorldView *world_view = new AwesomeWorldView(world_view_widget);
        world_view->SetZoomFactor(0.004f);
        // Attach the newly created WorldView to the World object.
        world->AttachWorldView(world_view);
        /* @endcode
        Below, the game loop remains unchanged relative to the
        @ref lesson03 "previous lesson" except for a call to
        <tt>world->ProcessFrame(current_real_time)</tt> which handles all off-screen
        game computation.
        @code */
        // These values will be used below in the framerate control code.
        Time current_real_time = Time::ms_beginning_of;
        Time next_real_time = Time::ms_beginning_of;
        Float desired_framerate = 30.0f;
        // Run the game loop until the Screen no longer has the will to live.
        while (!screen->IsQuitRequested())
        {
            // Get the current real time and figure out how long to sleep, then sleep.
            current_real_time = Singleton::Pal().CurrentTime();
            Time::Delta seconds_to_sleep = Max(0.0f, next_real_time - current_real_time);
            Singleton::Pal().Sleep(seconds_to_sleep);
            // Calculate the desired next game loop time
            next_real_time = Max(current_real_time, next_real_time + 1.0f / desired_framerate);

            // Process events until there are no more.
            Event *event = NULL;
            while ((event = Singleton::Pal().PollEvent(screen, current_real_time)) != NULL)
            {
                // Let the InputState singleton "have a go" at keyboard/mouse events.
                if (event->IsKeyEvent() || event->IsMouseButtonEvent())
                    Singleton::InputState().ProcessEvent(*event);
                // Give the GUI hierarchy a chance at the event and then delete it.
                screen->ProcessEvent(*event);
                Delete(event);
            }

            /* @endcode
            This call is what performs all off-screen game processing, mainly by
            World, and then in later lessons by PhysicsHandler and Entity subclasses.
            This is where all game logic and physics processing will happen.
            @code */
            world->ProcessFrame(current_real_time);

            // Turn the EventQueue crank, Perform off-screen GUI processing,
            // turn the EventQueue crank again, and then draw everything.
            screen->OwnerEventQueue()->ProcessFrame(current_real_time);
            screen->ProcessFrame(current_real_time);
            screen->OwnerEventQueue()->ProcessFrame(current_real_time);
            screen->Draw(current_real_time);
        }

        /* @endcode
        Although screen would automatically delete world_view_widget if left
        around, we must delete it ourselves in order to ensure proper deletion
        order relative to World.  Deletion of WorldViewWidget will cause the
        deletion of the attached WorldView which will in turn automatically
        detach itself from World.  Having no attached WorldViews is a necessary
        precondition for the destruction of World.

        Note that it is critical to detach a widget before deleting it!
        @code */
        world_view_widget->DetachFromParent();
        Delete(world_view_widget);
        Delete(world);
    }

    // Delete the Screen (and GUI hierarchy), "SHUT IT DOWN", and return success.
    Delete(screen);
    CleanUp();
    return 0;
}
/* @endcode

<strong>Exercises</strong>

    <ul>
    <li>Add code to AwesomeWorldView to smooth the zooming.  You'll need
        to implement <tt>virtual void AwesomeWorldView::HandleFrame()</tt>
        for this.  You can use the @ref Xrb::FrameHandler::FrameTime
        method to retrieve the current time from inside HandleFrame.</li>
    <li>Add code to AwesomeWorldView to smooth the rotation.  You'll need
        to implement <tt>virtual void AwesomeWorldView::HandleFrame()</tt>
        for this.  You can use the @ref Xrb::FrameHandler::FrameTime
        method to retrieve the current time from inside HandleFrame.</li>
    <li>Change the zooming code so that the zooming in 4 times in a row
        will result in an overall zoom-in of a factor of 2.  Hint: the
        factor to zoom by should equal 2 when raised to the 4th power.  You
        will need to use the @ref Xrb::Math::Pow function.</li>
    <li>Screw with the <tt>sprite->SetTranslation</tt> call in
        CreateAndPopulateWorld and set the sprite's translation to somewhere
        outside the 1000x1000 grid of the ObjectLayer and see what happens.</li>
    <li>In CreateAndPopulateWorld, change the second parameter of
        Engine2::ObjectLayer::Create to true and see what happens.</li>
    <li>Screw with the <tt>sprite->SetTranslation</tt> call again and see what
        happens when you place a Sprite outside the domain of the ObjectLayer
        this time.</li>
    <li>In AwesomeWorldView::ProcessMouseMotionEvent, remove the negative
        sign from in front of position_delta in the call to MoveView, and
        see what the effects are.  This new behavior might be preferable
        to some.</li>
    <li>Change CreateAndPopulateWorld so the sprites are not created with
        random positions and sizes, but form a spiral instead.  Make each
        sprite's scale factor proportional to its distance from the origin,
        and set its angle such that it's facing away from the origin.</li>
    <li>Instead of creating a WorldViewWidget as the child of screen, make
        a grid Layout (e.g. row major with major count 2) and create several
        pairs of WorldViewWidget and WorldView, so that you have a 2x2 grid
        showing 4 total independent WorldViews.  Remember to delete them
        at the end of the app-specific code.  You can do this conveniently
        by deleting the Layout containing them.</li>
    <li>Disable the mouse-based view zooming/rotating/moving and implement
        <tt>virtual void AwesomeWorldView::HandleFrame()</tt> to:
            <ul>
            <li>Zoom in and out smoothly using the equation
                <tt>zoom factor = 0.008 * sin(k*time) + 0.01</tt> where k is
                an arbitrary constant you can pick (I suggest k = 90).  You'll need
                to use the @ref Xrb::Engine2::WorldView::SetZoomFactor
                method.</li>
            <li>Rotate counter/clockwise smoothly using the equation
                <tt>angle = 400 * cos(k*time)</tt> where k is an arbitrary
                constant you can pick (I suggest k = 90).  You'll need to use
                the @ref Xrb::Engine2::WorldView::SetAngle method.</li>
            <li>Move the view in a circle using the vector equation
                <tt>position = 500 * (cos(k*time), sin(k*time))</tt> where k
                is an arbitrary constant you can pick (I suggest k = 90).
                You'll need to use the @ref Xrb::Engine2::WorldView::SetCenter
                method.</li>
            </ul>
        You can use the @ref Xrb::FrameHandler::FrameTime method to retrieve
        the current time from inside HandleFrame.  Mess around with the equations
        and then show your kid brother how much cooler you are than him.</li>
    </ul>

Thus concludes lesson04, you crazy almost-game-programming bastard, you.
*/
