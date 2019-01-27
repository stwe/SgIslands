#pragma once

#include <SFML/System/Vector2.hpp>
#include "iso/Unit.hpp"

namespace sg::islands
{
    class Entity
    {
    public:
        Entity() = default;

        Entity(const iso::Unit::UnitId t_unitId, const sf::Vector2f t_position)
            : m_unitId{ t_unitId }
            , m_position{ t_position }
        {}

        Entity(const Entity& t_other) = delete;
        Entity(Entity&& t_other) noexcept = delete;
        Entity& operator=(const Entity& t_other) = delete;
        Entity& operator=(Entity&& t_other) noexcept = delete;

        ~Entity() = default;

        void HandleInput(iso::Unit& t_unit, const sf::Event& t_event)
        {
            auto& animation{ t_unit.GetAnimation(m_unitId, m_direction) };
            auto& sprite{ animation.GetSprite() };

            if (t_event.type == sf::Event::KeyPressed && t_event.key.code == sf::Keyboard::Up)
            {
                m_direction = iso::Unit::Direction::N_DIRECTION;
                m_position.y -= 1;
            }
            if (t_event.type == sf::Event::KeyPressed && t_event.key.code == sf::Keyboard::Down)
            {
                m_direction = iso::Unit::Direction::S_DIRECTION;
                m_position.y += 1;
            }
            if (t_event.type == sf::Event::KeyPressed && t_event.key.code == sf::Keyboard::Left)
            {
                m_direction = iso::Unit::Direction::W_DIRECTION;
                m_position.x -= 1;
            }
            if (t_event.type == sf::Event::KeyPressed && t_event.key.code == sf::Keyboard::Right)
            {
                m_direction = iso::Unit::Direction::E_DIRECTION;
                m_position.x += 1;
            }
        }

        void UpdateAnimations(iso::Unit& t_unit, const sf::Time t_dt) const
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
            sprite.setPosition(m_position);
            t_window.draw(sprite);
        }

    protected:

    private:
        iso::Unit::UnitId m_unitId{ -1 };
        sf::Vector2f m_position{ 0.0f, 0.0f };

        iso::Unit::Direction m_direction{ iso::Unit::Direction::E_DIRECTION };
    };
}
