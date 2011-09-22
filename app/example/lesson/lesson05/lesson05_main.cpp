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
// Lesson 05 - Dynamic Objects For Fun And Profit
// ///////////////////////////////////////////////////////////////////////////


/** @page lesson05 Lesson 05 - Dynamic Objects For Fun And Profit
@code *//* @endcode
This lesson will show you how to make dynamic objects (game objects which can
move and interact).  This will be done by subclassing @ref Xrb::Engine2::Entity
and providing a game-specific implementation.  We will also have to subclass
@ref Xrb::Engine2::World to provide the code to control its subordinate Entity
objects.

    <ul>
    <li>@ref lesson05_main.cpp "This lesson's source code"</li>
    <li>@ref lessons "Main lesson index"</li>
    </ul>

In this lesson we will be creating an animated gravitational planet/moon
system.  This will require us to subclass the pure virtual Engine2::Entity
class with our own application-specific implementation, and to write code to
update the positions of the game objects to simulate gravitation.  The latter
will be in a custom subclass of Engine2::World.

Below, in the documentation for AwesomeWorld::HandleFrame, there is an
explanation of Euler Integration, which is absolutely central to game
programming.  Make sure not to skip it.

First, more detail on Object and Entity and their relationship.

As shown in the @ref lesson04 "previous lesson", Object is the physical,
visible game object which has as its properties: position, scale and angle.
By itself, it can't move or be interacted with.  Its subclasses implement its
Draw method -- as of Sept 2006, Sprite and Compound.  Alone, this is referred
to as a "static object" (as opposed to a "dynamic object"; see
Xrb::Engine2::Object::IsDynamicObject).

Entity (Xrb::Engine2::Entity) can be thought of as "imbuing a soul" upon
Object.  Entity is a pure virtual class intended to be subclassed to add all
the application-specific properties and code necessary for an interactive
game object.  An Object and Entity are both instantiated, and then the Entity
instance is attached to the Object instance, and which point, the Object
instance is a "dynamic object".  The Entity baseclass doesn't provide any
substance besides the bare minimum framework -- the actual form of the "soul"
is completely up to the application-specific implementation.

In this particular lesson, the application-specific properties we will add
to our custom subclass of Entity are mass, velocity and accumulated force.
These values will be used by the gravitation simulation code to update the
position of each respective dynamic object.

Our custom subclass of World will perform two functions: a one-time generation
of the game world (a large planet and many orbiting moons), and once-per-frame
calculations for the simulation of the gravitational system using the
properties of each dynamic object in the World.

<strong>Procedural Overview</strong> -- Items in bold are additions/changes to the previous lesson.

    <ul>
    <li>Global declarations</li>
        <ul>
        <li><strong>Declare subclass of Engine2::Entity specific to this app.</strong></li>
        <li><strong>Declare subclass of Engine2::World specific to this app.</strong></li>
            <ul>
            <li><strong>The constructor will populate the game world.</strong></li>
            <li><strong>Override HandleFrame to do once-per-frame gravitational
                simulation calculations and to update the velocities and positions
                of the dynamic objects.</strong></li>
            </ul>
        <li>Declare subclass of Engine2::WorldView specific to this app.</li>
        </ul>
    <li>Main function</li>
        <ul>
        <li>Initialize the Pal and game engine singletons.  Create the Screen object.</li>
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
                <li>Calculate the Singleton::Pal().Sleep duration necessary to achieve the desired framerate.</li>
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
There really isn't much to this subclass.  All we're actually doing is adding
three properties and various accessors/modifiers for them.  The m_mass value is
a scalar value representing the first moment of inertia as defined by Newtonian
mechanics (e.g. 28 kilograms); the higher this value is, the heavier the object
is, and the more gravity it applies to other objects.  The m_velocity value is
the vector representing the change in position per second (i.e. the derivative
of the position vector).  Finally, the m_force vector value is used by
AwesomeWorld during each game loop to calculate the total accumulated force on
each object; this value isn't actually a property of a body in Newtonian
mechanics -- it's just a value used by our simulation code.
@code */
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
    Float Mass () const { return m_mass; }
    FloatVector2 const &Velocity () const { return m_velocity; }
    FloatVector2 const &Force () const { return m_force; }

    // Modifiers for the properties of AwesomeEntity.  The ASSERT_NAN_SANITY_CHECK
    // macro is used in various places in Engine2 code to quickly catch common
    // bugs in game code which result in NaN values being fed to the snake.
    void SetMass (Float mass)
    {
        ASSERT_NAN_SANITY_CHECK(Math::IsFinite(mass));
        ASSERT1(mass > 0.0f);
        m_mass = mass;
    }
    void SetVelocity (FloatVector2 const &velocity)
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

