// This file is part of the SgIslands package.
// 
// Filename: Systems.hpp
// Created:  21.02.2019
// Updated:  09.03.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <entityx/System.h>
#include "Components.hpp"
#include "../iso/VecMath.hpp"

namespace sg::islands::ecs
{
    //-------------------------------------------------
    // FindPath
    //-------------------------------------------------

    class FindPathSystem : public entityx::System<FindPathSystem>
    {
    public:
        FindPathSystem(iso::Assets& t_assets, iso::Astar& t_astar)
            : m_assets{ t_assets }
            , m_astar{ t_astar }
        {}

        void update(entityx::EntityManager& t_entities, entityx::EventManager& t_events, entityx::TimeDelta t_dt) override
        {
            entityx::ComponentHandle<AssetComponent> assetComponent;
            entityx::ComponentHandle<PositionComponent> positionComponent;
            entityx::ComponentHandle<TargetComponent> targetComponent;
            entityx::ComponentHandle<ActiveEntityComponent> activeEntityComponent;

            for (auto entity : t_entities.entities_with_components(assetComponent, positionComponent, targetComponent, activeEntityComponent))
            {
                // get asset type
                assert(assetComponent->assetId >= 0);
                const auto assetType{ m_assets.GetAsset(assetComponent->assetId).assetType };

                // run `FindPathToMapPosition()` if valid target position
                if (targetComponent->targetMapPosition.x >= 0 && targetComponent->targetMapPosition.y >= 0)
                {
                    // find path to target
                    const auto findNewPath{ m_astar.FindPathToMapPosition(
                        positionComponent->mapPosition,
                        targetComponent->targetMapPosition,
                        assetType,
                        targetComponent->pathToTarget
                    ) };

                    if (findNewPath)
                    {
                        targetComponent->nextWayPoint = 1;
                        targetComponent->onTheWay = true;
                    }
                }
            }
        }

    protected:

    private:
        iso::Assets& m_assets;
        iso::Astar& m_astar;
    };

    //-------------------------------------------------
    // Movement
    //-------------------------------------------------

    class MovementSystem : public entityx::System<MovementSystem>
    {
    public:
        explicit MovementSystem(iso::Assets& t_assets, iso::Map& t_map)
            : m_assets{ t_assets }
            , m_map{ t_map } // todo
        {}

        void update(entityx::EntityManager& t_entities, entityx::EventManager& t_events, entityx::TimeDelta t_dt) override
        {
            entityx::ComponentHandle<PositionComponent> positionComponent;
            entityx::ComponentHandle<TargetComponent> targetComponent;
            entityx::ComponentHandle<ActiveEntityComponent> activeEntityComponent;
            entityx::ComponentHandle<AssetComponent> assetComponent;
            entityx::ComponentHandle<DirectionComponent> directionComponent;

            for (auto entity : t_entities.entities_with_components(positionComponent, targetComponent, activeEntityComponent, assetComponent, directionComponent))
            {
                if (targetComponent->onTheWay)
                {
                    assert(targetComponent->nextWayPoint < targetComponent->pathToTarget.size());

                    // the next position ist the target
                    auto nextTargetMapPosition{ targetComponent->pathToTarget[targetComponent->nextWayPoint].position };

                    // calc target screen position
                    auto targetScreenPosition{ iso::IsoMath::ToScreen(nextTargetMapPosition, true) };

                    // calc direction vector to the target
                    auto spriteScreenDirection{ iso::VecMath::Direction(positionComponent->screenPosition, targetScreenPosition) };

                    // calc the length to the target
                    targetComponent->lengthToTarget = iso::VecMath::Length(spriteScreenDirection);

                    // normalize the direction vector
                    directionComponent->spriteScreenNormalDirection = spriteScreenDirection;
                    iso::VecMath::Normalize(directionComponent->spriteScreenNormalDirection);

                    // only if it is a moving object: change the direction of the sprite in the direction of movement
                    assert(assetComponent->assetId >= 0);
                    const auto assetType{ m_assets.GetAsset(assetComponent->assetId).assetType };
                    if (assetType != iso::AssetType::BUILDING)
                    {
                        directionComponent->direction = iso::Assets::GetUnitDirectionByVec(directionComponent->spriteScreenNormalDirection);
                    }

                    if (targetComponent->lengthToTarget > 1.0f)
                    {
                        positionComponent->screenPosition.x += directionComponent->spriteScreenNormalDirection.x;
                        positionComponent->screenPosition.y += directionComponent->spriteScreenNormalDirection.y;
                    }

                    if (targetComponent->lengthToTarget <= 1.0f)
                    {
                        positionComponent->mapPosition = targetComponent->targetMapPosition;
                        targetComponent->nextWayPoint++;
                    }

                    if (targetComponent->nextWayPoint == targetComponent->pathToTarget.size())
                    {
                        targetComponent->onTheWay = false;
                    }
                }
            }
        }

