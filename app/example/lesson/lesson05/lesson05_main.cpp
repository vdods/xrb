// ///////////////////////////////////////////////////////////////////////////
// lesson05_main.cpp by Victor Dods, created 2006/08/06
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////


// ///////////////////////////////////////////////////////////////////////////
// Lesson 05 - Implementing Entity And World For Fun And Profit
// ///////////////////////////////////////////////////////////////////////////


/** @page lesson05 Lesson 05 - Implementing Entity And World For Fun And Profit
@code *//* @endcode
This lesson will show you how to make interactive game objects (aka dynamic
objects).  This will be done by subclassing @ref Xrb::Engine2::Entity and
providing a game-specific implementation.  We will also have to subclass
@ref Xrb::Engine2::World to provide the code to control its subordinate
Entity objects.

    <ul>
    <li>@ref lesson05_main.cpp "This lesson's source code"</li>
    <li>@ref lessons "Main lesson index"</li>
    </ul>

TODO: detailed description

<strong>Procedural Overview</strong> -- Items in bold are additions/changes to the previous lesson.

    <ul>
    <li>Global declarations</li>
        <ul>
        <li><strong>Declare subclass of Engine2::Entity specific to this app.</strong></li>
        <li><strong>Declare subclass of Engine2::World specific to this app.</strong></li>
        <li>Declare subclass of Engine2::WorldView specific to this app.</li>
        <li>CreateAndPopulateWorld function which will dynamically allocate
            the World and Object instances which inhabit said World.</li>
        </ul>
    <li>Main function</li>
        <ul>
        <li>Initialize SDL, engine singletons and create the Screen object</li>
        <li>Execute game-specific code.</li>
            <ul>
            <li>Create application-specific objects and GUI elements, and make necessary signals.</li>
                <ul>
                <li>Create the game world via CreateAndPopulateWorld.</li>
                <li>Create the WorldViewWidget and set it as screen's main widget.</li>
                <li>Create the game-specific WorldView.</li>
                <li>Attach the WorldView to the WorldViewWidget.</li>
                <li>Attach the WorldView to the World.</li>
                </ul>
            <li>Run the game loop</li>
                <ul>
                <li>Calculate the SDL_Delay duration necessary to achieve the desired framerate.</li>
                <li>Handle events (user and system-generated).</li>
                <li>Perform off-screen processing, including game world processing.</li>
                <li>Draw the Screen object's entire widget hierarchy.</li>
                </ul>
            <li>Destroy application-specific objects.</li>
                <ul>
                <li>Destroy WorldViewWidget object, which will destroy WorldView object.</li>
                <li>Destroy World object, which will destroy all its ObjectLayers, Objects <strong>and Entities.</strong></li>
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
#include "xrb.h"                         // Must be included in every source/header file.

#include "xrb_engine2_objectlayer.h"     // For use of the Engine2::ObjectLayer class.
#include "xrb_engine2_sprite.h"          // For use of the Engine2::Sprite class.
#include "xrb_engine2_world.h"           // For use of the Engine2::World class.
#include "xrb_engine2_worldview.h"       // For use of the Engine2::WorldView class.
#include "xrb_engine2_worldviewwidget.h" // For use of the Engine2::WorldViewWidget class.
#include "xrb_event.h"                   // For use of the Event classes.
#include "xrb_eventqueue.h"              // For use of the EventQueue class.
#include "xrb_input.h"                   // For use of the Input class (via Singletons::).
#include "xrb_input_events.h"            // For use of the EventMouseWheel class.
#include "xrb_math.h"                    // For use of the functions in the Math namespace.
#include "xrb_screen.h"                  // For use of the necessary Screen widget class.

using namespace Xrb;                     // To avoid having to use Xrb:: everywhere.

/* @endcode

@code */
class AwesomeEntity : public Engine2::Entity
{
public:

    AwesomeEntity ()
        :
        Engine2::Entity(),
        m_mass(1.0f),
        m_velocity(FloatVector2::ms_zero),
        m_force(FloatVector2::ms_zero)
    { }

    // Trivial accessors for the properties of AwesomeEntity.
    inline Float GetMass () const { return m_mass; }
    inline FloatVector2 const &GetVelocity () const { return m_velocity; }
    inline FloatVector2 const &GetForce () const { return m_force; }