private:

    Float m_mass;
    FloatVector2 m_velocity;
    FloatVector2 m_force;
}; // end of class AwesomeEntity

/* @endcode
Our custom subclass of World will do the two things mentioned above: create
the game world and populate it with objects, and handle per-frame gravitational
simulation calculations.
@code */
class AwesomeWorld : public Engine2::World
{
public:

    /* @endcode
    The constructor will create the single ObjectLayer to contain all the
    objects which will be created next.  The dynamic objects which will
    populate the game world will each be a Sprite instance and AwesomeEntity
    instance pair.  A large, heavy "planet" and many small, light "moons" will
    be created.  The moons' positions and velocities will be initialized to
    put them into circular orbit of the larger planet using Kepler's 3rd law.
    The scalar member value m_gravitational_constant is the symbol G in the
    Newtonian equation for gravitational force between two bodies.
    @code */
    AwesomeWorld ()
        :
        Engine2::World(NULL),
        m_gravitational_constant(60.0f)
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

        // Create a large, heavy planet.
        sprite = Engine2::Sprite::Create("resources/demi_3.png");
        planet = new AwesomeEntity();
        sprite->SetEntity(planet);
        planet->SetTranslation(FloatVector2::ms_zero);
        planet->SetScaleFactor(250.0f);
        planet->SetMass(100.0f * Sqr(planet->PhysicalRadius()));
        AddDynamicObject(sprite, object_layer);

        // Create a bunch of small, light moons.
        static Uint32 const s_moon_count = 50;
        for (Uint32 i = 0; i < s_moon_count; ++i)
        {
            sprite = Engine2::Sprite::Create("resources/shade_3.png");
            AwesomeEntity *moon = new AwesomeEntity();
            sprite->SetEntity(moon);
            sprite->SetZDepth(-0.1f);
            moon->SetScaleFactor(Math::RandomFloat(10.0f, 20.0f));
            moon->SetMass(0.01f * Sqr(moon->PhysicalRadius()));
            // Pick a distance to orbit the moon at.
            Float minimum_orbital_radius = planet->PhysicalRadius() + moon->PhysicalRadius() + 100.0f;
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
            Float gravitational_force = CalculateGravitationalForce(planet, moon);
            /* @endcode
            We will solve for the necessary orbital speed by using Kepler's Third Law; let \f$v\f$ be scalar orbital velocity (speed), \f$r\f$ be the distance between the centers of the two bodies, and \f$a\f$ be scalar acceleration.
            \f[ \frac{v^2}{r} = a \f]
            We must also replace \f$a\f$ by known quantities.  This can be done using Newton's Second Law; let \f$a\f$ be scalar acceleration, \f$f\f$ be scalar force (magnitude of the force vector), and \f$m\f$ be mass (of the orbiting body, so in this case, the small moon).
            \f[ a = \frac{f}{m} \f]
            Composing the two, we get
            \f[ \frac{v^2}{r} = \frac{f}{m} \f]
            Solve for \f$v\f$:
            \f[ v^2 = \frac{fr}{m} \f]
            \f[ v = \sqrt{\frac{fr}{m}} \f]
            @code */
            Float orbital_speed = Math::Sqrt(gravitational_force * orbital_radius / moon->Mass());
            // The velocity must be perpendicular to the vector joining the
            // centers of the planet and the moon.
            moon->SetVelocity(orbital_speed * Math::UnitVector(angle+90.0f));
            // Finally add it to the world.
            AddDynamicObject(sprite, object_layer);
        }
    }

