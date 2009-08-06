// ///////////////////////////////////////////////////////////////////////////
// lesson06_main.cpp by Victor Dods, created 2006/10/01
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////


// ///////////////////////////////////////////////////////////////////////////
// Lesson 06 - PhysicsHandler And Additional Object Layers
// ///////////////////////////////////////////////////////////////////////////


/** @page lesson06 Lesson 06 - PhysicsHandler And Additional Object Layers
@code *//* @endcode
This lesson will

    <ul>
    <li>@ref lesson06_main.cpp "This lesson's source code"</li>
    <li>@ref lessons "Main lesson index"</li>
    </ul>

In this lesson, we will build upon the @ref lesson05 "previous lesson" by
moving our physics code into an implementation of the pure virtual class
@ref Xrb::Engine2::PhysicsHandler.

Except for the very simplest cases (such as in this lesson), physics code gets
very complicated.  Thus, through PhysicsHandler, it is separated out of the
World class.  Another possibly more important reason for this separation is to
provide an interface class by which games can implement their own
application-specific physics behavior.  This leaves the World implementation
to handle things like game flow/logic.

We will only slightly build upon the previous lesson.  We will write our own
custom implementation of PhysicsHandler, into which we will transplant the
physics code from the previous lesson.  We will also make a simple addition
of a background ObjectLayer which will contain a starfield, to demonstrate
the parallaxing effect of WorldView.

<strong>Procedural Overview</strong> -- Items in bold are additions/changes to the previous lesson.

    <ul>
    <li>Global declarations</li>
        <ul>
        <li>Declare subclass of Engine2::Entity specific to this app.</li>
        <li><strong>Declare subclass of Engine2::PhysicsHandler specific to this app.</strong></li>
            <ul>
            <li><strong>Implement the pure virtual methods required by the
                PhysicsHandler interface class.  These are AddObjectLayer,
                SetMainObjectLayer, AddEntity and RemoveEntity.</strong></li>
            <li><strong>Override HandleFrame to do once-per-frame gravitational
                simulation calculations and to update the velocities and positions
                of the dynamic objects.</strong></li>
            </ul>
        <li>Declare subclass of Engine2::World specific to this app.</li>
            <ul>
            <li><strong>The constructor will instantiate the application-specific
                PhysicsHandler and pass it to the superclass Engine2::World
                constructor.</strong></li>
            </ul>
        <li>Declare subclass of Engine2::WorldView specific to this app.</li>
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
#include "xrb.hpp"                         // Must be included in every source/header file.

#include <set>                           // For use of the std::set template class.

#include "xrb_engine2_objectlayer.hpp"     // For use of the Engine2::ObjectLayer class.
#include "xrb_engine2_physicshandler.hpp"  // For use of the Engine2::PhysicsHandler class.
#include "xrb_engine2_sprite.hpp"          // For use of the Engine2::Sprite class.
#include "xrb_engine2_world.hpp"           // For use of the Engine2::World class.
#include "xrb_engine2_worldview.hpp"       // For use of the Engine2::WorldView class.
#include "xrb_engine2_worldviewwidget.hpp" // For use of the Engine2::WorldViewWidget class.
#include "xrb_event.hpp"                   // For use of the Event classes.
#include "xrb_eventqueue.hpp"              // For use of the EventQueue class.
#include "xrb_input.hpp"                   // For use of the Input class (via Singletons::).
#include "xrb_input_events.hpp"            // For use of the EventMouseWheel class.
#include "xrb_math.hpp"                    // For use of the functions in the Math namespace.
#include "xrb_screen.hpp"                  // For use of the necessary Screen widget class.

using namespace Xrb;                     // To avoid having to use Xrb:: everywhere.

// AwesomeEntity remains unchanged relative to the @ref lesson05 "previous lesson".
class AwesomeEntity : public Engine2::Entity
{
public:

    // The constructor simply initializes the properties to sane values.  Mass
    // must be greater than zero to avoid division by zero in some calculations.
    AwesomeEntity ()
        :
        Engine2::Entity(),
        m_mass(1.0f),
        m_velocity(FloatVector2::ms_zero),
        m_force(FloatVector2::ms_zero)
    { }

    // Trivial accessors for the properties of AwesomeEntity.
    inline Float Mass () const { return m_mass; }
    inline FloatVector2 const &GetVelocity () const { return m_velocity; }
    inline FloatVector2 const &Force () const { return m_force; }

    // Modifiers for the properties of AwesomeEntity.  The ASSERT_NAN_SANITY_CHECK
    // macro is used in various places in Engine2 code to quickly catch common
    // bugs in game code which result in NaN values being fed to the snake.
    inline void SetMass (Float mass)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(mass));
        ASSERT1(mass > 0.0f);
        m_mass = mass;
    }
    inline void SetVelocity (FloatVector2 const &velocity)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(velocity[Dim::X]));
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(velocity[Dim::Y]));
        m_velocity = velocity;
    }

    // Procedures which will be used by the gravity calculations in AwesomeWorld.
    void IncrementVelocity (FloatVector2 const &velocity_delta)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(velocity_delta[Dim::X]));
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(velocity_delta[Dim::Y]));
        m_velocity += velocity_delta;
    }
    void IncrementForce (FloatVector2 const &force_delta)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(force_delta[Dim::X]));
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(force_delta[Dim::Y]));
        m_force += force_delta;
    }
    void ResetForce () { m_force = FloatVector2::ms_zero; }

    // These are pure virtual methods which have to be implemented in Entity
    // subclasses.  Write can be left blank for our purposes.
    virtual void Write (Serializer &serializer) const { }
    // This method is called on entities which have hit the side of the
    // ObjectLayer.  For now we'll just stop the entity's motion along the
    // indicated dimension(s).  This method will only be called on entities
    // in a non-wrapped ObjectLayer.
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

/* @endcode
PhysicsHandler can be thought of as having a very simple relationship with
World.  It tracks two things: ObjectLayer and Entity.  Any time an
ObjectLayer is added to the world, it lets the PhysicsHandler know.  Any time
an Entity is added or removed from the world, it lets the PhysicsHandler know.
The only other consideration is that World will call ProcessFrame on the
PhysicsHandler, so that all physics code has a chance to run once per frame
in HandleFrame.

The only thing provided by the PhysicsHandler baseclass is a pointer to the
World which owns it.  The rest is a blank slate on which a programmer can
implement whatever retarded designs they see fit.

In this lesson, we will yank the physics code out of the previous lesson's
AwesomeWorld::HandleFrame and dump it, functionally unchanged, into
AwesomePhysicsHandler::HandleFrame.  The physics behavior as seen by
the user will not change.
@code */
class AwesomePhysicsHandler : public Engine2::PhysicsHandler
{
public:

    // The constructor initializes m_gravitational_constant.
    AwesomePhysicsHandler (Float gravitational_constant)
        :
        Engine2::PhysicsHandler(),
        m_gravitational_constant(gravitational_constant)
    { }
    // The destructor will just assert that there are no remaining entities.
    virtual ~AwesomePhysicsHandler ()
    {
        ASSERT1(m_entity_set.empty());
    }

    // Trivial accessor for m_gravitational_constant.
    inline Float GravitationalConstant () const { return m_gravitational_constant; }
    // Helper function to calculate Newton's Law Of Universal Gravitation.
    Float CalculateGravitationalForce (AwesomeEntity *entity0, AwesomeEntity *entity1) const
    {
        ASSERT1(entity0 != NULL && entity1 != NULL);
        FloatVector2 entity_offset(entity1->GetTranslation() - entity0->GetTranslation());
        Float distance = entity_offset.Length();
        // If they're touching, don't apply gravitational force (this
        // is to avoid a divide by zero if their positions coincide).
        if (distance < entity0->GetScaleFactor() + entity1->GetScaleFactor())
            return 0.0f;
        else
            return
                m_gravitational_constant *
                entity0->Mass() * entity1->Mass() /
                (distance * distance);
    }

    /* @endcode
    These two virtual methods are provided to indicate all created ObjectLayers
    to the PhysicsHandler, so that if desired in a particular implementation,
    physics calculations can be handled separately for different ObjectLayers.
    For example, in a game with ground and sky layers, the objects on the
    ground should not collide with the objects in the sky.  Determining which
    ObjectLayer an Entity belongs to is done in AddEntity and RemoveEntity
    (see below).

    In this lesson, we assume that all entities will be in the foreground
    ObjectLayer, so we will not track ObjectLayers.
    @code */
    virtual void AddObjectLayer (Engine2::ObjectLayer *object_layer) { }
    virtual void SetMainObjectLayer (Engine2::ObjectLayer *object_layer) { }