    // Modifiers for the properties of AwesomeEntity.
    inline void SetMass (Float mass)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(mass))
        ASSERT1(mass > 0.0f)
        m_mass = mass;
    }
    inline void SetVelocity (FloatVector2 const &velocity)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(velocity[Dim::X]))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(velocity[Dim::Y]))
        m_velocity = velocity;
    }

    // Procedures which will be used by the gravity calculations in AwesomeWorld.
    void IncrementVelocity (FloatVector2 const &velocity_delta)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(velocity_delta[Dim::X]))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(velocity_delta[Dim::Y]))
        m_velocity += velocity_delta;
    }
    void IncrementForce (FloatVector2 const &force_delta)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(force_delta[Dim::X]))
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(force_delta[Dim::Y]))
        m_force += force_delta;
    }
    void ResetForce () { m_force = FloatVector2::ms_zero; }

    // These are pure virtual methods required by Entity implementations.
    // Write can be left blank for our purposes.
    virtual void Write (Serializer &serializer) const { }
    // This method is called on entities which have hit the side of the ObjectLayer.
    // For now we'll just stop the entity's motion along the indicated dimension(s).
    // This method will only be called on entities in a non-wrapped ObjectLayer.
    virtual void HandleObjectLayerContainment (bool component_x, bool component_y)
    {
        if (component_x)
            m_velocity[Dim::X] = 0.0f;
        if (component_y)
            m_velocity[Dim::Y] = 0.0f;
    }

protected:

    // These are pure virtual methods which have to be implemented in Entity
    // subclasses.  For our purposes, they can be left empty.
    virtual void HandleNewOwnerObject () { }
    virtual void CloneProperties (Entity const *entity) { }

private:

    Float m_mass;
    FloatVector2 m_velocity;
    FloatVector2 m_force;
}; // end of class AwesomeEntity

class AwesomeWorld : public Engine2::World
{
public:

    /* @endcode

    @code */
    AwesomeWorld ()
        :
        World(NULL),
        m_gravitational_constant(3.0f)
    {
        // At this point, the world is empty.

        // Decide some size for the ObjectLayer (the hardcoded scale factors
        // and translations below are loosely dependent on this value).
        static Float const s_object_layer_side_length = 2000.0f;
        // Create the ObjectLayer which will hold our game objects.
        Engine2::ObjectLayer *object_layer =
            Engine2::ObjectLayer::Create(
                this,                       // owner world
                false,                      // not wrapped
                s_object_layer_side_length, // side length
                6,                          // visibility quad tree depth
                0.0f);                      // z depth
        AddObjectLayer(object_layer);
        SetMainObjectLayer(object_layer);

        Engine2::Sprite *sprite;
        AwesomeEntity *planet;

        // create a planet
        sprite = Engine2::Sprite::Create("resources/demi3_small.png");
        planet = new AwesomeEntity();
        sprite->SetEntity(planet);
        planet->SetTranslation(FloatVector2::ms_zero);
        planet->SetScaleFactor(250.0f);
        planet->SetMass(100.0f * planet->GetScaleFactor() * planet->GetScaleFactor());
        AddDynamicObject(sprite, object_layer);

        // create a bunch of moons
        static Uint32 const s_moon_count = 50;
        for (Uint32 i = 0; i < s_moon_count; ++i)
        {
            sprite = Engine2::Sprite::Create("resources/shade3_small.png");
            AwesomeEntity *moon = new AwesomeEntity();
            sprite->SetEntity(moon);
            sprite->SetZDepth(-0.1f);
            moon->SetScaleFactor(Math::RandomFloat(10.0f, 20.0f));
            moon->SetMass(0.01f * moon->GetScaleFactor() * moon->GetScaleFactor());
            Float angle = Math::RandomFloat(0.0f, 360.0f);
            Float minimum_orbital_radius = planet->GetScaleFactor() + moon->GetScaleFactor() + 100.0f;
            Float orbital_radius = Math::RandomFloat(minimum_orbital_radius, minimum_orbital_radius + 400.0f);
            moon->SetTranslation(orbital_radius * Math::UnitVector(angle));
            ASSERT1(orbital_radius > 0.0f)
            Float gravitational_force = GetGravitationalForce(planet, moon);
            // equation for acceleration in relation to force and mass
            // a = f / m
            // v = sqrt(r * f / m)
            // use kepler's 3rd law to calculate the speed necessary to stay in circular orbit
            // a = v^2 / r
            // v^2 = r * a
            // v = sqrt(r * a)
            Float orbital_speed = Math::Sqrt(orbital_radius * gravitational_force / moon->GetMass());
            moon->SetVelocity(orbital_speed * Math::UnitVector(angle+90.0f));
            AddDynamicObject(sprite, object_layer);
        }
    }

protected:

    virtual void HandleFrame ()
    {
        Uint32 entity_capacity = GetEntityCapacity();

    /* @endcode

    @code */
        // Apply gravitational forces between each distinct pair of entities.
        for (Uint32 i = 0; i < entity_capacity; ++i)
        {
            AwesomeEntity *entity0 = dynamic_cast<AwesomeEntity *>(GetEntity(i));
            if (entity0 == NULL)
                continue;

            for (Uint32 j = i+1; j < entity_capacity; ++j)
            {
                AwesomeEntity *entity1 = dynamic_cast<AwesomeEntity *>(GetEntity(j));
                if (entity1 == NULL)
                    continue;

                ASSERT1(entity0 != entity1)

    /* @endcode

    @code */
                Float gravitational_force = GetGravitationalForce(entity0, entity1);
                ASSERT1(gravitational_force >= 0.0f)
                // If the force is zero (which can happen when the entities'
                // centers coincide and the gravitation equation would divide
                // by zero), skip this entity pair.
                if (gravitational_force == 0.0f)
                    continue;

                // The gravitational force is from entity0 to entity1
                FloatVector2 force_direction = (entity1->GetTranslation() - entity0->GetTranslation()).GetNormalization();
                // Apply equal and opposite gravitational force to both entities.
                entity0->IncrementForce( gravitational_force * force_direction);
                entity1->IncrementForce(-gravitational_force * force_direction);
            }
        }

    /* @endcode

    @code */
        // Update the velocity vector of each entity with the accumulated force
        // and update the position vector using the newly calculated velocity.
        for (Uint32 i = 0; i < entity_capacity; ++i)
        {
            AwesomeEntity *entity = dynamic_cast<AwesomeEntity *>(GetEntity(i));
            if (entity == NULL)
                continue;

            ASSERT1(entity->GetMass() > 0.0f)
            entity->IncrementVelocity(entity->GetForce() / entity->GetMass() * GetFrameDT());
            entity->ResetForce();
            entity->Translate(entity->GetVelocity() * GetFrameDT());
        }
    }

private:

    /* @endcode

    @code */
    Float GetGravitationalForce (AwesomeEntity *entity0, AwesomeEntity *entity1) const
    {
        ASSERT1(entity0 != NULL && entity1 != NULL)
        FloatVector2 entity_offset(entity1->GetTranslation() - entity0->GetTranslation());
        Float distance = entity_offset.GetLength();
        // If they're touching, don't apply gravitational force (this
        // is to avoid a divide by zero if their positions coincide).
        if (distance < entity0->GetScaleFactor() + entity1->GetScaleFactor())
            return 0.0f;
        else
            return
                m_gravitational_constant *
                entity0->GetMass() * entity1->GetMass() /
                (distance * distance);
    }

    Float m_gravitational_constant;
}; // end of class AwesomeWorld

/* @endcode
Here is our totally awesome customized WorldView which is the same as the one
explained in the @ref lesson04 "previous lesson".
@code */
class AwesomeWorldView : public Engine2::WorldView
{
public:

    // Trivial constructor which is just a frontend for WorldView's constructor.
    AwesomeWorldView (Engine2::WorldViewWidget *parent_world_view_widget)
        :
        Engine2::WorldView(parent_world_view_widget)
    { }

