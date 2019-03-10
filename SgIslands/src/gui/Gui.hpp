// This file is part of the SgIslands package.
// 
// Filename: Gui.hpp
// Created:  10.03.2019
// Updated:  10.03.2019
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

namespace sg::islands::gui
{
    class Gui : public sf::Drawable
    {
    public:
        //-------------------------------------------------
        // Ctor. && Dtor.
        //-------------------------------------------------

        explicit Gui(const sf::Font& t_font)
        {
            m_nameTexture.loadFromFile("res/gfx/gui/city_name_bg.png");
            m_sidebarTexture.loadFromFile("res/gfx/gui/square_250_350.png");
            m_nameSprite.setTexture(m_nameTexture);
            m_sidebarSprite.setTexture(m_sidebarTexture);

            m_nameSprite.setPosition(250, 10);
            m_sidebarSprite.setPosition(700, 30);

            m_name.setFont(t_font);
            m_name.setString("Testcity");
            m_name.setCharacterSize(18);
            m_name.setFillColor(sf::Color::Black);
            m_name.setStyle(sf::Text::Bold);
            m_name.setPosition(320, 12);

            m_info.setFont(t_font);
            m_info.setCharacterSize(12);
            m_info.setFillColor(sf::Color::Black);
            m_info.setStyle(sf::Text::Bold | sf::Text::Italic | sf::Text::Underlined);
            m_info.setPosition(720, 50);
            m_info.setString("");
        }

        Gui(const Gui& t_other) = delete;
        Gui(Gui&& t_other) noexcept = delete;
        Gui& operator=(const Gui& t_other) = delete;
        Gui& operator=(Gui&& t_other) noexcept = delete;

        ~Gui() noexcept = default;

        //-------------------------------------------------
        // Update
        //-------------------------------------------------

        void SetActiveEntityInfo(const std::string& t_string)
        {
            m_info.setString(t_string);
        }

    protected:

    private:
        sf::Text m_name;
        sf::Text m_info;

        sf::Texture m_nameTexture;
        sf::Texture m_sidebarTexture;
        sf::Sprite m_nameSprite;
        sf::Sprite m_sidebarSprite;

        //-------------------------------------------------
        // Draw
        //-------------------------------------------------

        void draw(sf::RenderTarget& t_target, sf::RenderStates t_states) const override
        {
            t_target.draw(m_nameSprite, t_states);
            t_target.draw(m_sidebarSprite, t_states);
            t_target.draw(m_name);
            t_target.draw(m_info);
        }
    };
}
