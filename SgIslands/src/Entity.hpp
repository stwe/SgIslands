// This file is part of the SgIslands package.
// 
// Filename: Entity.hpp
// Created:  27.01.2019
// Updated:  18.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

//#include <SFML/Graphics/RectangleShape.hpp>
#include <memory>
#include <array>
#include "iso/VecMath.hpp"
#include "iso/Assets.hpp"
#include "iso/Astar.hpp"

namespace sg::islands
{
    class Entity
    {
    public:
        //-------------------------------------------------
        // Ctor. && Dtor.
        //-------------------------------------------------

        Entity() = delete;

        Entity(
            iso::TileAtlas& t_tileAtlas,
            const iso::AssetId t_assetId,
            const iso::AssetId t_assetIdIdle,
            const sf::Vector2i& t_mapPosition,
            iso::Map& t_map
        )
            : m_tileAtlas{ t_tileAtlas }
            , m_assetId{ t_assetId }
            , m_assetIdIdle{ t_assetIdIdle }
            , m_mapPosition{ t_mapPosition }
        {
            m_currentScreenPosition = iso::IsoMath::ToScreen(m_mapPosition, true);

            // todo tmp code
            m_astar = std::make_unique<iso::Astar>(t_map);
        }

        Entity(const Entity& t_other) = delete;
        Entity(Entity&& t_other) noexcept = delete;
        Entity& operator=(const Entity& t_other) = delete;
        Entity& operator=(Entity&& t_other) noexcept = delete;

        ~Entity() noexcept = default;

        //-------------------------------------------------
        // Game Logic
        //-------------------------------------------------

        void HandleInput(sf::RenderWindow& t_window, const sf::Event& t_event, iso::Map& t_map, const iso::Assets& t_assets)
        {
            if (t_event.type == sf::Event::KeyPressed)
            {

            }

            if (t_event.type == sf::Event::MouseButtonPressed)
            {
                if (t_event.mouseButton.button == sf::Mouse::Left)
                {
                    SG_ISLANDS_DEBUG("Left Mouse pressed.");

                    m_isMove = FindPathToMouse(t_window, t_assets);
                }
            }
        }

        void UpdateAnimations(iso::Assets& t_assets, const sf::Time& t_dt)
        {
            const auto assetType{ t_assets.GetAssetMetaData(m_assetId).assetType };

            if (assetType == iso::AssetType::BUILDING)
            {
                HandleAssetUpdate(iso::Assets::BUILDING_DIRECTIONS, t_assets, t_dt);
            }
            else
            {
                HandleAssetUpdate(iso::Assets::UNIT_DIRECTIONS, t_assets, t_dt);
            }

            if (m_isMove)
            {
                SetNextWayPoint(t_assets, m_wayPoint);

                if (m_lengthToTarget > 1.0f)
                {
                    m_currentScreenPosition.x += m_spriteScreenNormalDirection.x;
                    m_currentScreenPosition.y += m_spriteScreenNormalDirection.y;
                }

                if (m_lengthToTarget <= 1.0f)
                {
                    m_mapPosition = m_targetMapPosition;
                    m_wayPoint++;
                }

                if (m_wayPoint == m_path.size())
                {
                    m_isMove = false;
                }
            }
        }

        void Draw(iso::Assets& t_assets, sf::RenderWindow& t_window)
        {
            sf::Sprite* sprite;
            iso::Animation* animation;

            if (m_isMove)
            {
                animation = &t_assets.GetAnimation(m_assetId, m_direction);
                sprite = &animation->GetSprite();
            }
            else
            {
                animation = &t_assets.GetAnimation(m_assetIdIdle, m_direction);
                sprite = &animation->GetSprite();
            }

            // get asset type
            const auto assetType{ t_assets.GetAssetMetaData(m_assetId).assetType };

            // get tile width
            const auto tileWidth{ t_assets.GetAssetMetaData(m_assetId).tileWidth };

            // get tile height
            const auto tileHeight{ t_assets.GetAssetMetaData(m_assetId).tileHeight };

            if (assetType == iso::AssetType::BUILDING)
            {
                // support only for building sizes of 2x4 tiles
                assert(tileWidth == 2);
                assert(tileHeight == 4);

                const auto x{ tileWidth * iso::IsoMath::DEFAULT_TILE_WIDTH_HALF };
                const auto y{ tileHeight * iso::IsoMath::DEFAULT_TILE_HEIGHT_HALF };

                sprite->setOrigin(x, y);
                sprite->setPosition(m_currentScreenPosition);
            }
            else if (assetType == iso::AssetType::LAND_UNIT)
            {
                // support only for land unit sizes of 1x1 tiles
                assert(tileWidth == 1);
                assert(tileHeight == 1);

                sprite->setOrigin(iso::IsoMath::DEFAULT_TILE_WIDTH_QUARTER, iso::IsoMath::DEFAULT_TILE_HEIGHT_HALF);
                sprite->setPosition(m_currentScreenPosition);
            }
            else if (assetType == iso::AssetType::WATER_UNIT)
            {
                // support only for water unit sizes of 3x3 tiles
                assert(tileWidth == 3);
                assert(tileHeight == 3);

                const auto localBounds{ sprite->getLocalBounds() };
                //auto rect{ sf::RectangleShape(sf::Vector2f(localBounds.width, localBounds.height)) };

                // copy screen position
                auto drawPosition{ m_currentScreenPosition };

                sprite->setOrigin(localBounds.width / 2, localBounds.height);
                //rect.setOrigin(localBounds.width / 2, localBounds.height);

                drawPosition.y += iso::IsoMath::DEFAULT_TILE_HEIGHT * 2;

                sprite->setPosition(drawPosition);
                //rect.setPosition(drawPosition);
                //rect.setFillColor(sf::Color(0, 0, 128, 64));
                //t_window.draw(rect);
            }

            // todo tmp code
            for (const auto& n : m_path)
            {
                m_tileAtlas.DrawMiscTile(iso::TileAtlas::GRID_TILE, n.position.x, n.position.y, t_window);
            }

            t_window.draw(*sprite);
        }

