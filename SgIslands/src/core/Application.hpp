// This file is part of the SgIslands package.
// 
// Filename: Application.hpp
// Created:  25.01.2019
// Updated:  25.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>
#include <entityx/entityx.h>
#include "Config.hpp"
#include "ResourceHolder.hpp"
#include "imGui/imgui.h"
#include "imGui/imgui-SFML.h"
#include "../iso/Map.hpp"
#include "../iso/Assets.hpp"
#include "../iso/Astar.hpp"
#include "../ecs/Components.hpp"
#include "../ecs/Systems.hpp"

namespace sg::islands::core
{
    class Application : public entityx::EntityX
    {
    public:
        using RenderWindowUniquePtr = std::unique_ptr<sf::RenderWindow>;
        using TileAtlasUniquePtr = std::unique_ptr<iso::TileAtlas>;
        using MapUniquePtr = std::unique_ptr<iso::Map>;
        using AssetsUniquePtr = std::unique_ptr<iso::Assets>;
        using AstarUniquePtr = std::unique_ptr<iso::Astar>;

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
                while (timeSinceLastUpdate > SF_TIME_PER_FRAME)
                {
                    timeSinceLastUpdate -= SF_TIME_PER_FRAME;

                    Input();
                    Update(SF_TIME_PER_FRAME);
                }

                UpdateStatistics(dt);
                Render();
            }
        }

    protected:

    private:
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
         * @brief The assets (units && buildings).
         */
        AssetsUniquePtr m_assets;

        /**
         * @brief A* (star) pathfinding.
         */
        AstarUniquePtr m_astar;

        /**
         * @brief Draw a grid if true.
         */
        bool m_drawGrid{ false };

        // entities
        entityx::Entity m_farmerEntity;
        entityx::Entity m_pirateShipEntity;
        entityx::Entity m_bakeryEntity;

        // frame statistics
        sf::Text m_statisticsText;
        sf::Time m_statisticsUpdateTime;
        std::size_t m_statisticsNumFrames{ 0 };

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

            // init imGui
            ImGui::SFML::Init(*m_window);
            auto& io{ ImGui::GetIO() };
            io.IniFilename = "res/config/Imgui.ini";

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

            // create `Astar` object
            m_astar = std::make_unique<iso::Astar>(*m_map);
            assert(m_astar);

            // create `Assets`
            m_assets = std::make_unique<iso::Assets>(m_appOptions.assets);
            assert(m_assets);

            // setup `EntityX` and create entities
            SetupEcs();

            SG_ISLANDS_INFO("[Application::Init()] Initialization finished.");
        }

        void Input()
        {
            sf::Event event{};
            while (m_window->pollEvent(event))
            {
                auto& io{ ImGui::GetIO() };

                ImGui::SFML::ProcessEvent(event);

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
                    if (io.WantCaptureMouse)
                    {
                        break;
                    }

                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        SG_ISLANDS_DEBUG("Application Left Mouse pressed.");

                        // get mouse position
                        const auto mousePosition{ sf::Mouse::getPosition(*m_window) };
                        const auto mouseWorldPosition{ m_window->mapPixelToCoords(mousePosition) };

                        // get map position of the mouse
                        const auto targetMapPosition{ iso::IsoMath::ToMap(mouseWorldPosition) };
                        SG_ISLANDS_DEBUG("mouse map x: {}", targetMapPosition.x);
                        SG_ISLANDS_DEBUG("mouse map y: {}", targetMapPosition.y);



                        /*

                        wenn click auf entity, dann setze entity als aktiv
                            - ansonsten suche Pfad

                        */


                        // set the targetMapPosition as target to all active entities
                        entities.each<ecs::TargetComponent, ecs::ActiveEntityComponent>(
                            [&targetMapPosition](entityx::Entity t_entity, ecs::TargetComponent& t_target, ecs::ActiveEntityComponent)
                            {
                                t_target.targetMapPosition = targetMapPosition;
                            }
                        );

                        // try to find path to target for all active entities
                        systems.update<ecs::FindPathSystem>(EX_TIME_PER_FRAME);





                    }
                }
            }
        }

        void Update(const sf::Time& t_dt)
        {
            systems.update<ecs::AnimationSystem>(EX_TIME_PER_FRAME);
            systems.update<ecs::MovementSystem>(EX_TIME_PER_FRAME);
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

            systems.update<ecs::RenderSystem>(EX_TIME_PER_FRAME);

            RenderImGui();

            m_tileAtlas->DrawMiscTile(iso::TileAtlas::CLICKED_TILE, 20, 20, *m_window);

            m_window->display();
        }

        void SetupEcs()
        {
            m_farmerEntity = entities.create();
            m_pirateShipEntity = entities.create();
            m_bakeryEntity = entities.create();

            m_farmerEntity.assign<ecs::PositionComponent>(sf::Vector2i(15, 15));
            m_farmerEntity.assign<ecs::AssetComponent>("Farmer0");
            m_farmerEntity.assign<ecs::DirectionComponent>(iso::DEFAULT_DIRECTION);
            m_farmerEntity.assign<ecs::TargetComponent>();

            m_pirateShipEntity.assign<ecs::PositionComponent>(sf::Vector2i(20, 20));
            m_pirateShipEntity.assign<ecs::AssetComponent>("Pirate1");
            m_pirateShipEntity.assign<ecs::DirectionComponent>(iso::DEFAULT_DIRECTION);
            m_pirateShipEntity.assign<ecs::ActiveEntityComponent>();
            m_pirateShipEntity.assign<ecs::TargetComponent>();

            m_bakeryEntity.assign<ecs::PositionComponent>(sf::Vector2i(8, 7));
            m_bakeryEntity.assign<ecs::AssetComponent>("Bakery0");
            m_bakeryEntity.assign<ecs::DirectionComponent>(iso::DEFAULT_DIRECTION);
            m_bakeryEntity.assign<ecs::TargetComponent>();

            systems.add<ecs::MovementSystem>(*m_assets);
            systems.add<ecs::RenderSystem>(*m_window, *m_assets, *m_tileAtlas);
            systems.add<ecs::AnimationSystem>(*m_assets);
            systems.add<ecs::FindPathSystem>(*m_assets, *m_astar);

            systems.configure();
        }

        void RenderImGui() const
        {
            ImGui::SFML::Update(*m_window, SF_TIME_PER_FRAME);
            ImGui::Begin("Menu");

            // close button
            if (ImGui::Button("Close"))
            {
                m_window->close();
            }

            ImGui::End();
            ImGui::SFML::Render(*m_window);
        }

        void UpdateStatistics(const sf::Time& t_dt)
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
