
#ifndef RENDERHEALTHSYSTEM_H
#define RENDERHEALTHSYSTEM_H

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../Components/HealthComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include <SDL2/SDL.h>

class RenderHealthSystem: public System {
    public:
        RenderHealthSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<HealthComponent>();
            RequireComponent<SpriteComponent>();
        }

        void Update(SDL_Renderer* renderer, const std::unique_ptr<AssetStore>& assetStore, const SDL_Rect& camera) {
            for (auto entity: GetSystemEntities()) {

                const auto transform = entity.GetComponent<TransformComponent>();
                const auto health = entity.GetComponent<HealthComponent>();
                int healthPercentage = health.healthPercentage;

                SDL_Color color = {0, 255, 0};
                if (healthPercentage <= 30) {
                    color = {255, 0, 0};
                } else if (healthPercentage <= 60) {
                    color = {255, 255, 0};
                }

                SDL_Surface* surface = TTF_RenderText_Blended(
                    assetStore->GetFont("charriot-font-small"), 
                    ("HP: " + std::to_string(healthPercentage) + "%").c_str(),
                    color
                );
                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);

                int labelWidth = 0;
                int labelHeight = 0;

                SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);

                int offsetX = 40;
                int offsetY = 0;
                int boxWidthFull = 50;
                int boxWidth = healthPercentage / 2;
                int boxHeight = 6;

                SDL_Rect dstRect = {
                    static_cast<int>(transform.position.x + offsetX - camera.x),
                    static_cast<int>(transform.position.y + offsetY - camera.y),
                    labelWidth,
                    labelHeight
                };

                SDL_RenderCopy(renderer, texture, NULL, &dstRect);

                SDL_DestroyTexture(texture);
                
                SDL_Rect hpRect = {
                    static_cast<int>(transform.position.x + offsetX - camera.x),
                    static_cast<int>(transform.position.y + offsetY + 13 - camera.y),
                    static_cast<int>(boxWidth),
                    static_cast<int>(boxHeight)
                };
                SDL_Rect hpOutlineRect = {
                    static_cast<int>(transform.position.x + offsetX - camera.x),
                    static_cast<int>(transform.position.y + offsetY + 13 - camera.y),
                    static_cast<int>(boxWidthFull),
                    static_cast<int>(boxHeight)
                };
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
                SDL_RenderFillRect(renderer, &hpRect);
                SDL_RenderDrawRect(renderer, &hpOutlineRect);
            }
        }
};

#endif
