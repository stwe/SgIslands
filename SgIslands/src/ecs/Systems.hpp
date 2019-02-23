// This file is part of the SgIslands package.
// 
// Filename: Systems.hpp
// Created:  21.02.2019
// Updated:  23.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

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
            entityx::ComponentHandle<AssetsComponent> assets;
            entityx::ComponentHandle<PositionComponent> position;
            entityx::ComponentHandle<TargetComponent> target;
            entityx::ComponentHandle<ActiveEntityComponent> activeEntity;

            for (auto entity : t_entities.entities_with_components(assets, position, target, activeEntity))
            {
                // get asset type
                const auto assetType{ m_assets.GetAssetMetaData(assets->actionAssetId).assetType };

                // run `FindPathToMapPosition()` if valid target position
                if (target->targetMapPosition.x >= 0 && target->targetMapPosition.y >= 0)
                {
                    // find path to target
                    const auto findNewPath{ m_astar.FindPathToMapPosition(
                        position->mapPosition,
                        target->targetMapPosition,
                        assetType,
                        target->pathToTarget
                    ) };

                    if (findNewPath)
                    {
                        target->nextWayPoint = 1;
                        target->onTheWay = true;
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
        explicit MovementSystem(iso::Assets& t_assets)
            : m_assets{ t_assets }
        {}

        void update(entityx::EntityManager& t_entities, entityx::EventManager& t_events, entityx::TimeDelta t_dt) override
        {
            entityx::ComponentHandle<PositionComponent> position;
            entityx::ComponentHandle<TargetComponent> target;
            entityx::ComponentHandle<ActiveEntityComponent> activeEntity;
            entityx::ComponentHandle<AssetsComponent> assets;
            entityx::ComponentHandle<DirectionComponent> direction;

            for (auto entity : t_entities.entities_with_components(position, target, activeEntity, assets, direction))
            {
                if (target->onTheWay)
                {
                    assert(target->nextWayPoint < target->pathToTarget.size());

                    // the next position ist the target
                    auto targetMapPosition = target->pathToTarget[target->nextWayPoint].position;

                    // calc target screen position
                    auto targetScreenPosition = iso::IsoMath::ToScreen(targetMapPosition, true);

                    // calc direction vector to the target
                    auto spriteScreenDirection = iso::VecMath::Direction(position->screenPosition, targetScreenPosition);

                    // calc the length to the target
                    target->lengthToTarget = iso::VecMath::Length(spriteScreenDirection);

                    // normalize the direction vector
                    direction->spriteScreenNormalDirection = spriteScreenDirection;
                    iso::VecMath::Normalize(direction->spriteScreenNormalDirection);

                    // only if it is a moving object: change the direction of the spit in the direction of movement
                    const auto assetType{ m_assets.GetAssetMetaData(assets->actionAssetId).assetType };
                    if (assetType != iso::AssetType::BUILDING)
                    {
                        direction->direction = iso::Assets::GetUnitDirectionByVec(direction->spriteScreenNormalDirection);
                    }

                    if (target->lengthToTarget > 1.0f)
                    {
                        position->screenPosition.x += direction->spriteScreenNormalDirection.x;
                        position->screenPosition.y += direction->spriteScreenNormalDirection.y;
                    }

                    if (target->lengthToTarget <= 1.0f)
                    {
                        position->mapPosition = target->targetMapPosition;
                        target->nextWayPoint++;
                    }

                    if (target->nextWayPoint == target->pathToTarget.size())
                    {
                        target->onTheWay = false;
                    }
                }
            }
        }

    protected:

    private:
        iso::Assets& m_assets;
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

        void update(entityx::EntityManager& t_entities, entityx::EventManager& t_events, entityx::TimeDelta t_dt) override
        {
            entityx::ComponentHandle<AssetsComponent> assets;

            for (auto entity : t_entities.entities_with_components(assets))
            {
                const auto assetType{ m_assets.GetAssetMetaData(assets->actionAssetId).assetType };

                if (assetType == iso::AssetType::BUILDING)
                {
                    for (const auto& direction : iso::Assets::BUILDING_DIRECTIONS)
                    {
                        // update action animation
                        auto& actionAnimation{ m_assets.GetAnimation(assets->actionAssetId, direction) };
                        actionAnimation.Update(sf::seconds(t_dt));

                        // update idle animation (which have only one frame)
                        auto& idleAnimation{ m_assets.GetAnimation(assets->idleAssetId, direction) };
                        idleAnimation.Update(sf::seconds(t_dt));
                    }
                }
                else
                {
                    for (const auto& direction : iso::Assets::UNIT_DIRECTIONS)
                    {
                        // update action animation
                        auto& actionAnimation{ m_assets.GetAnimation(assets->actionAssetId, direction) };
                        actionAnimation.Update(sf::seconds(t_dt));

                        // update idle animation (which have only one frame)
                        auto& idleAnimation{ m_assets.GetAnimation(assets->idleAssetId, direction) };
                        idleAnimation.Update(sf::seconds(t_dt));
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
        RenderSystem(sf::RenderWindow& t_window, iso::Assets& t_assets, iso::TileAtlas& t_tileAtlas)
            : m_window{ t_window }
            , m_assets{ t_assets }
            , m_tileAtlas{ t_tileAtlas }
        {}

        void update(entityx::EntityManager& t_entities, entityx::EventManager& t_events, entityx::TimeDelta t_dt) override
        {
            sf::Sprite* sprite;
            iso::Animation* animation;

            entityx::ComponentHandle<PositionComponent> position;
            entityx::ComponentHandle<TargetComponent> target;
            entityx::ComponentHandle<AssetsComponent> assets;
            entityx::ComponentHandle<DirectionComponent> direction;

            for (auto entity : t_entities.entities_with_components(position, target, assets, direction))
            {
                if (target->onTheWay)
                {
                    animation = &m_assets.GetAnimation(assets->actionAssetId, direction->direction);
                    sprite = &animation->GetSprite();
                }
                else
                {
                    animation = &m_assets.GetAnimation(assets->idleAssetId, direction->direction);
                    sprite = &animation->GetSprite();
                }

                // get asset type
                const auto assetType{ m_assets.GetAssetMetaData(assets->actionAssetId).assetType };

                // get tile width
                const auto tileWidth{ m_assets.GetAssetMetaData(assets->actionAssetId).tileWidth };

                // get tile height
                const auto tileHeight{ m_assets.GetAssetMetaData(assets->actionAssetId).tileHeight };

                if (assetType == iso::AssetType::BUILDING)
                {
                    // support only for building sizes of 2x4 tiles
                    assert(tileWidth == 2);
                    assert(tileHeight == 4);

                    const auto x{ tileWidth * iso::IsoMath::DEFAULT_TILE_WIDTH_HALF };
                    const auto y{ tileHeight * iso::IsoMath::DEFAULT_TILE_HEIGHT_HALF };

                    sprite->setOrigin(x, y);
                    sprite->setPosition(position->screenPosition);
                }
                else if (assetType == iso::AssetType::LAND_UNIT)
                {
                    // support only for land unit sizes of 1x1 tiles
                    assert(tileWidth == 1);
                    assert(tileHeight == 1);

                    sprite->setOrigin(iso::IsoMath::DEFAULT_TILE_WIDTH_QUARTER, iso::IsoMath::DEFAULT_TILE_HEIGHT_HALF);
                    sprite->setPosition(position->screenPosition);
                }
                else if (assetType == iso::AssetType::WATER_UNIT)
                {
                    // support only for water unit sizes of 3x3 tiles
                    assert(tileWidth == 3);
                    assert(tileHeight == 3);

                    const auto localBounds{ sprite->getLocalBounds() };
                    //auto rect{ sf::RectangleShape(sf::Vector2f(localBounds.width, localBounds.height)) };

                    // copy screen position
                    auto drawPosition{ position->screenPosition };

                    sprite->setOrigin(localBounds.width / 2, localBounds.height);
                    //rect.setOrigin(localBounds.width / 2, localBounds.height);

                    drawPosition.y += iso::IsoMath::DEFAULT_TILE_HEIGHT * 2;

                    sprite->setPosition(drawPosition);
                    //rect.setPosition(drawPosition);
                    //rect.setFillColor(sf::Color(0, 0, 128, 64));
                    //t_window.draw(rect);
                }

                // draw path to target if exist
                if (!target->pathToTarget.empty())
                {
                    for (const auto& node : target->pathToTarget)
                    {
                        m_tileAtlas.DrawMiscTile(iso::TileAtlas::GRID_TILE, node.position.x, node.position.y, m_window);
                    }
                }

                m_window.draw(*sprite);
            }
        }

    protected:

    private:
        sf::RenderWindow& m_window;
        iso::Assets& m_assets;
        iso::TileAtlas& m_tileAtlas;
    };
}