protected:

    /* @endcode
    In our override of HandleFrame, we put the per-frame calculations necessary
    for the gravitational simulation.  First, we iterate through all distinct
    pairs of different entities and apply gravitational forces between them.
    Then update the velocities (apply the forces accumulated during this frame),
    and finally update the positions (based on the corresponding velocity values).
    @code */
    virtual void HandleFrame ()
    {
        Uint32 entity_capacity = EntityCapacity();

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

                ASSERT1(entity0 != entity1);

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
                FloatVector2 force_direction = (entity1->Translation() - entity0->Translation()).Normalization();
                // Apply equal and opposite gravitational force to both entities.
                entity0->IncrementForce( gravitational_force * force_direction);
                entity1->IncrementForce(-gravitational_force * force_direction);
            }
        }

        /* @endcode
        The calculations for velocity based on acceleration and for position based
        on velocity are using what's known as Euler Integration (see
        http://en.wikipedia.org/wiki/Euler_integration and
        http://en.wikipedia.org/wiki/Numerical_ordinary_differential_equations for
        technical descriptions).  The general idea is that you have a frequently
        changing value (e.g. velocity) which will be referred to as the principal,
        and you have the rate at which the principal changes (e.g. acceleration)
        which will be referred to as the derivative.  Euler Integration is a method
        for updating the principal based on the derivative, using the the time-step
        value (e.g. <tt>FrameDT()</tt>).  In the following, the time-step is
        given by <tt>dt</tt>.

        <tt>principal += derivative * dt</tt>

        In this lesson, there are two integrations to perform: updating velocity
        (the principal) using acceleration (the derivative), and updating position
        (the principal) using velocity (the derivative).  It should be noted that
        both of these values are vector-valued, and that the above and
        above-referenced descriptions of Euler Integration appear to be
        scalar-valued.  The derivative of a vector value is defined as a
        component-wise derivative (the vector containing derivative of the
        X-component and the derivative of the Y-component).  The time delta is
        always scalar.

        Believe it or not, by doing this, you're actually computing numeric
        solutions for differential equations.  Euler Integration is a very simple
        method for numeric integration, but is relatively inaccurate.  Fortunately
        for the purposes of games, it works just fine.  For a more accurate method,
        see http://en.wikipedia.org/wiki/Runge-Kutta_methods -- it is much more
        complicated and difficult to implement, but if accuracy is a consideration
        (e.g. in scientific computation) then it's worth it.
        @code */
        // Update the velocity vector of each entity with the accumulated force
        // and update the position vector using the newly calculated velocity.
        for (Uint32 i = 0; i < entity_capacity; ++i)
        {
            AwesomeEntity *entity = dynamic_cast<AwesomeEntity *>(GetEntity(i));
            if (entity == NULL)
                continue;

            ASSERT1(entity->Mass() > 0.0f);
            // Use Euler Integration to calculate the new velocity, based on
            // the accumulated force during this frame.
            entity->IncrementVelocity(entity->Force() / entity->Mass() * FrameDT());
            // Reset the accumulated force for next frame.
            entity->ResetForce();
            // Use Euler Integration again to calculate the new position,
            // based on the entity's velocity.
            entity->Translate(entity->Velocity() * FrameDT());
        }

        /* @endcode
        You must always call the superclass' HandleFrame method, as it performs
        vital processing -- specifically of the EventQueue and PhysicsHandler,
        which will be covered in a later lesson.
        @code */
        Engine2::World::HandleFrame();
    }

