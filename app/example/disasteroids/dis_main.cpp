// ///////////////////////////////////////////////////////////////////////////
// dis_main.cpp by Victor Dods, created 2005/10/28
// ///////////////////////////////////////////////////////////////////////////
// Unless a different license was explicitly granted in writing by the
// copyright holder (Victor Dods), this software is freely distributable under
// the terms of the GNU General Public License, version 2.  Any works deriving
// from this work must also be released under the GNU GPL.  See the included
// file LICENSE for details.
// ///////////////////////////////////////////////////////////////////////////

#include "xrb.hpp"

#include "dis_commandlineoptions.hpp"
#include "dis_config.hpp"
#include "dis_master.hpp"
#include "xrb_screen.hpp"
#include "xrb_sdlpal.hpp"

using namespace std;
using namespace Xrb;

#define CONFIG_FILE_PATH "disasteroids.config"

Dis::Config g_config;

// shuts down our Pal and the singletons.
void CleanUp ()
{
    Singleton::Pal().Shutdown();
    Singleton::Shutdown();
}

int main (int argc, char **argv)
{
    fprintf(stderr, "\nmain();\n");

    {
        // read in the user's config file (video resolution, key binds, etc).
        g_config.Read(CONFIG_FILE_PATH);

        // initialize the commandline options with the config values and then
        // parse the commandline into the options object.
        Dis::CommandLineOptions options(argv[0]);
        options.InitializeFullscreen(g_config.Boolean(Dis::VIDEO__FULLSCREEN));
        options.InitializeResolution(g_config.Resolution());
        options.InitializeKeyMapName(g_config.GetString(Dis::SYSTEM__KEY_MAP_NAME));
        options.Parse(argc, argv);
        if (!options.ParseSucceeded() || options.IsHelpRequested())
        {
            options.PrintHelpMessage(cerr);
            return options.ParseSucceeded() ? 0 : 1;
        }
        // update the config values with the parsed commandline options.
        g_config.SetBoolean(Dis::VIDEO__FULLSCREEN, options.Fullscreen());
        g_config.SetResolution(options.Resolution());
        g_config.SetString(Dis::SYSTEM__KEY_MAP_NAME, options.KeyMapName());

        Singleton::Initialize(SDLPal::Create, options.KeyMapName().c_str());
        // set the GlTextureAtlas size based on the config values
        Singleton::Pal().GlTextureAtlasSize(g_config.GlTextureAtlasSize());

        // register on-exit function, which will be called after main() has returned.
        atexit(CleanUp);

        if (Singleton::Pal().Initialize() != Pal::SUCCESS)
            return 1;

        Singleton::Pal().SetWindowCaption("Disasteroids");

        // init the screen
        Screen *screen = Screen::Create(
            options.Resolution()[Dim::X],
            options.Resolution()[Dim::Y],
            32,
            options.Fullscreen());
        if (screen == NULL)
            return 2;

        // create and run the game
        {
            Dis::Master master(screen);
            master.Run();
        }

        Delete(screen);

        // write the config file back out (because it may have changed during
        // the execution of the game.
        g_config.Write(CONFIG_FILE_PATH);
    }

    // return with no error condition
    return 0;
}