    protected:

    private:
        iso::Assets& m_assets;
        iso::Map& m_map;
    };

    //-------------------------------------------------
    // Animation
    //-------------------------------------------------

    class AnimationSystem : public entityx::System<AnimationSystem>
    {
    public:
        explicit AnimationSystem(iso::Assets& t_assets)
            : m_assets{ t_assets }
        {}

        void update(entityx::EntityManager& t_entities, entityx::EventManager& t_events, entityx::TimeDelta) override
        {
            entityx::ComponentHandle<AssetComponent> assetComponent;

            for (auto entity : t_entities.entities_with_components(assetComponent))
            {
                assert(assetComponent->assetId >= 0);
                const auto& asset{ m_assets.GetAsset(assetComponent->assetId) };
                const auto assetType{ asset.assetType };

                if (assetType == iso::AssetType::BUILDING)
                {
                    for (const auto& direction : iso::BUILDING_DIRECTIONS)
                    {
                        // update action animation
                        auto& actionAnimation{ m_assets.GetAnimation(asset.assetId, "Work", direction) };
                        actionAnimation.Update(core::SF_TIME_PER_FRAME);

                        // update idle animation (which have only one frame)
                        auto& idleAnimation{ m_assets.GetAnimation(asset.assetId, "Idle", direction) };
                        idleAnimation.Update(core::SF_TIME_PER_FRAME);
                    }
                }
                else
                {
                    for (const auto& direction : iso::UNIT_DIRECTIONS)
                    {
                        // update action animation
                        auto& actionAnimation{ m_assets.GetAnimation(asset.assetId, "Move", direction) };
                        actionAnimation.Update(core::SF_TIME_PER_FRAME);

                        // update idle animation (which have only one frame)
                        auto& idleAnimation{ m_assets.GetAnimation(asset.assetId, "Idle", direction) };
                        idleAnimation.Update(core::SF_TIME_PER_FRAME);
                    }
                }
            }
        }

    protected:

    private:
        iso::Assets& m_assets;
    };

    //-------------------------------------------------
    // Render
    //-------------------------------------------------

    class RenderSystem : public entityx::System<RenderSystem>
    {
    public:
        RenderSystem(
            sf::RenderWindow& t_window,
            iso::Assets& t_assets,
            iso::TileAtlas& t_tileAtlas,
            iso::Map& t_map
        )
            : m_window{ t_window }
            , m_assets{ t_assets }
            , m_tileAtlas{ t_tileAtlas }
            , m_map{ t_map }
        {}