        void SetRenderActive(const bool t_renderActiveGraphic) { m_renderActiveGraphic = t_renderActiveGraphic; }

        auto GetMapPosition() const { return m_mapPosition; }
        auto GetRenderActive() const { return m_renderActiveGraphic; }

        void SetMapPosition(const int t_x, const int t_y)
        {
            m_mapPosition.x = t_x;
            m_mapPosition.y = t_y;

            m_currentScreenPosition = iso::IsoMath::ToScreen(m_mapPosition, true);
        }

    protected:

    private:
        iso::TileAtlas& m_tileAtlas;
        std::vector<iso::Node> m_path;

        iso::AssetId m_assetId{ -1 };
        iso::AssetId m_assetIdIdle{ -1 };

        sf::Vector2i m_mapPosition{ -1, -1 };
        sf::Vector2f m_currentScreenPosition{ -1.0f, -1.0f };

        sf::Vector2i m_targetMapPosition{ -1, -1 };
        sf::Vector2f m_targetScreenPosition{ -1.0f, -1.0f };

        sf::Vector2f m_spriteScreenDirection{ -1.0f, -1.0f };
        sf::Vector2f m_spriteScreenNormalDirection{ -1.0f, -1.0f };

        float m_lengthToTarget{ -1.0f };

        bool m_isMove{ false };
        std::size_t m_wayPoint{ 0 };

        bool m_renderActiveGraphic{ false };

        iso::Assets::Direction m_direction{ iso::Assets::DEFAULT_DIRECTION };

        std::unique_ptr<iso::Astar> m_astar;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        bool FindPathToMouse(sf::RenderWindow& t_window, const iso::Assets& t_assets)
        {
            // get mouse position
            const auto mousePosition{ sf::Mouse::getPosition(t_window) };
            const auto targetPosition{ t_window.mapPixelToCoords(mousePosition) };

            // get map position of the mouse
            const auto targetMapPosition{ iso::IsoMath::ToMap(targetPosition) };
            SG_ISLANDS_DEBUG("target map x: {}", targetMapPosition.x);
            SG_ISLANDS_DEBUG("target map y: {}", targetMapPosition.y);

            // find full path to mouse map position
            iso::Node node;
            iso::Node endNode;

            node.position = m_mapPosition;
            endNode.position = targetMapPosition;

            auto& animation{ t_assets.GetAnimation(m_assetId, m_direction) };
            m_path = m_astar->FindPath(node, endNode, animation.GetAssetMetaData().assetType);

            if (m_path.empty())
            {
                return false;
            }

            SG_ISLANDS_DEBUG("path size: {}", m_path.size());

            m_wayPoint = 1;

            return true;
        }

        void SetNextWayPoint(const iso::Assets& t_assets, const std::size_t t_pathIndex)
        {
            assert(t_pathIndex < m_path.size());

            // the next position ist the target
            m_targetMapPosition = m_path[t_pathIndex].position;

            // calc target screen position
            m_targetScreenPosition = iso::IsoMath::ToScreen(m_targetMapPosition, true);

            // calc direction vector to the target
            m_spriteScreenDirection = iso::VecMath::Direction(m_currentScreenPosition, m_targetScreenPosition);

            // calc the length to the target
            m_lengthToTarget = iso::VecMath::Length(m_spriteScreenDirection);

            // normalize the direction vector
            m_spriteScreenNormalDirection = m_spriteScreenDirection;
            iso::VecMath::Normalize(m_spriteScreenNormalDirection);

            // only if it is a moving object: change the direction of the spit in the direction of movement
            const auto assetType{ t_assets.GetAssetMetaData(m_assetId).assetType };
            if (assetType != iso::AssetType::BUILDING)
            {
                m_direction = iso::Assets::GetUnitDirectionByVec(m_spriteScreenNormalDirection);
            }
        }

        template <std::size_t N>
        void HandleAssetUpdate(
            const std::array<iso::Assets::Direction, N>& t_directions,
            iso::Assets& t_assets,
            const sf::Time& t_dt
        )
        {
            for (const auto& direction : t_directions)
            {
                // update work or move animation
                auto& animation{ t_assets.GetAnimation(m_assetId, direction) };
                animation.Update(t_dt);

                // update idle animation (which have only one frame)
                auto& animationIdle{ t_assets.GetAnimation(m_assetIdIdle, direction) };
                animationIdle.Update(t_dt);
            }
        }
    };
}
