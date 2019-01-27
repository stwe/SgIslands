// This file is part of the SgIslands package.
// 
// Filename: Application.hpp
// Created:  25.01.2019
// Updated:  26.01.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>
#include "Config.hpp"
#include "ResourceHolder.hpp"
#include "../iso/Map.hpp"
#include "../iso/Unit.hpp"

namespace sg::islands::core
{
    class Application
    {
    public:
        using RenderWindowUniquePtr = std::unique_ptr<sf::RenderWindow>;
        using TileAtlasUniquePtr = std::unique_ptr<iso::TileAtlas>;
        using MapUniquePtr = std::unique_ptr<iso::Map>;
        using UnitUniquePtr = std::unique_ptr<iso::Unit>;

        static constexpr auto PIRATE_SHIP{ 0 };

        //-------------------------------------------------
        // Ctor. && Dtor.
        //-------------------------------------------------

        Application() = delete;

        explicit Application(const Filename& t_filename)
        {
            Init(t_filename);
        }

        Application(const Application& t_other) = delete;
        Application(Application&& t_other) noexcept = delete;
        Application& operator=(const Application& t_other) = delete;
        Application& operator=(Application&& t_other) noexcept = delete;

        ~Application() noexcept = default;

        //-------------------------------------------------
        // Game Loop
        //-------------------------------------------------

        void Run()
        {
            sf::Clock clock;
            auto timeSinceLastUpdate{ sf::Time::Zero };

            while (m_window->isOpen())
            {
                const auto dt{ clock.restart() };
                timeSinceLastUpdate += dt;
                while (timeSinceLastUpdate > TIME_PER_FRAME)
                {
                    timeSinceLastUpdate -= TIME_PER_FRAME;

                    Input();
                    Update(TIME_PER_FRAME);
                }

                UpdateStatistics(dt);
                Render();
            }
        }

    protected:

    private:
        inline static const sf::Time TIME_PER_FRAME{ sf::seconds(1.0f / 60.0f) };

        /**
         * @brief The loaded app options.
         */
        AppOptions m_appOptions;

        /**
         * @brief Target for 2D drawing.
         */
        RenderWindowUniquePtr m_window;

        /**
         * @brief Loaded character fonts.
         */
        FontHolder m_fonts;

        /**
         * @brief Loaded textures to use for tiles.
         */
        TileAtlasUniquePtr m_tileAtlas;

        /**
         * @brief A `Map` which contains several `Island`s.
         */
        MapUniquePtr m_map;

        /**
         * @brief A 2D camera.
         */
        sf::View m_islandView;

        /**
         * @brief A container for all animations of moveable units.
         */
        UnitUniquePtr m_units;

        sf::Text m_statisticsText;
        sf::Time m_statisticsUpdateTime;
        std::size_t m_statisticsNumFrames{ 0 };

        bool m_drawGrid{ false };

        //-------------------------------------------------
        // Game Logic
        //-------------------------------------------------

        void Init(const Filename& t_filename)
        {
            SG_ISLANDS_INFO("[Application::Init()] Initialize the application ...");

            // load options
            Config::LoadAppOptions(t_filename, m_appOptions);

            // create `RenderWindow`
            m_window = std::make_unique<sf::RenderWindow>(sf::VideoMode(m_appOptions.windowWidth, m_appOptions.windowHeight), m_appOptions.windowTitle);
            assert(m_window);

            // load and use the first font only
            m_fonts.Load(1, m_appOptions.fonts[0]);
            m_statisticsText.setFont(m_fonts.GetResource(1));
            m_statisticsText.setPosition(5.0f, 5.0f);
            m_statisticsText.setCharacterSize(10);

            // create `TileAtlas`
            m_tileAtlas = std::make_unique<iso::TileAtlas>(m_appOptions.tileset);
            assert(m_tileAtlas);

            // create `Map`
            m_map = std::make_unique<iso::Map>(m_appOptions.map);
            assert(m_map);

            // create `Unit`s
            m_units = std::make_unique<iso::Unit>(m_appOptions.units);
            assert(m_units);

            SG_ISLANDS_INFO("[Application::Init()] Initialization finished.");
        }

        void Input()
        {
            sf::Event event{};
            while (m_window->pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    m_window->close();
                }

                if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        SG_ISLANDS_DEBUG("Left Mouse");

                        const auto mousePosition{ sf::Mouse::getPosition(*m_window) };
                        const auto mPos{ m_window->mapPixelToCoords(mousePosition) };
                        const auto pos{ islands::iso::IsoMath::ToMap(mPos) };

                        SG_ISLANDS_DEBUG("map x: {}", pos.x);
                        SG_ISLANDS_DEBUG("map y: {}", pos.y);

                        for (auto& island : m_map->GetIslands())
                        {
                            auto onIsland{ island->IsMapPositionOnIsland(pos.x, pos.y) };

                            SG_ISLANDS_DEBUG("HasTile: {}", onIsland);

                            if (onIsland)
                            {
                                auto& islandField{ island->GetIslandFieldByMapPosition(pos.x, pos.y) };
                                islandField.clicked = true;
                            }
                        }
                    }
                }

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W)
                {
                    m_islandView.move(0, -40);
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S)
                {
                    m_islandView.move(0, 40);
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A)
                {
                    m_islandView.move(-40, 0);
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D)
                {
                    m_islandView.move(40, 0);
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R)
                {
                    m_islandView.rotate(25.0f);
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z)
                {
                    m_islandView.zoom(1.5f);
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::G)
                {
                    m_drawGrid = !m_drawGrid;
                }
            }
        }

        void Update(const sf::Time t_dt)
        {
            auto& animation{ m_units->GetAnimation(PIRATE_SHIP, iso::Unit::Direction::N_DIRECTION) };
            animation.Update(t_dt);
        }

        void Render()
        {
            m_window->clear();
            m_window->setView(m_islandView);

            m_map->DrawDeepWater(*m_window, *m_tileAtlas);
            m_map->DrawMap(*m_window, *m_tileAtlas);

            if (m_drawGrid)
            {
                m_map->DrawMapGrid(*m_window, *m_tileAtlas, m_fonts);
            }

            m_window->draw(m_statisticsText);

            auto& animation{ m_units->GetAnimation(PIRATE_SHIP, iso::Unit::Direction::N_DIRECTION) };
            auto& sprite{ animation.GetSprite() };
            sprite.setPosition(240, 240);
            m_window->draw(sprite);

            m_window->display();
        }

        void UpdateStatistics(const sf::Time t_dt)
        {
            m_statisticsUpdateTime += t_dt;
            m_statisticsNumFrames += 1;
            if (m_statisticsUpdateTime >= sf::seconds(1.0f))
            {
                m_statisticsText.setString("FPS: " + std::to_string(m_statisticsNumFrames));

                m_statisticsUpdateTime -= sf::seconds(1.0f);
                m_statisticsNumFrames = 0;
            }
        }
    };
}
