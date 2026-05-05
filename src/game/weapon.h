//
// Created by Alan Pitcher on 4/29/2026.
//

#ifndef WEAPON_H
#define WEAPON_H

#define MAX_PLAYER_WEAPONS 3
#include <stdbool.h>

typedef enum {
    WEAPON_STATE_READY,
    WEAPON_STATE_RELOAD,
    WEAPON_STATE_EMPTY,
    WEAPON_STATE_FIRING,
    WEAPON_STATE_SELECT,
    WEAPON_STATE_DESELECT
} WEAPON_STATE;

typedef enum {
    WEAPON_TYPE_HITSCAN,
    WEAPON_TYPE_PROJECTILE
} WEAPON_TYPE;

typedef struct {
    const char *name;
    int id;
    int damage;
    int ammoInMag;
    int magCapacity;
    int reserveAmmo;
    WEAPON_STATE state;
    WEAPON_TYPE type;
    double fireCooldown;
    double fireTimer;
    double reloadDuration;
    double reloadTimer;
    double range;
}Weapon;


//  Initializes the default starter pistol weapon
void weapon_init_pistol(Weapon *weapon);

//  To advance timers and state transitions
void weapon_update(Weapon *weapon, double deltaTime);

//  Check if firing the weapon is legal
bool weapon_can_fire(const Weapon *weapon);

// Consumes ammo, starts cooldown, changes state, and returns whether a shot was actually fired
bool weapon_fire(Weapon *weapon);

//  Check if reload is legal
bool weapon_can_reload(const Weapon *weapon);

//  Starts reload state/timer
void weapon_start_reload(Weapon *weapon);

//  Moves ammo from reserve into the mag when the reload is complete
void weapon_finish_reload(Weapon *weapon);

//  Check if weapon has no ammo and normalize EMPTY state when applicable
bool weapon_is_empty(Weapon *weapon);

//  Clears weapon data and resets the slot to an empty default state
void weapon_free(Weapon *weapon);



#endif //WEAPON_H