/*

//////////////////////////////////////////////////////////////////////////////
// Disasteroids Game Design
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Premise
//////////////////////////////////////////////////////////////////////////////

The player is piloting a small spaceship which is on a quest really just to
blow shit up.  The game only ends when the action gets too crazy and the
player snuffs it.

//////////////////////////////////////////////////////////////////////////////
// Game world
//////////////////////////////////////////////////////////////////////////////

The game is played in a single map, where there aren't discrete levels, but
rather the game escalates further and further in realtime.

The map is filled with asteroids, which the player can blow up, and which
split apart into smaller asteroids and sometimes produce small chunks of
minerals of different types.

Enemy ships sometimes appear to kill the player (duh).

//////////////////////////////////////////////////////////////////////////////
// Equipment And Upgrades
//////////////////////////////////////////////////////////////////////////////

Weapons (each have infinite ammo, unless otherwise specified)
- pea shooter (player starts off with this)
  * upgrades will be to the projectile speed and damage (the projectiles will
    not harm you by default)
- lasers
  * upgrades will be to the power and range (the lasers will not harm the
    player by default)
- flame thrower (will harm the player by default)
  * upgrades will be to the power and range to make the fire
    not harm you
- gauss gun (high power instant-hit type weapon)
  * upgrades will be to the damage of the projectile and the recharge time
    (by default the projectile will not harm you)
- grenade launcher (hold the button down to fire, release to explode)
  * upgrades will be to the power of the grenade explosion and to make the
    explosions not harm you
- missile launcher
  * upgrades will be to the projectile speed and explosion power,
    dumb-seeking, smart-seeking, to make the explosions not hurt you
- EMP core - disables enemies (maybe with certain exceptions) for a short
  time.

Engines (maybe combine with armor)
- engine level X (each level outputs more thrust, and uses more power)
  * upgrade: reverse-thrust

Armor (designed to be able to deflect weaker enemies' attacks with no damage)
- no armor (player starts off with this)
- armor level X (as the level goes up, it provides better protection, weighs
  the player's ship down more, and increases the size of the player's ship).

Shields
level 0: plain old shields
level 1: strong against collision damage
level 2: damages enemies (also protects against collision damage)
level 3: strong against explosion damage (also damages enemies and strong
         against collision damage)

Power Generators
- power generators give a minimum base level of output (which doesn't require
  any fuel), and will consume certain minerals to provide more power.  power
  generators have a built-in battery, since expenditures will take an amount
  of power (current * time)
  * upgrades are to the rate of power production and built-in storage capacity

Tractors
- pulls/pushes/holds objects.  the left mouse button activates
  the pull mode, the right mouse button activates the push mode, while holding
  down both buttons activates the hold mode (objects are held in place
  relative to the player's ship).  this is primarily used for sucking up
  minerals.
  * two modes for the tractor: ambient (where it is not directional at
    all), and directional, where the tractor effects only where the
    ship is pointing (or if the turret is activated, where the mouse pointer
    is).
  * upgrades
    - power
    - does not enact a counterforce on the player's ship, only the tractoree --
      this can be used to fling really heavy stuff in the way of enemies.

Misc Equipment
- mr. fusion - allows the player to collect the "useless" rock to generate
  power (but only ones that are small enough).  this will be useful towards
  the end of the game when the player is too preoccupied with blowing shit up.
- dematerializer - modifies the tractor so that when it is activated upon
  minerals, they will dematerialize slowly into the ship's inventory.

//////////////////////////////////////////////////////////////////////////////
// Minerals
//////////////////////////////////////////////////////////////////////////////

- asteroids: not technically a "mineral" as such, but with the "mr fusion"
  you can use small asteroids for power
- hydrates: very common, used for power production, and some equipment
- oxides: very common, used in building all equipment
- alkalines: common, used in building all equipment
- metalloids: common, used in building most equipment
- heavy metals: somewhat uncommon, used in most equipment
- radioactives: least common, used in top-of-the-line equipment

//////////////////////////////////////////////////////////////////////////////
// Gameplay
//////////////////////////////////////////////////////////////////////////////

The player starts off in the world, where the asteroids are nice and calm and
there is no enemy activity.  This will let the player get acquainted with the
feel of the game (there can be an option to start at a more difficult level).
The initial equipment the player starts of is naturally the crappiest -- a
pea shooter, no shields, no tractor, etc.  The calm at the beginning of
the game will also let the player accumulate some minerals to build some
equipment.

Minerals are found by blowing up asteroids.  The different types of minerals
will each have a frequency ratio (giving the statistical probability that
a particular mineral type will be found in an asteroid), meaning that rare
minerals will not be found very frequently.  *Possibly make different
asteroids (which look different) have different mineral frequencies, e.g.
a gold asteroid will render lots of gold, etc.

When an asteroid blows up, most of it will be useless rock, but there will
be a few small mineral particles created, which the player can collect.

In order to collect minerals, the player just needs to touch the minerals.
They will be automatically sucked up into the player's inventory.  There is
no limit to the amount of minerals of any type the player can hold.

Building equipment happens instantly, but only when the player has enough of
the necessary minerals.  Equipment is then added to the player's inventory,
and is only lost when killed.

Soon after the start of the game (and throughout the game), more asteroids
will be spawned (off screen), as well as enemies.  Enemies will sometimes be
spawned on-screen.  The first enemies will be the weakest/dumbest ones, but
successive waves of enemies will grow stronger as the game progresses.

Killing enemies will give the player points, and the more frequently the
player kills enemies, the more points are given.  There will be a kill-o-meter
(not necessarily visible to the player) which increases when a player kills
an enemy, and its value, multiplied by the enemy's point value gives the
number of points the player receives for that kill.  The kill-o-meter will
decay exponentially, so that if the player doesn't kill any enemies for a
short while, there will be no bonus.

Points are used to gain extra lives.  The number of points required to get
an extra life starts off relavitely low, but then increases exponentially,
to make it impossible to keep getting extra lives forever.

The player's score (and time alive) will naturally be fed into a high scores
scoreboard.

If a player's ship explodes, he will lose one life.  All the equipment that
the player was using at the time of death will be lost (maybe with a few
exceptions).

//////////////////////////////////////////////////////////////////////////////
// Player Ship Properties
//////////////////////////////////////////////////////////////////////////////

The player's ship will be able to turn left or right, and be able to thrust
in the direction it is pointing.  There is an engine upgrade which allows
the player to reverse thrust.

The player's ship has the following properties:
- health points
- defense points (from armor, gives amount of protection for health loss)
- shield points (which recharge over time)
- power production rate
- available power (power production + power stored in battery)
- weapon recharge (indicator for when the weapon can fire next, or not
  applicable for constant-fire weapons such as the laser and flame thrower)

//////////////////////////////////////////////////////////////////////////////
// Game Environment And Behavior
//////////////////////////////////////////////////////////////////////////////

The game will employ Newtonian collision physics (using only circles as the
collision geometry).  The exception is that there will be a maximum speed
imposed on all objects, to keep the physics engine sane and to prevent
ridiculously fast flying asteroids.

To keep the game from producing so many game objects that it slows to a crawl,
small asteroids and minerals will decay (in size) and disappear after living
for a short period.  This should only apply to asteroids that were created
as debris from an explosion of a larger asteroid (so small asteroids can exist
in the game world, if spawned "naturally" by the game.

The environmental effects (explosions and thrust fire and such) should have
the ability to be capped at a certain point (when there's too much going on
in the world), so that there will be a managable level of shit for the physics
engine to cope with (and so visual effects can be turned down on low-end
systems)

//////////////////////////////////////////////////////////////////////////////
// Enemies (and upgraded versions of enemies)
//////////////////////////////////////////////////////////////////////////////

Each different type of enemy will come in several versions, each version
looking similar, but colored differently (a-la Phantasy Star) and in different
sizes.

- Interloper - dumbest enemy which just flies directly at the player,
  ignoring everything else.  This often leads to it being killed by collisions
  with asteroids.  It has no weapons, it just wants to crash into the player
  at a high velocity.  The purpose of this is to keep the player from getting
  complacent and staying still for too long, and to generally annoy the player.
  * upgraded versions include:
    - a smarter version that plots an intercept course instead of aiming
      directly at the player
    - a higher-acceleration version
    - one that has a melee-type weapon that it uses when it hits the player,
      to inflict more damage
- Shade - slow alien craft that wanders vaguely near the player,
  keeping to a relatively low speed, and fires low-speed projectiles.  The
  base version aims directly at the player, +- a few degrees (bad aim).
  * upgraded versions include:
    - smarter version that fires the projectiles on an intercept course.
    - better-aim version that doesn't have the +- few degrees of error.
    - higher rate of fire.
- Revulsion - like the ghosts from super mario brothers, they will freeze (or
  maybe become invisible?) when the player is looking at/aiming at them.  but
  when the player looks/aims away, the ghost will charge up and fire a gauss
  gun type weapon.  the base version has a +- few degrees of aim error.  this
  enemy will plot a course parallel to the player.
  * upgraded versions include:
    - no +- few degrees of aim error
    - no ghosting -- will shoot you in the face
- Devourment - just wants to eat large things, including
  asteroids and ships.  it has a tractor "mouth" which sucks things
  towards it, and inflicts heavy damage on them once they touch the front of
  it.  isn't terribly belligerent, and will mainly go after the yummy
  asteroids, unless a ship wanders too close.  very large ship, and has lots
  of health.  moves slowly. the main hazard here is when it chews up an
  asteroid, the asteroid will explode violently.  they will also tractor-
  beam up minerals nearby, thus pissing off the player.
  * upgrades include:
    - smaller and slightly faster one that goes only after ships.
- Demi - large, intelligent ship that moves purposefully and slowly, but
  then strikes out decisively when it does.  has a decent array of weapons:
  - flame thrower for close range battle
  - gauss gun for long-range battle
  has lots of defensive mechanisms:
  - a "flare" type projectile it will shoot when a seeking missile
    approaches, which the missile will seek, instead of it (but this
    will be used only when there is a missile in front of it).
  - a non-full-circle shield that will deflect weapons (maybe with certain
    exceptions)
  - lots and lots of health

//////////////////////////////////////////////////////////////////////////////
// User Interface
//////////////////////////////////////////////////////////////////////////////

The user interface will be kept very tidy, with not much on the screen at
any given time.  The only HUD indicators will be:

- score (number printout, on the top-right of the screen)
- time alive (time-formatted number printout, on the top-left of the screen)
- ship/shield health (combination colored bar meter, lower-left of the screen)
- power available (colored bar meter, lower-center of the screen)
- weapon status (color bar meter for fire-readiness, weapon/ammo indicator,
  lower-right of the screen)

The game can be paused at any time.  Pausing the game will cause the game
world to freeze in place, and a status screen will appear on the right side
of the screen, pushing the game view aside.  In this status screen will be
the player's mineral and equipment inventory.  The mineral inventory will be
a short series of colored number displays indicating the amount of minerals
of each type that the player has.  The equipment inventory will be more
substantial, containing a row of buttons for each type of equipment (and one
row for each upgrade?)

- one row for each weapon
- one row for engines
- one row for armors
- one row for shields
- one row for power generators
- one row for misc shit (mr fusion, and other one-shot things)

Each equipment button will either be blacked out, (indicating the player does
not have it and can't afford it), darkened (indicating the player does
not have it but can afford it), normal (indicating the player has it), or
flashing (indicating that the player has it and it is equipped).

The player can click on any of the buttons (except for the blacked out ones)
to do the following:

- for a darkened button (equipment the player does not have but can afford),
  clicking on it will build that piece of equipment, put it into the player's
  inventory, and equip/activate it.
- for a normal button, (equipment the player current owns), clicking on it
  will equip/activate it.
- for a flashing button (equipment that is currently equipped/activated),
  nothing will happen.

Moving the mouse over a darkened or blackened button will show how many
of each type of minerals it costs to build.

//////////////////////////////////////////////////////////////////////////////
// disasteroids power-up redesign (rename to Option, as in gradius)
//////////////////////////////////////////////////////////////////////////////

- powerups are similar to minerals in that they are a physical object that occur in the game world and that the player collects.
- powerups should be emitted by asteroids, Devourments and Demis in a way similar to minerals, but with less frequency.
- EMP core should be a powerup instead of a weapon that is bought in the inventory screen
- inventory screen should show powerup inventory (a count of stored powerups)
- stoke-mode: as the player kills enemies, the stoke-o-meter goes up, and once it reaches a critical level, the player starts flashing
  for a limited time, during which a powerup can be used for free (but the player has to manually activate it).
- instead of collecting particular powerups, count powerups as generic things which can be spent to activate particular powerups.
  so a particular type of powerup can be 'equipped' (like normal weaponry) and fired at the user's discretion, which will spend one
  powerup unit.
- equipped powerup which can be activated similar to firing the main weapon using some button
- activate particular powerup type using particular keys (e.g. 7 8 9 0)

//////////////////////////////////////////////////////////////////////////////
// types of powerups (there shouldn't be too many)
//////////////////////////////////////////////////////////////////////////////

- EMP blast
- increased damage/firing rate/ship speed.
- increased shields/power/mass
- bullet-time (slows game down so that the player can react with inhuman accuracy)
  this should probably also increase the player's speed, otherwise it will feel
  like only slow-motion.

*/