    /* @endcode
    This method is called by the World object when a dynamic object is added.
    This is necessary because because the PhysicsHandler is responsible for
    tracking entities on its own.

    In this lesson, we are keeping a std::set of all added entities.  The
    std::set template class was chosen because adding/removing elements is
    O(log(n)).
    @code */
    virtual void AddEntity (Engine2::Entity *entity)
    {
        ASSERT1(entity != NULL);
        ASSERT1(dynamic_cast<AwesomeEntity *>(entity) != NULL);
        m_entity_set.insert(static_cast<AwesomeEntity *>(entity));
    }
    /* @endcode
    The removal analog to AddEntity.  This method is provided so the
    PhysicsHandler can clean up its own entity-tracking when a dynamic
    object is removed from the World.
    @code */
    virtual void RemoveEntity (Engine2::Entity *entity)
    {
        ASSERT1(entity != NULL);
        ASSERT1(dynamic_cast<AwesomeEntity *>(entity) != NULL);
        ASSERT1(m_entity_set.find(static_cast<AwesomeEntity *>(entity)) != m_entity_set.end());
        m_entity_set.erase(static_cast<AwesomeEntity *>(entity));
    }

protected:

    /* @endcode
    The physics code that was in AwesomeWorld::HandleFrame in the
    @ref lesson05 "previous lesson" has been moved into this method, with the
    for-loop iteration changed to accomodate the std::set which stores the
    entities being tracked by this PhysicsHandler.
    @code */
    virtual void HandleFrame ()
    {
        EntitySetIterator it_end = m_entity_set.end();

        // Apply gravitational forces between each distinct pair of entities.
        for (EntitySetIterator it0 = m_entity_set.begin();
             it0 != it_end;
             ++it0)
        {
            AwesomeEntity *entity0 = *it0;
            ASSERT1(entity0 != NULL);

            for (EntitySetIterator it1 = it0;
                 it1 != it_end;
                 ++it1)
            {
                AwesomeEntity *entity1 = *it1;
                ASSERT1(entity1 != NULL);

                // Skip this calculation if the pair is an entity with itself.
                if (entity0 == entity1)
                    continue;

                // Use the helper function to calculate the gravitational force
                // between the two entities.
                Float gravitational_force = CalculateGravitationalForce(entity0, entity1);
                ASSERT1(gravitational_force >= 0.0f);
                // If the force is zero (which can happen when the entities'
                // centers coincide and the gravitation equation would divide
                // by zero), skip this entity pair.
                if (gravitational_force == 0.0f)
                    continue;

                // The gravitational force is from entity0 to entity1
                FloatVector2 force_direction = (entity1->GetTranslation() - entity0->GetTranslation()).Normalization();
                // Apply equal and opposite gravitational force to both entities.
                entity0->IncrementForce( gravitational_force * force_direction);
                entity1->IncrementForce(-gravitational_force * force_direction);
            }
        }

        // Update the velocity vector of each entity with the accumulated force
        // and update the position vector using the newly calculated velocity.
        for (EntitySetIterator it = m_entity_set.begin();
             it != it_end;
             ++it)
        {
            AwesomeEntity *entity = *it;
            ASSERT1(entity != NULL);

            ASSERT1(entity->Mass() > 0.0f);
            // Use Euler Integration to calculate the new velocity, based on
            // the accumulated force during this frame.
            entity->IncrementVelocity(entity->Force() / entity->Mass() * FrameDT());
            // Reset the accumulated force for next frame.
            entity->ResetForce();
            // Use Euler Integration again to calculate the new position,
            // based on the entity's velocity.
            entity->Translate(entity->GetVelocity() * FrameDT());
        }
    }

private:

    typedef std::set<AwesomeEntity *> EntitySet;
    typedef EntitySet::iterator EntitySetIterator;

    EntitySet m_entity_set;
    Float m_gravitational_constant;
}; // end of class AwesomePhysicsHandler

class AwesomeWorld : public Engine2::World
{
public:

    /* @endcode
    The difference in this constructor from the @ref lesson05 "previous lesson"
    is that a background ObjectLayer, populated with "stars" is created before
    the foreground ObjectLayer.  The Z depth of the layer is set to a high value
    so that the parallax effect is noticeable.  The Z depth causes the ObjectLayer
    to appear close up/far away, so changing the size of the ObjectLayer itself
    is sometimes necessary to get the effect you want.  In this case, since the
    layer is far off in the distance, its size has been set to a large value
    relative to the foreground layer.

    Also notice that the superclass constructor is being passed a new'ed
    instance of AwesomePhysicsHandler.  This is the PhysicsHandler implementation
    which will be passed all Entity instances which are added to the world
    and whose responsibility is to update their application-specific
    functionality -- not just position/velocity/etc but also any "think"
    functions which may be implemented through Entity, and other things of
    that type.
    @code */
    AwesomeWorld ()
        :
        Engine2::World(new AwesomePhysicsHandler(60.0f))
    {
        // At this point, the world is empty.

        Float object_layer_side_length;
        Engine2::ObjectLayer *object_layer;
        Engine2::Sprite *sprite;

        // Here we will create an ObjectLayer to contain the starfield
        // which will constitute the background.
        object_layer_side_length = 10000.0f;
        object_layer =
            Engine2::ObjectLayer::Create(
                this,                     // owner world
                false,                    // not wrapped
                object_layer_side_length, // side length
                6,                        // visibility quad tree depth
                1000.0f);                 // z depth
        AddObjectLayer(object_layer);

        // Add a bunch of "star" sprites.
        static Uint32 const s_star_count = 100;
        for (Uint32 i = 0; i < s_star_count; ++i)
        {
            sprite = Engine2::Sprite::Create("resources/shade0_small.png");
            sprite->SetScaleFactor(Math::RandomFloat(0.0016f, 0.0024f) * object_layer_side_length);
            sprite->SetTranslation(
                object_layer_side_length *
                FloatVector2(
                    Math::RandomFloat(-0.5f, 0.5f),
                    Math::RandomFloat(-0.5f, 0.5f)));
            AddStaticObject(sprite, object_layer);
        }

        // Create the ObjectLayer which will hold our game objects.
        object_layer_side_length = 2000.0f;
        object_layer =
            Engine2::ObjectLayer::Create(
                this,                     // owner world
                false,                    // not wrapped
                object_layer_side_length, // side length
                6,                        // visibility quad tree depth
                0.0f);                    // z depth
        AddObjectLayer(object_layer);
        SetMainObjectLayer(object_layer);

        AwesomeEntity *planet;

        // Create a large, heavy planet.
        sprite = Engine2::Sprite::Create("resources/demi3_small.png");
        planet = new AwesomeEntity();
        sprite->SetEntity(planet);
        planet->SetTranslation(FloatVector2::ms_zero);
        planet->SetScaleFactor(250.0f);
        planet->SetMass(100.0f * planet->GetScaleFactor() * planet->GetScaleFactor());
        AddDynamicObject(sprite, object_layer);

        // Create a bunch of small, light moons.
        static Uint32 const s_moon_count = 50;
        for (Uint32 i = 0; i < s_moon_count; ++i)
        {
            sprite = Engine2::Sprite::Create("resources/shade3_small.png");
            AwesomeEntity *moon = new AwesomeEntity();
            sprite->SetEntity(moon);
            sprite->SetZDepth(-0.1f);
            moon->SetScaleFactor(Math::RandomFloat(10.0f, 20.0f));
            moon->SetMass(0.01f * moon->GetScaleFactor() * moon->GetScaleFactor());
            // Pick a distance to orbit the moon at.
            Float minimum_orbital_radius = planet->GetScaleFactor() + moon->GetScaleFactor() + 100.0f;
            Float orbital_radius = Math::RandomFloat(minimum_orbital_radius, minimum_orbital_radius + 400.0f);
            ASSERT1(orbital_radius > 0.0f);
            // The moon will be placed randomly using polar coordinates.
            // We've calculated the R value, now we need theta.
            Float angle = Math::RandomFloat(0.0f, 360.0f);
            // Initialize the moon's position
            moon->SetTranslation(orbital_radius * Math::UnitVector(angle));
            // In order to figure out what speed to use to set the moon into
            // circular orbit, we need to know the magnitude of the gravitational
            // force between it and the large planet.
            Float gravitational_force = GetPhysicsHandler()->CalculateGravitationalForce(planet, moon);
            // Solve for the necessary orbital speed using Kepler's Third Law.
            Float orbital_speed = Math::Sqrt(gravitational_force * orbital_radius / moon->Mass());
            // The velocity must be perpendicular to the vector joining the
            // centers of the planet and the moon.
            moon->SetVelocity(orbital_speed * Math::UnitVector(angle+90.0f));
            // Finally add it to the world.
            AddDynamicObject(sprite, object_layer);
        }
    }

protected:

