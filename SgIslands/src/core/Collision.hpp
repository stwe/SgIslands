// This file is part of the SgIslands package.
// 
// Filename: Collision.hpp
// Created:  06.03.2019
// Updated:  16.03.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include "BitmaskManager.hpp"
#include "../iso/Assets.hpp"
#include "../ecs/Components.hpp"
#include "../ecs/Events.hpp"

namespace sg::islands::core
{
    class Collision
    {
    public:
        static constexpr sf::Uint8 DEFAULT_ALPHA_LIMIT{ 100 };

        /**
         * @brief Test for a collision between two sprites by comparing the alpha values of overlapping pixels.
         *        Supports scaling and rotation.
         * @param t_sprite1 A Sprite.
         * @param t_sprite2 Other Sprite.
         * @param t_alphaLimit The threshold at which a pixel becomes "solid". If AlphaLimit is 127, a pixel with
         *                     alpha value 128 will cause a collision and a pixel with alpha value 126 will not.
         * @param t_bitmaskManager A reference to the BitmaskManager.
         * @return bool
         */
        static auto PixelPerfect(
            const sf::Sprite& t_sprite1,
            const sf::Sprite& t_sprite2,
            const sf::Uint8 t_alphaLimit,
            BitmaskManager& t_bitmaskManager
        )
        {
            sf::FloatRect intersection;

            if (t_sprite1.getGlobalBounds().intersects(t_sprite2.getGlobalBounds(), intersection))
            {
                const auto& o1SubRect{ t_sprite1.getTextureRect() };
                const auto& o2SubRect{ t_sprite2.getTextureRect() };

                const auto& mask1{ t_bitmaskManager.GetBitmask(t_sprite1.getTexture()) };
                const auto& mask2{ t_bitmaskManager.GetBitmask(t_sprite2.getTexture()) };

                const auto il{ static_cast<int>(intersection.left) };
                const auto iw{ static_cast<int>(intersection.width) };

                const auto it{ static_cast<int>(intersection.top) };
                const auto ih{ static_cast<int>(intersection.height) };

                /*
                t_rect = sf::RectangleShape(sf::Vector2f(intersection.width, intersection.height));
                t_rect.setPosition(intersection.left, intersection.top);
                t_rect.setFillColor(sf::Color(0, 0, 128, 64));
                */
 
                for (auto i{ il }; i < il + iw; ++i)
                {
                    for (auto j{ it }; j < it + ih; ++j)
                    {
                        const auto o1V{ t_sprite1.getInverseTransform().transformPoint(static_cast<float>(i), static_cast<float>(j)) };
                        const auto o2V{ t_sprite2.getInverseTransform().transformPoint(static_cast<float>(i), static_cast<float>(j)) };

                        if (o1V.x > 0 && o1V.y > 0 && o2V.x > 0 && o2V.y > 0 &&
                            o1V.x < o1SubRect.width && o1V.y < o1SubRect.height &&
                            o2V.x < o2SubRect.width && o2V.y < o2SubRect.height)
                        {
                            if (BitmaskManager::GetPixel(mask1, t_sprite1.getTexture(), static_cast<int>(o1V.x) + o1SubRect.left, static_cast<int>(o1V.y) + o1SubRect.top) > t_alphaLimit &&
                                BitmaskManager::GetPixel(mask2, t_sprite2.getTexture(), static_cast<int>(o2V.x) + o2SubRect.left, static_cast<int>(o2V.y) + o2SubRect.top) > t_alphaLimit)
                            {
                                return true;
                            }
                        }
                    }
                }
            }

            return false;
        }

        /**
         * @brief Checks if the active sprite collides with another type of `WATER_UNIT`.
         * @param t_entities Reference to the EntityX EntityManager.
         * @param t_events Reference to the EntityX EventManager.
         * @param t_assets Reference to the game Assets.
         * @param t_sprite The active sprite.
         * @param t_entityId The Id of the active entity.
         * @param t_assetId The asset Id of the active sprite.
         * @param t_bitmaskManager Reference to the BitmaskManager.
         * @return bool
         */
        static bool CheckWithOtherWaterUnits(
            entityx::EntityManager& t_entities,
            entityx::EventManager& t_events,
            iso::Assets& t_assets,
            sf::Sprite& t_sprite,
            const entityx::Entity::Id t_entityId,
            const iso::AssetId t_assetId,
            BitmaskManager& t_bitmaskManager
        )
        {
            entityx::ComponentHandle<ecs::AssetComponent> otherAssetComponent;
            entityx::ComponentHandle<ecs::DirectionComponent> otherDirectionComponent;
            entityx::ComponentHandle<ecs::WaterUnitComponent> otherWaterUnitComponent;

            // for each entity
            for (auto otherEntity : t_entities.entities_with_components(otherAssetComponent, otherDirectionComponent, otherWaterUnitComponent))
            {
                // get sprite
                const auto otherAssetId{ otherAssetComponent->assetId };

                if (otherAssetId != t_assetId)
                {
                    const auto& otherAnimation{ t_assets.GetAnimation(otherAssetId, "Idle", otherDirectionComponent->direction) };
                    const auto& otherSprite{ otherAnimation.GetSprite() };

                    // check for collision
                    if (PixelPerfect(t_sprite, otherSprite, DEFAULT_ALPHA_LIMIT, t_bitmaskManager))
                    {
                        t_events.emit<ecs::CollisionEvent>(t_entityId, otherEntity.id());

                        return true;
                    }
                }
            }

            return false;
        }

        /**
         * @brief Checks if the active sprite collides with another type of `BUILDING`.
         * @param t_entities Reference to the EntityX EntityManager.
         * @param t_events Reference to the EntityX EventManager.
         * @param t_assets Reference to the game Assets.
         * @param t_sprite The active sprite.
         * @param t_entityId The Id of the active entity.
         * @param t_assetId The asset Id of the active sprite.
         * @param t_bitmaskManager Reference to the BitmaskManager.
         * @return bool
         */
        static bool CheckWithBuildings(
            entityx::EntityManager& t_entities,
            entityx::EventManager& t_events,
            iso::Assets& t_assets,
            sf::Sprite& t_sprite,
            const entityx::Entity::Id t_entityId,
            const iso::AssetId t_assetId,
            BitmaskManager& t_bitmaskManager
        )
        {
            entityx::ComponentHandle<ecs::AssetComponent> otherAssetComponent;
            entityx::ComponentHandle<ecs::DirectionComponent> otherDirectionComponent;
            entityx::ComponentHandle<ecs::BuildingComponent> buildingComponent;

            // for each entity
            for (auto otherEntity : t_entities.entities_with_components(otherAssetComponent, otherDirectionComponent, buildingComponent))
            {
                // get sprite
                const auto otherAssetId{ otherAssetComponent->assetId };

                if (otherAssetId != t_assetId)
                {
                    const auto& otherAnimation{ t_assets.GetAnimation(otherAssetId, "Idle", otherDirectionComponent->direction) };
                    const auto& otherSprite{ otherAnimation.GetSprite() };

                    // check for collision
                    if (PixelPerfect(t_sprite, otherSprite, DEFAULT_ALPHA_LIMIT, t_bitmaskManager))
                    {
                        t_events.emit<ecs::CollisionEvent>(t_entityId, otherEntity.id());

                        return true;
                    }
                }
            }

            return false;
        }

    protected:

    private:

    };
}
