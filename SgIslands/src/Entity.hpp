// This file is part of the SgIslands package.
// 
// Filename: Entity.hpp
// Created:  27.01.2019
// Updated:  08.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <memory>
#include "iso/VecMath.hpp"
#include "iso/Unit.hpp"
#include "iso/Astar.hpp"

namespace sg::islands
{
    class Entity
    {
    public:
        static constexpr auto SHIP_TILE_WIDTH_HALF{ 112.0f };
        static constexpr auto SHIP_TILE_HEIGHT_HALF{ 112.0f };
        static constexpr auto SHIP_TILE_HEIGHT{ SHIP_TILE_HEIGHT_HALF * 2.0f };

        //-------------------------------------------------
        // Ctor. && Dtor.
        //-------------------------------------------------

        Entity() = delete;

        Entity(iso::TileAtlas& t_tileAtlas, const iso::Unit::UnitId t_unitId, const sf::Vector2i& t_mapPosition, iso::Map& t_map)
            : m_tileAtlas{ t_tileAtlas }
            , m_unitId{ t_unitId }
            , m_mapPosition{ t_mapPosition }
        {
            m_currentScreenPosition = iso::IsoMath::ToScreen(m_mapPosition);

            // todo tmp code
            m_astar = std::make_unique<iso::Astar>(t_map.GetObstaclesForShipUnits(), t_map.GetMapWidth(), t_map.GetMapHeight());
        }

        Entity(const Entity& t_other) = delete;
        Entity(Entity&& t_other) noexcept = delete;
        Entity& operator=(const Entity& t_other) = delete;
        Entity& operator=(Entity&& t_other) noexcept = delete;

        ~Entity() noexcept = default;

        //-------------------------------------------------
        // Game Logic
        //-------------------------------------------------

        void HandleInput(sf::RenderWindow& t_window, const sf::Event& t_event, iso::Map& t_map)
        {
            if (t_event.type == sf::Event::KeyPressed)
            {

            }

            if (t_event.type == sf::Event::MouseButtonPressed)
            {
                if (t_event.mouseButton.button == sf::Mouse::Left)
                {
                    m_isMove = true;

                    SG_ISLANDS_DEBUG("Left Mouse pressed.");

                    FindPathToMouse(t_window);
                }
            }
        }

        void UpdateAnimations(iso::Unit& t_unit, const sf::Time t_dt)
        {
            for (auto& direction : iso::Unit::DIRECTIONS)
            {
                auto& animation{ t_unit.GetAnimation(m_unitId, direction) };
                animation.Update(t_dt);
            }

            if (m_isMove)
            {
                SetNextWayPoint(m_wayPoint);

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

                if (m_wayPoint == m_path.size() - 1)
                {
                    m_isMove = false;
                }
            }
        }

        void Draw(iso::Unit& t_unit, sf::RenderWindow& t_window)
        {
            auto& animation{ t_unit.GetAnimation(m_unitId, m_direction) };
            auto& sprite{ animation.GetSprite() };

            sprite.setPosition(m_currentScreenPosition);
            sprite.setOrigin(SHIP_TILE_WIDTH_HALF, SHIP_TILE_HEIGHT_HALF);

            // tmp code
            for (const auto& n : m_path)
            {
                iso::Tile::DrawTile(1000, n.position.x, n.position.y, t_window, m_tileAtlas);
            }

            t_window.draw(sprite);
        }

    protected:

    private:
        iso::TileAtlas& m_tileAtlas;
        std::vector<iso::Node> m_path;

        iso::Unit::UnitId m_unitId{ -1 };

        sf::Vector2i m_mapPosition{ -1, -1 };
        sf::Vector2f m_currentScreenPosition{ -1.0f, -1.0f };

        sf::Vector2i m_targetMapPosition{ -1, -1 };
        sf::Vector2f m_targetScreenPosition{ -1.0f, -1.0f };

        sf::Vector2f m_spriteScreenDirection{ -1.0f, -1.0f };
        sf::Vector2f m_spriteScreenNormalDirection{ -1.0f, -1.0f };

        float m_lengthToTarget{ -1.0f };

        bool m_isMove{ false };
        std::size_t m_wayPoint{ 0 };

        iso::Unit::Direction m_direction{ iso::Unit::Direction::E_DIRECTION };

        std::unique_ptr<iso::Astar> m_astar;

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        void FindPathToMouse(sf::RenderWindow& t_window)
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

            m_path = m_astar->FindPath(node, endNode);

            SG_ISLANDS_DEBUG("path size: {}", m_path.size());

            m_wayPoint = 1;
        }

        void SetNextWayPoint(const std::size_t t_pathIndex)
        {
            assert(t_pathIndex < m_path.size());

            // Wegpunkt als nächstes Ziel setzen
            m_targetMapPosition = m_path[t_pathIndex].position;

            // Ziel in Screen Coords umrechnen
            m_targetScreenPosition = iso::IsoMath::ToScreen(m_targetMapPosition);

            // calc direction vector to the target
            m_spriteScreenDirection = iso::VecMath::Direction(m_currentScreenPosition, m_targetScreenPosition);

            // calc the length to the target
            m_lengthToTarget = iso::VecMath::Length(m_spriteScreenDirection);
            SG_ISLANDS_DEBUG("length to the target: {}", m_lengthToTarget);

            // normalize the direction vector
            m_spriteScreenNormalDirection = m_spriteScreenDirection;
            iso::VecMath::Normalize(m_spriteScreenNormalDirection);

            // calc the angle to the target to set the sprite `Direction`
            m_direction = iso::Unit::GetDirectionByVec(m_spriteScreenNormalDirection);
        }
    };
}