    // Called by WorldViewWidget with all mouse wheel events for this WorldView
    virtual bool ProcessMouseWheelEvent (EventMouseWheel const *e)
    {
        // Rotate the view on ALT+mouse-wheel-up/down.
        if (e->GetIsEitherAltKeyPressed())
            RotateView((e->GetButtonCode() == Key::MOUSEWHEELUP) ? -15.0f : 15.0f);
        // Otherwise, we will zoom the view on mouse-wheel-up/down.
        else
            ZoomView((e->GetButtonCode() == Key::MOUSEWHEELUP) ? 1.2f : 1.0f / 1.2f);
        // Indicates that the event was used by this method.
        return true;
    }
    // This method is the mouse motion analog of ProcessMouseWheelEvent.
    virtual bool ProcessMouseMotionEvent (EventMouseMotion const *e)
    {
        // Only do stuff if the left mouse button was pressed for this event.
        if (e->GetIsLeftMouseButtonPressed())
        {
            // Move the view by a delta which is calculated by transforming
            // the screen coordinates of the event to world coordinates as used
            // by WorldView.
            MoveView(
                GetParallaxedScreenToWorld() * FloatVector2::ms_zero -
                GetParallaxedScreenToWorld() * e->GetDelta().StaticCast<Float>());
            // Indicates that the event was used by this method.
            return true;
        }
        else
            // Event not used.
            return false;
    }
}; // end of class AwesomeWorldView

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
    SDL_WM_SetCaption("XuqRijBuh Lesson 05", "");
    Screen *screen = Screen::Create(800, 600, 32, 0);
    // If the Screen failed to initialize, print an error message and quit.
    if (screen == NULL)
    {
        fprintf(stderr, "unable to initialize video mode\n");
        CleanUp();
        return 2;
    }

    // Here is where the application-specific code begins.
    {
        // Create our sweet game world via a call to CreateAndPopulateWorld.
        AwesomeWorld *world = new AwesomeWorld();
        // Create the WorldViewWidget as a child of screen.  This is what will
        // contain an instance of WorldView and will cause it to be rendered.
        Engine2::WorldViewWidget *world_view_widget = new Engine2::WorldViewWidget(screen);
        screen->SetMainWidget(world_view_widget);
        // Create an instance of our AwesomeWorldView, using the newly created
        // WorldViewWidget as its "parent".  Set the zoom factor so something
        // reasonable (though arbitrary).
        AwesomeWorldView *world_view = new AwesomeWorldView(world_view_widget);
        world_view->SetZoomFactor(0.002f);
        // Attach the newly created WorldView to the World object.
        world->AttachWorldView(world_view);
        /* @endcode
        Below, the game loop remains unchanged relative to the @ref lesson04 "previous lesson".
        @code */
        // These values will be used below in the framerate control code.
        Float current_real_time = 0.0f;
        Float next_real_time = 0.0f;
        Float desired_framerate = 60.0f;
        // Run the game loop until the Screen no longer has the will to live.
        while (!screen->GetIsQuitRequested())
        {
            // Get the current real time and figure out how long to sleep, then sleep.
            current_real_time = 0.001f * SDL_GetTicks();
            Sint32 milliseconds_to_sleep = Max(0, static_cast<Sint32>(1000.0f * (next_real_time - current_real_time)));
            SDL_Delay(milliseconds_to_sleep);
            // Calculate the desired next game loop time
            next_real_time = Max(current_real_time, next_real_time + 1.0f / desired_framerate);

            // Process SDL events until there are no more.
            SDL_Event sdl_event;
            while (SDL_PollEvent(&sdl_event))
            {
                // Repackage SDL_Event into Xrb::Event subclasses, skipping NULLs
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

            // Perform all off-screen game processing.
            world->ProcessFrame(current_real_time);

            // Turn the EventQueue crank, Perform off-screen GUI processing,
            // turn the EventQueue crank again, and then draw everything.
            screen->GetOwnerEventQueue()->ProcessFrame(current_real_time);
            screen->ProcessFrame(current_real_time);
            screen->GetOwnerEventQueue()->ProcessFrame(current_real_time);
            screen->Draw();
        }

        // Delete world_view_widget and world, in that order.  This will
        // automatically delete our AwesomeWorldView instance.
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
    <li>In the AwesomeWorld constructor, double the orbital speed for each
        moon so they fly out of orbit and smash into the side of the
        ObjectLayer and see what happens.</li>
    <li>Make entities which hit the sides of the ObjectLayer bounce back by
        changing AwesomeEntity::HandleObjectLayerContainment and see what
        happens.  Change it again so the entities bounce back inelastically
        (losing velocity after each bounce).</li>
    <li>Reverse the direction the moon entities orbit the planet.</li>
    <li>Play with the value of m_gravitational_constant and see what the
        effects are (taking note of where it is used in computations).</li>
    <li>Change the AwesomeWorld constructor so  all the moon entities spawn
        only on either the X or Y axes so they are initially arranged in a
        cross pattern, and see what happens.</li>
    <li>Change the gravitational system to be a binary planet system (i.e.
        two heavy planets orbiting one another, with many lighter moons
        orbiting at a safe distance.</li>
    <li>In the AwesomeWorld constructor, spawn only a single moon and a single
        planet, and make the mass of the moon and planet similar to one another.
        The net momentum of the gravitational system will not be zero, and it
        will experience a drifting effect (the system's center of gravity is
        not stationary). Correct this drifting so that the net momentum of the
        system is zero.</li>
    </ul>

Thus concludes lesson05, you crazy almost-game-programming bastard, you.
*/
