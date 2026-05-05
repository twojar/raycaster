//
// Created by Alan Pitcher on 4/29/2026.
//

#include "game/weapon.h"

#include <stddef.h>

static int weapon_min_int(int a, int b) {
    return (a < b) ? a : b;
}

void weapon_init_pistol(Weapon *weapon) {
    if (weapon == NULL) return;

    weapon->name = "Pistol";
    weapon->id = 0;
    weapon->damage = 12;
    weapon->ammoInMag = 12;
    weapon->magCapacity = 12;
    weapon->reserveAmmo = 48;
    weapon->state = WEAPON_STATE_READY;
    weapon->type = WEAPON_TYPE_HITSCAN;
    weapon->fireCooldown = 0.2;
    weapon->fireTimer = 0.0;
    weapon->reloadDuration = 1.2;
    weapon->reloadTimer = 0.0;
    weapon->range = 18.0;
}

void weapon_update(Weapon *weapon, double deltaTime) {
    if (weapon == NULL) return;
    if (deltaTime < 0.0) return;

    if (weapon->fireTimer > 0.0) {
        weapon->fireTimer -= deltaTime;
        if (weapon->fireTimer < 0.0) {
            weapon->fireTimer = 0.0;
        }
    }

    if (weapon->reloadTimer > 0.0) {
        weapon->reloadTimer -= deltaTime;
        if (weapon->reloadTimer < 0.0) {
            weapon->reloadTimer = 0.0;
        }
    }

    if (weapon->state == WEAPON_STATE_RELOAD) {
        if (weapon->reloadTimer <= 0.0) {
            weapon_finish_reload(weapon);
        }
        return;
    }

    if (weapon->state == WEAPON_STATE_FIRING && weapon->fireTimer <= 0.0) {
        weapon->state = (weapon->ammoInMag > 0) ? WEAPON_STATE_READY : WEAPON_STATE_EMPTY;
    } else if (weapon->state == WEAPON_STATE_READY && weapon->ammoInMag <= 0) {
        weapon->state = WEAPON_STATE_EMPTY;
    }
}

bool weapon_can_fire(const Weapon *weapon) {
    if (weapon == NULL) return false;
    if (weapon->state != WEAPON_STATE_READY) return false;
    if (weapon->fireTimer > 0.0) return false;
    if (weapon->ammoInMag <= 0) return false;
    return true;
}

bool weapon_fire(Weapon *weapon) {
    if (weapon == NULL) return false;

    if (!weapon_can_fire(weapon)) {
        if (weapon->ammoInMag <= 0) {
            weapon->state = WEAPON_STATE_EMPTY;
            printf("%s: magazine empty\n", weapon->name);
        } else if (weapon->state == WEAPON_STATE_RELOAD) {
            printf("%s: cannot fire while reloading\n", weapon->name);
        } else if (weapon->fireTimer > 0.0) {
            printf("%s: still on cooldown\n", weapon->name);
        }
        return false;
    }

    weapon->ammoInMag--;
    weapon->fireTimer = weapon->fireCooldown;
    weapon->state = WEAPON_STATE_FIRING;
    printf("%s: fired (%d/%d, reserve %d)\n", weapon->name, weapon->ammoInMag, weapon->magCapacity, weapon->reserveAmmo);
    return true;
}

bool weapon_can_reload(const Weapon *weapon) {
    if (weapon == NULL) return false;
    if (weapon->state != WEAPON_STATE_READY && weapon->state != WEAPON_STATE_EMPTY) return false;
    if (weapon->state == WEAPON_STATE_RELOAD) return false;
    if (weapon->reserveAmmo <= 0) return false;
    if (weapon->ammoInMag >= weapon->magCapacity) return false;
    return true;
}

void weapon_start_reload(Weapon *weapon) {
    if (weapon == NULL) return;
    if (!weapon_can_reload(weapon)) {
        if (weapon->reserveAmmo <= 0) {
            printf("%s: no reserve ammo to reload\n", weapon->name);
        } else if (weapon->ammoInMag >= weapon->magCapacity) {
            printf("%s: magazine already full\n", weapon->name);
        } else if (weapon->state == WEAPON_STATE_RELOAD) {
            printf("%s: already reloading\n", weapon->name);
        }
        return;
    }

    weapon->state = WEAPON_STATE_RELOAD;
    weapon->reloadTimer = weapon->reloadDuration;
    printf("%s: reloading...\n", weapon->name);
}

void weapon_finish_reload(Weapon *weapon) {
    if (weapon == NULL) return;

    if (weapon->ammoInMag < 0) weapon->ammoInMag = 0;
    if (weapon->reserveAmmo < 0) weapon->reserveAmmo = 0;

    int missingAmmo = weapon->magCapacity - weapon->ammoInMag;
    int ammoToLoad = weapon_min_int(missingAmmo, weapon->reserveAmmo);

    weapon->ammoInMag += ammoToLoad;
    weapon->reserveAmmo -= ammoToLoad;
    weapon->reloadTimer = 0.0;
    weapon->state = (weapon->ammoInMag > 0) ? WEAPON_STATE_READY : WEAPON_STATE_EMPTY;
    printf("%s: reload complete (%d/%d, reserve %d)\n", weapon->name, weapon->ammoInMag, weapon->magCapacity, weapon->reserveAmmo);
}

bool weapon_is_empty(Weapon *weapon) {
    return weapon->state == WEAPON_STATE_EMPTY;
}

void weapon_free(Weapon *weapon) {
    if (weapon == NULL) return;

    weapon->name = NULL;
    weapon->id = -1;
    weapon->damage = 0;
    weapon->ammoInMag = 0;
    weapon->magCapacity = 0;
    weapon->reserveAmmo = 0;
    weapon->state = WEAPON_STATE_EMPTY;
    weapon->type = WEAPON_TYPE_HITSCAN;
    weapon->fireCooldown = 0.0;
    weapon->fireTimer = 0.0;
    weapon->reloadDuration = 0.0;
    weapon->reloadTimer = 0.0;
    weapon->range = 0.0;
}
