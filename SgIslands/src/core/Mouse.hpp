// This file is part of the SgIslands package.
// 
// Filename: Mouse.hpp
// Created:  09.03.2019
// Updated:  09.03.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "BitmaskManager.hpp"
#include "Collision.hpp"

namespace sg::islands::core
{
    class Mouse : public sf::Drawable
    {
    public:
        //-------------------------------------------------
        // Ctor. && Dtor.
        //-------------------------------------------------

        explicit Mouse(BitmaskManager& t_bitmaskManager, const Filename& t_filename)
            : m_bitmaskManager{ t_bitmaskManager }
        {
            m_bitmaskManager.CreateTextureAndBitmask(m_texture, t_filename);
            m_sprite.setTexture(m_texture);
        }

        Mouse(const Mouse& t_other) = delete;
        Mouse(Mouse&& t_other) noexcept = delete;
        Mouse& operator=(const Mouse& t_other) = delete;
        Mouse& operator=(Mouse&& t_other) noexcept = delete;

        ~Mouse() noexcept = default;

        //-------------------------------------------------
        // Setter
        //-------------------------------------------------

        void SetPosition(const sf::Vector2f& t_position)
        {
            m_sprite.setPosition(t_position);
        }

        //-------------------------------------------------
        // Collision
        //-------------------------------------------------

        auto CollisionWith(const sf::Sprite& t_sprite, const sf::Uint8 t_alphaLimit = 0) const
        {
            return Collision::PixelPerfect(t_sprite, m_sprite, t_alphaLimit, m_bitmaskManager);
        }

    protected:

    private:
        BitmaskManager& m_bitmaskManager;

        sf::Sprite m_sprite;
        sf::Texture m_texture;

        //-------------------------------------------------
        // Draw
        //-------------------------------------------------

        void draw(sf::RenderTarget& t_target, sf::RenderStates t_states) const override
        {
            t_target.draw(m_sprite, t_states);
        }
    };
}