private:

    /* @endcode
    The following function is just a helper, useful in condensing a cluttery
    equation down into a nice li'l old self-documenting function call.  The
    returned value is the computed value of Newton's Law Of Universal
    Gravitation:

    \f[ F = G \frac{m_0 m_1}{r^2} \f]

    See http://en.wikipedia.org/wiki/Gravitation for more info.  In order to
    prevent a divide by zero, if the entities are too close (overlapping), the
    return value is zero.
    @code */
    Float CalculateGravitationalForce (AwesomeEntity *entity0, AwesomeEntity *entity1) const
    {
        ASSERT1(entity0 != NULL && entity1 != NULL);
        FloatVector2 entity_offset(entity1->Translation() - entity0->Translation());
        Float distance = entity_offset.Length();
        // If they're touching, don't apply gravitational force (this
        // is to avoid a divide by zero if their positions coincide).
        if (distance < entity0->PhysicalRadius() + entity1->PhysicalRadius())
            return 0.0f;
        else
            return
                m_gravitational_constant *
                entity0->Mass() * entity1->Mass() /
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
    virtual bool ProcessMouseWheelEvent (EventMouseWheel const &e)
    {
        // Rotate the view on ALT+mouse-wheel-up/down.
        if (e.IsEitherAltKeyPressed())
            RotateView((e.ButtonCode() == Key::MOUSEWHEELUP) ? -15.0f : 15.0f);
        // Otherwise, we will zoom the view on mouse-wheel-up/down.
        else
            ZoomView((e.ButtonCode() == Key::MOUSEWHEELUP) ? 1.2f : 1.0f / 1.2f);
        // Indicates that the event was used by this method.
        return true;
    }
    // This method is the mouse motion analog of ProcessMouseWheelEvent.
    virtual bool ProcessMouseMotionEvent (EventMouseMotion const &e)
    {
        // Only do stuff if the left mouse button was pressed for this event.
        if (e.IsLeftMouseButtonPressed())
        {
            // Move the view by a delta which is calculated by transforming
            // the screen coordinates of the event to world coordinates as used
            // by WorldView.
            MoveView(
                ParallaxedScreenToWorld() * FloatVector2::ms_zero -
                ParallaxedScreenToWorld() * e.Delta().StaticCast<Float>());
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
    // Shutdown the Pal and singletons.
    Singleton::Pal().Shutdown();
    Singleton::Shutdown();
}

int main (int argc, char **argv)
{
    fprintf(stderr, "main();\n");

    // Initialize engine singletons.
    Singleton::Initialize(SDLPal::Create, "none");
    // Initialize the Pal.
    if (Singleton::Pal().Initialize() != Pal::SUCCESS)
        return 1;
    // Set the window caption.
    Singleton::Pal().SetWindowCaption("XuqRijBuh Lesson 05");
    // Create Screen object and initialize given video mode.
    Screen *screen = Screen::Create(800, 600, 32, false);
    // If the Screen failed to initialize, print an error message and quit.
    if (screen == NULL)
    {
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
        // Create our sweet game world via a call to CreateAndPopulateWorld.
        AwesomeWorld *world = new AwesomeWorld();
        // Create the WorldViewWidget as a child of screen.  This is what will
        // contain an instance of WorldView and will cause it to be rendered.
        Engine2::WorldViewWidget *world_view_widget = new Engine2::WorldViewWidget(screen->Context());
        screen->AttachChild(world_view_widget);
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
        while (!screen->IsQuitRequested())
        {
            // Get the current real time and figure out how long to sleep, then sleep.
            current_real_time = 0.001f * Singleton::Pal().CurrentTime();
            Sint32 milliseconds_to_sleep = Max(0, static_cast<Sint32>(1000.0f * (next_real_time - current_real_time)));
            Singleton::Pal().Sleep(milliseconds_to_sleep);
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

            // Perform all off-screen game processing.
            world->ProcessFrame(current_real_time);

            // Turn the EventQueue crank, Perform off-screen GUI processing,
            // turn the EventQueue crank again, and then draw everything.
            screen->OwnerEventQueue()->ProcessFrame(current_real_time);
            screen->ProcessFrame(current_real_time);
            screen->OwnerEventQueue()->ProcessFrame(current_real_time);
            screen->Draw(current_real_time);
        }

        // Detach world_view_widget from its parent, delete world_view_widget,
        // then delete world, in that order.  This will automatically delete
        // our AwesomeWorldView instance.
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
    <li>Change the AwesomeWorld constructor so all the moon entities spawn
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
    <li>Add UI controls to change the world's gravitational constant and
        the desired framerate.</li>
    <li>Revert the planetary system back to the default single large planet
        with many moons.  Change the desired framerate to something low such
        as 5 frames per second and see what the effect on the numerical
        integration is.</li>
    <li><strong>Extra credit:</strong> Implement Runge-Kutta Integration (see
        http://en.wikipedia.org/wiki/Runge-Kutta_methods for info) in addition
        to the less accurate Euler Integration in AwesomeWorld::HandleFrame.
        Make AwesomeWorld configurable as to which integration method should
        be used, and make the SPACE key toggle the method (you will need to
        override <tt>virtual bool ProcessKeyEvent (EventKey const &e)</tt> in
        AwesomeWorldView).  See how the different integration methods perform
        compared to each other at different framerates, especially low ones.</li>
    </ul>

Thus concludes lesson05, you crazy almost-game-programming bastard, you.
*/