        void update(entityx::EntityManager& t_entities, entityx::EventManager& t_events, entityx::TimeDelta t_dt) override
        {
            sf::Sprite* sprite;
            iso::Animation* animation;

            entityx::ComponentHandle<PositionComponent> positionComponent;
            entityx::ComponentHandle<TargetComponent> targetComponent;
            entityx::ComponentHandle<AssetComponent> assetComponent;
            entityx::ComponentHandle<DirectionComponent> directionComponent;
            entityx::ComponentHandle<RenderComponent> renderComponent;

            for (auto entity : t_entities.entities_with_components(positionComponent, targetComponent, assetComponent, directionComponent, renderComponent))
            {
                assert(assetComponent->assetId >= 0);
                const auto& asset{ m_assets.GetAsset(assetComponent->assetId) };
                const auto assetType{ asset.assetType };

                // set animation
                if (assetType == iso::AssetType::BUILDING)
                {
                    // idle animation
                    animation = &m_assets.GetAnimation(asset.assetId, "Idle", directionComponent->direction);
                    sprite = &animation->GetSprite();
                }
                else
                {
                    if (targetComponent->onTheWay)
                    {
                        // action animation
                        animation = &m_assets.GetAnimation(asset.assetId, "Move", directionComponent->direction);
                        sprite = &animation->GetSprite();
                    }
                    else
                    {
                        // idle animaton
                        animation = &m_assets.GetAnimation(asset.assetId, "Idle", directionComponent->direction);
                        sprite = &animation->GetSprite();
                    }
                }

                // get tile width
                const auto tileWidth{ asset.tileWidth };

                // get tile height
                const auto tileHeight{ asset.tileHeight };

                if (assetType == iso::AssetType::BUILDING)
                {
                    // support only for building sizes of 2x4 tiles
                    assert(tileWidth == 2);
                    assert(tileHeight == 4);

                    const auto x{ tileWidth * iso::IsoMath::DEFAULT_TILE_WIDTH_HALF };
                    const auto y{ tileHeight * iso::IsoMath::DEFAULT_TILE_HEIGHT_HALF };

                    sprite->setOrigin(x, y);
                    sprite->setPosition(positionComponent->screenPosition);
                }
                else if (assetType == iso::AssetType::LAND_UNIT)
                {
                    // support only for land unit sizes of 1x1 tiles
                    assert(tileWidth == 1);
                    assert(tileHeight == 1);

                    sprite->setOrigin(iso::IsoMath::DEFAULT_TILE_WIDTH_QUARTER, iso::IsoMath::DEFAULT_TILE_HEIGHT_HALF);
                    sprite->setPosition(positionComponent->screenPosition);
                }
                else if (assetType == iso::AssetType::WATER_UNIT && tileWidth == 3 && tileHeight == 3)
                {
                    const auto localBounds{ sprite->getLocalBounds() };
                    //auto rect{ sf::RectangleShape(sf::Vector2f(localBounds.width, localBounds.height)) };

                    // copy screen position
                    auto drawPosition{ positionComponent->screenPosition };

                    sprite->setOrigin(localBounds.width / 2, localBounds.height);
                    //rect.setOrigin(localBounds.width / 2, localBounds.height);

                    drawPosition.y += iso::IsoMath::DEFAULT_TILE_HEIGHT * 2;

                    sprite->setPosition(drawPosition);
                    //rect.setPosition(drawPosition);
                    //rect.setFillColor(sf::Color(0, 0, 128, 64));
                    //m_window.draw(rect);
                }
                else if (assetType == iso::AssetType::WATER_UNIT && tileWidth == 1 && tileHeight == 1)
                {
                    const auto localBounds{ sprite->getLocalBounds() };
                    //auto rect{ sf::RectangleShape(sf::Vector2f(localBounds.width, localBounds.height)) };

                    // copy screen position
                    auto drawPosition{ positionComponent->screenPosition };

                    sprite->setOrigin(localBounds.width / 2, localBounds.height);
                    //rect.setOrigin(localBounds.width / 2, localBounds.height);

                    drawPosition.y += iso::IsoMath::DEFAULT_TILE_HEIGHT;

                    sprite->setPosition(drawPosition);
                    //rect.setPosition(drawPosition);
                    //rect.setFillColor(sf::Color(0, 0, 128, 64));
                    //m_window.draw(rect);
                }

                // draw path to target if exist
                if (!targetComponent->pathToTarget.empty())
                {
                    for (const auto& node : targetComponent->pathToTarget)
                    {
                        m_tileAtlas.DrawMiscTile(iso::TileAtlas::GRID_TILE, node.position.x, node.position.y, m_window);
                    }
                }

                // draw sprite
                if (renderComponent->render)
                {
                    m_window.draw(*sprite);
                }
            }
        }

    protected:

    private:
        sf::RenderWindow& m_window;
        iso::Assets& m_assets;
        iso::TileAtlas& m_tileAtlas;
        iso::Map& m_map;
    };
}