    // Trivial accessor to retrieve a pointer to the World's AwesomePhysicsHandler.
    inline AwesomePhysicsHandler *GetPhysicsHandler () { return dynamic_cast<AwesomePhysicsHandler *>(m_physics_handler); }

    /* @endcode
    Since we have moved all the physics code into the HandleFrame method of
    AwesomePhysicsHandler, we no longer need to do it here.  However, this is
    where you could put game flow/logic code (e.g. spawning ships, incrementing
    a player's score, etc).
    @code */
    virtual void HandleFrame ()
    {
        // You must always call the superclass' HandleFrame method, as it
        // performs vital processing -- specifically of the EventQueue and
        // PhysicsHandler, which will be covered in a later lesson.
        Engine2::World::HandleFrame();
    }
}; // end of class AwesomeWorld

/* @endcode
The rest of the code is identical to the previous lesson.
@code */
// AwesomeWorldView is unchanged relative to the previous lesson.
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
        if (e->IsEitherAltKeyPressed())
            RotateView((e->ButtonCode() == Key::MOUSEWHEELUP) ? -15.0f : 15.0f);
        // Otherwise, we will zoom the view on mouse-wheel-up/down.
        else
            ZoomView((e->ButtonCode() == Key::MOUSEWHEELUP) ? 1.2f : 1.0f / 1.2f);
        // Indicates that the event was used by this method.
        return true;
    }
    // This method is the mouse motion analog of ProcessMouseWheelEvent.
    virtual bool ProcessMouseMotionEvent (EventMouseMotion const *e)
    {
        // Only do stuff if the left mouse button was pressed for this event.
        if (e->IsLeftMouseButtonPressed())
        {
            // Move the view by a delta which is calculated by transforming
            // the screen coordinates of the event to world coordinates as used
            // by WorldView.
            MoveView(
                ParallaxedScreenToWorld() * FloatVector2::ms_zero -
                ParallaxedScreenToWorld() * e->Delta().StaticCast<Float>());
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
    SDL_WM_SetCaption("XuqRijBuh Lesson 06", "");
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

        // These values will be used below in the framerate control code.
        Float current_real_time = 0.0f;
        Float next_real_time = 0.0f;
        Float desired_framerate = 60.0f;
        // Run the game loop until the Screen no longer has the will to live.
        while (!screen->IsQuitRequested())
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
                if (event->IsKeyEvent() || event->IsMouseButtonEvent())
                    Singletons::Input().ProcessEvent(event);
                // Give the GUI hierarchy a chance at the event and then delete it.
                screen->ProcessEvent(event);
                Delete(event);
            }

            // Perform all off-screen game processing.
            world->ProcessFrame(current_real_time);

            // Turn the EventQueue crank, Perform off-screen GUI processing,
            // turn the EventQueue crank again, and then draw everything.
            screen->OwnerEventQueue()->ProcessFrame(current_real_time);
            screen->ProcessFrame(current_real_time);
            screen->OwnerEventQueue()->ProcessFrame(current_real_time);
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
    <li>Left-click and drag the view around to see the parallaxing effect
        between the two ObjectLayers.</li>
    <li>Add more starfield ObjectLayers at different z depths</li>
    <li>Using file "resources/explosion1a_small.png" as nebulae, add a
        foreground nebula field with an ObjectLayer Z depth of a negative
        value which will put it in front of the main object layer.
        Move the view around and see the effect.  Now zoom the view in
        and out and see what the effect is on the nebula layer.</li>
    </ul>

Thus concludes lesson06.  \f$ \displaystyle \sum_{i=0}^{6}lesson_{i}\approx0 \f$
*/
