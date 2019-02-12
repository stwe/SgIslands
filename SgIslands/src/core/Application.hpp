// This file is part of the SgIslands package.
// 
// Filename: Application.hpp
// Created:  25.01.2019
// Updated:  12.02.2019
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
#include "../iso/UnitAnimations.hpp"
#include "../Entity.hpp"

namespace sg::islands::core
{
    class Application
    {
    public:
        using RenderWindowUniquePtr = std::unique_ptr<sf::RenderWindow>;
        using TileAtlasUniquePtr = std::unique_ptr<iso::TileAtlas>;
        using MapUniquePtr = std::unique_ptr<iso::Map>;
        using UnitAnimationsUniquePtr = std::unique_ptr<iso::UnitAnimations>;

        static constexpr auto PIRATE_SHIP{ 0 };
        static constexpr auto FARMER{ 1 };

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
         * @brief All animations of moveable units.
         */
        UnitAnimationsUniquePtr m_unitAnimations;

        sf::Text m_statisticsText;
        sf::Time m_statisticsUpdateTime;
        std::size_t m_statisticsNumFrames{ 0 };

        bool m_drawGrid{ false };
        int m_activeEntity{ PIRATE_SHIP };

        std::unique_ptr<Entity> m_shipEntity;
        std::unique_ptr<Entity> m_farmerEntity;

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

            // load and use the first font
            m_fonts.Load(1, m_appOptions.fonts[0]);
            m_statisticsText.setFont(m_fonts.GetResource(1));
            m_statisticsText.setPosition(5.0f, 5.0f);
            m_statisticsText.setCharacterSize(10);

            // create `TileAtlas`
            m_tileAtlas = std::make_unique<iso::TileAtlas>(m_appOptions.backgroundTileset, m_appOptions.terrainTileset, m_appOptions.miscTileset);
            assert(m_tileAtlas);

            // create `Map` with all `Island`s
            m_map = std::make_unique<iso::Map>(m_appOptions.map);
            assert(m_map);

            //////////////////////////////////////////////

            // create an `Entity`s
            m_shipEntity = std::make_unique<Entity>(*m_tileAtlas, PIRATE_SHIP, sf::Vector2i(19, 11), *m_map);
            m_farmerEntity = std::make_unique<Entity>(*m_tileAtlas, FARMER, sf::Vector2i(15, 15), *m_map);

            // create `UnitAnimations`
            m_unitAnimations = std::make_unique<iso::UnitAnimations>(m_appOptions.unitAnimations);
            assert(m_unitAnimations);

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

                if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        SG_ISLANDS_DEBUG("Application Left Mouse pressed.");

                        // get mouse position
                        const auto mousePosition{ sf::Mouse::getPosition(*m_window) };
                        const auto targetPosition{ m_window->mapPixelToCoords(mousePosition) };

                        // get map position of the mouse
                        const auto targetMapPosition{ iso::IsoMath::ToMap(targetPosition) };
                        SG_ISLANDS_DEBUG("mouse map x: {}", targetMapPosition.x);
                        SG_ISLANDS_DEBUG("mouse map y: {}", targetMapPosition.y);

                        // todo - aktive Entity setzen - manche sprites belegen mehrere Felder

                        if (targetMapPosition == m_shipEntity->GetMapPosition())
                        {
                            SG_ISLANDS_DEBUG("Schiff auf dem Feld");
                            m_activeEntity = PIRATE_SHIP;
                            m_shipEntity->SetRenderActive(true);
                            m_farmerEntity->SetRenderActive(false);
                        }
                        else if (targetMapPosition == m_farmerEntity->GetMapPosition())
                        {
                            SG_ISLANDS_DEBUG("Farmer auf dem Feld");
                            m_activeEntity = FARMER;
                            m_farmerEntity->SetRenderActive(true);
                            m_shipEntity->SetRenderActive(false);
                        }

                        if (m_activeEntity == PIRATE_SHIP && targetMapPosition != m_shipEntity->GetMapPosition())
                        {
                            m_shipEntity->HandleInput(*m_window, event, *m_map, *m_unitAnimations);
                        }

                        if (m_activeEntity == FARMER && targetMapPosition != m_farmerEntity->GetMapPosition())
                        {
                            m_farmerEntity->HandleInput(*m_window, event, *m_map, *m_unitAnimations);
                        }
                    }
                }
            }
        }

        void Update(const sf::Time t_dt)
        {
            // update entities
            m_shipEntity->UpdateAnimations(*m_unitAnimations, t_dt);
            m_farmerEntity->UpdateAnimations(*m_unitAnimations, t_dt);
        }

        void Render()
        {
            m_window->clear();
            m_window->setView(m_islandView);

            m_map->DrawTerrain(*m_window, *m_tileAtlas);

            if (m_drawGrid)
            {
                m_map->DrawGrid(*m_window, *m_tileAtlas, m_fonts);
            }

            m_window->setTitle(m_appOptions.windowTitle + " " + m_statisticsText.getString());

            // draw entities
            m_shipEntity->Draw(*m_unitAnimations, *m_window);
            m_farmerEntity->Draw(*m_unitAnimations, *m_window);

            m_window->display();
        }

        void UpdateStatistics(const sf::Time t_dt)
        {
            m_statisticsUpdateTime += t_dt;
            m_statisticsNumFrames += 1;

            if (m_statisticsUpdateTime >= sf::seconds(1.0f))
            {
                m_statisticsText.setString(" |  FPS: " + std::to_string(m_statisticsNumFrames));

                m_statisticsUpdateTime -= sf::seconds(1.0f);
                m_statisticsNumFrames = 0;
            }
        }
    };
}
