// This file is part of the SgIslands package.
// 
// Filename: Entity.hpp
// Created:  27.01.2019
// Updated:  02.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include "iso/VecMath.hpp"
#include "iso/Unit.hpp"

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

        Entity(const iso::Unit::UnitId t_unitId, const sf::Vector2i& t_mapPosition)
            : m_unitId{ t_unitId }
            , m_mapPosition{ t_mapPosition }
        {
            m_currentScreenPosition = iso::IsoMath::ToScreen(m_mapPosition);
        }

        Entity(const Entity& t_other) = delete;
        Entity(Entity&& t_other) noexcept = delete;
        Entity& operator=(const Entity& t_other) = delete;
        Entity& operator=(Entity&& t_other) noexcept = delete;

        ~Entity() noexcept = default;

        //-------------------------------------------------
        // Game Logic
        //-------------------------------------------------

        void HandleInput(sf::RenderWindow& t_window, const sf::Event& t_event)
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

                    // set target position
                    const auto mousePosition{ sf::Mouse::getPosition(t_window) };
                    const auto targetPosition{ t_window.mapPixelToCoords(mousePosition) };
                    m_targetMapPosition = iso::IsoMath::ToMap(targetPosition);
                    m_targetScreenPosition = iso::IsoMath::ToScreen(m_targetMapPosition);

                    SG_ISLANDS_DEBUG("target map x: {}", m_targetMapPosition.x);
                    SG_ISLANDS_DEBUG("target map y: {}", m_targetMapPosition.y);
                    SG_ISLANDS_DEBUG("target screen x: {}", m_targetScreenPosition.x);
                    SG_ISLANDS_DEBUG("target screen y: {}", m_targetScreenPosition.y);

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
            }
        }

        void UpdateAnimations(iso::Unit& t_unit, const sf::Time t_dt)
        {
            for (auto& direction : iso::Unit::DIRECTIONS)
            {
                auto& animation{ t_unit.GetAnimation(m_unitId, direction) };
                animation.Update(t_dt);
            }
        }

        void Draw(iso::Unit& t_unit, sf::RenderWindow& t_window)
        {
            auto& animation{ t_unit.GetAnimation(m_unitId, m_direction) };
            auto& sprite{ animation.GetSprite() };

            // todo Update()
            if (m_isMove)
            {
                m_spriteScreenDirection = iso::VecMath::Direction(m_currentScreenPosition, m_targetScreenPosition);
                m_lengthToTarget = iso::VecMath::Length(m_spriteScreenDirection);
                if (m_lengthToTarget > 1.0f)
                {
                    m_currentScreenPosition.x += m_spriteScreenNormalDirection.x;
                    m_currentScreenPosition.y += m_spriteScreenNormalDirection.y;
                }

                if (m_lengthToTarget <= 1.0f)
                {
                    m_isMove = false;
                }
            }

            sprite.setPosition(m_currentScreenPosition);
            sprite.setOrigin(SHIP_TILE_WIDTH_HALF, SHIP_TILE_HEIGHT_HALF);

            t_window.draw(sprite);
        }

    protected:

    private:
        iso::Unit::UnitId m_unitId{ -1 };

        sf::Vector2i m_mapPosition{ -1, -1 };
        sf::Vector2f m_currentScreenPosition{ -1.0f, -1.0f };

        sf::Vector2i m_targetMapPosition{ -1, -1 };
        sf::Vector2f m_targetScreenPosition{ -1.0f, -1.0f };

        sf::Vector2f m_spriteScreenDirection{ -1, -1 };
        sf::Vector2f m_spriteScreenNormalDirection{ -1, -1 };

        float m_lengthToTarget{ -1.0f };

        bool m_isMove{ false };

        iso::Unit::Direction m_direction{ iso::Unit::Direction::E_DIRECTION };
    };
}
