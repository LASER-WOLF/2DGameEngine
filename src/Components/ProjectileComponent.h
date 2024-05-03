#ifndef PROJECTILECOMPONENT_H
#define PROJECTILECOMPONENT_H

#include <SDL2/SDL.h>

struct ProjectileComponent {
    int duration;
    int hitPercentDamage;
    bool isFriendly;
    int startTime;

    ProjectileComponent(bool isFriendly = false, int hitPercentDamage = 0, int duration = 0) {
        this->isFriendly = isFriendly;
        this->duration = duration;
        this->hitPercentDamage = hitPercentDamage;
        this->startTime = SDL_GetTicks();
    }
};

#endif
