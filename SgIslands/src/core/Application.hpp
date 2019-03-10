// This file is part of the SgIslands package.
// 
// Filename: Application.hpp
// Created:  25.01.2019
// Updated:  09.03.2019
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
#include "Mouse.hpp"
#include "imGui/imgui.h"
#include "imGui/imgui-SFML.h"
#include "Collision.hpp"
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
        using BitmaskManagerUniquePtr = std::unique_ptr<BitmaskManager>;
        using MouseUniquePtr = std::unique_ptr<Mouse>;

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
         * @brief The `BitmaskManager` for pixel perfect collision detection.
         */
        BitmaskManagerUniquePtr m_bitmaskManager;

        /**
         * @brief A custom mouse cursor.
         */
        MouseUniquePtr m_mouse;

        /**
         * @brief Draw a grid if true.
         */
        bool m_drawGrid{ false };

        /**
         * @brief Draw a assets id grid if true.
         */
        bool m_drawAssetsGrid{ false };

        /**
         * @brief Draw Entities if true.
         */
        bool m_drawEntities{ true };

        bool m_drawMenu{ true };

        // entities
        entityx::Entity m_fisherShipEntity;
        entityx::Entity m_frigateShipEntity;
        entityx::Entity m_hukerShipEntity;
        entityx::Entity m_pirateShipEntity;
        entityx::Entity m_traderShipEntity;
        entityx::Entity m_farmerEntity;
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
            //io.ConfigFlags |= ImGuiMouseCursor_None;
            //io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

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

            // create `BitmaskManager`
            m_bitmaskManager = std::make_unique<BitmaskManager>();
            assert(m_bitmaskManager);

            // create `Assets`
            m_assets = std::make_unique<iso::Assets>(m_appOptions.assets, *m_bitmaskManager);
            assert(m_assets);

            // create `Mouse`
            m_mouse = std::make_unique<Mouse>(*m_bitmaskManager, m_appOptions.mouseCursor);
            assert(m_mouse);

            // hide default mouse cursor
            m_window->setMouseCursorVisible(false);

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

                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::M)
                {
                    m_drawMenu = !m_drawMenu;
                }

                if (event.type == sf::Event::MouseButtonPressed)
                {
                    if (io.WantCaptureMouse)
                    {
                        break;
                    }

                    // activate sprite
                    if (event.mouseButton.button == sf::Mouse::Left)
                    {
                        SG_ISLANDS_DEBUG("Left Mouse pressed.");

                        // set the targetMapPosition as target to all active entities
                        entities.each<ecs::DirectionComponent, ecs::AssetComponent>(
                            [this](entityx::Entity t_entity, ecs::DirectionComponent& t_directionComponent, ecs::AssetComponent& t_assetComponent)
                            {
                                const sf::Uint8 alphaLimit{ 100 };

                                const auto& animation{ m_assets->GetAnimation(t_assetComponent.assetId, "Idle", t_directionComponent.direction) };
                                const auto& sprite{ animation.GetSprite() };

                                const auto clickedEntity{ m_mouse->CollisionWith(sprite, alphaLimit) };

                                if (clickedEntity)
                                {
                                    // remove `ActiveEntityComponent` from all entities
                                    entities.each<ecs::ActiveEntityComponent>(
                                        [](entityx::Entity t_entity, ecs::ActiveEntityComponent)
                                        {
                                            t_entity.remove<ecs::ActiveEntityComponent>();
                                        }
                                    );

                                    // activate new entity
                                    t_entity.assign<ecs::ActiveEntityComponent>();
                                }

                                SG_ISLANDS_DEBUG("Pixel perfect result: {} for {}", clickedEntity, t_assetComponent.assetName);
                            }
                        );
                    }

                    // search path
                    if (event.mouseButton.button == sf::Mouse::Right)
                    {
                        SG_ISLANDS_DEBUG("Right Mouse pressed.");

                        // get mouse position
                        const auto mousePosition{ sf::Mouse::getPosition(*m_window) };
                        const auto mouseWorldPosition{ m_window->mapPixelToCoords(mousePosition) };

                        // get map position of the mouse
                        const auto targetMapPosition{ iso::IsoMath::ToMap(mouseWorldPosition) };
                        SG_ISLANDS_DEBUG("mouse map x: {}", targetMapPosition.x);
                        SG_ISLANDS_DEBUG("mouse map y: {}", targetMapPosition.y);

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
            const auto mousePosition{ sf::Mouse::getPosition(*m_window) };
            const auto mouseWorldPosition{ m_window->mapPixelToCoords(mousePosition) };
            m_mouse->SetPosition(mouseWorldPosition);

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

            if (m_drawAssetsGrid)
            {
                m_map->DrawAssetsGrid(*m_window, *m_tileAtlas, m_fonts);
            }

            m_window->setTitle(m_appOptions.windowTitle + " " + m_statisticsText.getString());

            systems.update<ecs::RenderSystem>(EX_TIME_PER_FRAME);

            if (m_drawMenu)
            {
                RenderImGui();
                m_window->setMouseCursorVisible(false);
            }

            m_window->draw(*m_mouse);

            m_window->display();
        }

        void SetupEcs()
        {
            // ships
            m_fisherShipEntity = entities.create();
            m_frigateShipEntity = entities.create();
            m_hukerShipEntity = entities.create();
            m_pirateShipEntity = entities.create();
            m_traderShipEntity = entities.create();

            // farmer && bakery
            m_farmerEntity = entities.create();
            m_bakeryEntity = entities.create();

            // setup ships
            m_fisherShipEntity.assign<ecs::PositionComponent>(sf::Vector2i(18, 8));
            m_fisherShipEntity.assign<ecs::AssetComponent>(0, "Fisher0");
            m_fisherShipEntity.assign<ecs::DirectionComponent>(iso::DEFAULT_DIRECTION);
            m_fisherShipEntity.assign<ecs::ActiveEntityComponent>();
            m_fisherShipEntity.assign<ecs::TargetComponent>();
            m_fisherShipEntity.assign<ecs::RenderComponent>();

            m_frigateShipEntity.assign<ecs::PositionComponent>(sf::Vector2i(4, 25));
            m_frigateShipEntity.assign<ecs::AssetComponent>(1, "Figate0");
            m_frigateShipEntity.assign<ecs::DirectionComponent>(iso::DEFAULT_DIRECTION);
            //m_frigateShipEntity.assign<ecs::ActiveEntityComponent>();
            m_frigateShipEntity.assign<ecs::TargetComponent>();
            m_frigateShipEntity.assign<ecs::RenderComponent>();

            m_hukerShipEntity.assign<ecs::PositionComponent>(sf::Vector2i(30, 30));
            m_hukerShipEntity.assign<ecs::AssetComponent>(2, "Huker0");
            m_hukerShipEntity.assign<ecs::DirectionComponent>(iso::DEFAULT_DIRECTION);
            //m_hukerShipEntity.assign<ecs::ActiveEntityComponent>();
            m_hukerShipEntity.assign<ecs::TargetComponent>();
            m_hukerShipEntity.assign<ecs::RenderComponent>();

            m_pirateShipEntity.assign<ecs::PositionComponent>(sf::Vector2i(20, 20));
            m_pirateShipEntity.assign<ecs::AssetComponent>(3, "Pirate1");
            m_pirateShipEntity.assign<ecs::DirectionComponent>(iso::DEFAULT_DIRECTION);
            //m_pirateShipEntity.assign<ecs::ActiveEntityComponent>();
            m_pirateShipEntity.assign<ecs::TargetComponent>();
            m_pirateShipEntity.assign<ecs::RenderComponent>();

            m_traderShipEntity.assign<ecs::PositionComponent>(sf::Vector2i(14, 25));
            m_traderShipEntity.assign<ecs::AssetComponent>(4, "Trader0");
            m_traderShipEntity.assign<ecs::DirectionComponent>(iso::DEFAULT_DIRECTION);
            //m_traderShipEntity.assign<ecs::ActiveEntityComponent>();
            m_traderShipEntity.assign<ecs::TargetComponent>();
            m_traderShipEntity.assign<ecs::RenderComponent>();

            // setup farmer
            m_farmerEntity.assign<ecs::PositionComponent>(sf::Vector2i(15, 15));
            m_farmerEntity.assign<ecs::AssetComponent>(5, "Farmer0");
            m_farmerEntity.assign<ecs::DirectionComponent>(iso::DEFAULT_DIRECTION);
            m_farmerEntity.assign<ecs::TargetComponent>();
            m_farmerEntity.assign<ecs::RenderComponent>();

            // setup bakery
            m_bakeryEntity.assign<ecs::PositionComponent>(sf::Vector2i(8, 7));
            m_bakeryEntity.assign<ecs::AssetComponent>(6, "Bakery0");
            m_bakeryEntity.assign<ecs::DirectionComponent>(iso::DEFAULT_DIRECTION);
            m_bakeryEntity.assign<ecs::TargetComponent>();
            m_bakeryEntity.assign<ecs::RenderComponent>();

            systems.add<ecs::MovementSystem>(*m_assets, *m_map);
            systems.add<ecs::RenderSystem>(*m_window, *m_assets, *m_tileAtlas, *m_map);
            systems.add<ecs::AnimationSystem>(*m_assets);
            systems.add<ecs::FindPathSystem>(*m_assets, *m_astar);

            systems.configure();
        }

        void RenderImGui()
        {
            ImGui::SFML::Update(*m_window, SF_TIME_PER_FRAME);
            ImGui::Begin("Menu");

            // draw grid
            if (ImGui::Button("Draw Grid"))
            {
                m_drawGrid = !m_drawGrid;
            }

            // draw assets grid
            if (ImGui::Button("Draw Assets Grid"))
            {
                m_drawAssetsGrid = !m_drawAssetsGrid;
            }

            // draw entities
            if (ImGui::Button("Draw Entities"))
            {
                m_drawEntities = !m_drawEntities;

                entities.each<ecs::RenderComponent>(
                    [this](entityx::Entity t_entity, ecs::RenderComponent& t_renderComponent)
                    {
                        t_renderComponent.render = m_drawEntities;
                    }
                );
            }

            // active entity
            auto fisherShip{ m_fisherShipEntity.has_component<ecs::ActiveEntityComponent>() };
            auto frigateShip{ m_frigateShipEntity.has_component<ecs::ActiveEntityComponent>() };
            auto hukerShip{ m_hukerShipEntity.has_component<ecs::ActiveEntityComponent>() };
            auto pirateShip{ m_pirateShipEntity.has_component<ecs::ActiveEntityComponent>() };
            auto traderShip{ m_traderShipEntity.has_component<ecs::ActiveEntityComponent>() };

            ImGui::Checkbox("Fisher Ship", &fisherShip);
            ImGui::SameLine(150);
            if (ImGui::Button("Toggle##0"))
            {
                if (fisherShip)
                {
                    m_fisherShipEntity.remove<ecs::ActiveEntityComponent>();
                }
                else
                {
                    m_fisherShipEntity.assign<ecs::ActiveEntityComponent>();
                }
            }

            ImGui::Checkbox("Frigate Ship", &frigateShip);
            ImGui::SameLine(150);
            if (ImGui::Button("Toggle##1"))
            {
                if (frigateShip)
                {
                    m_frigateShipEntity.remove<ecs::ActiveEntityComponent>();
                }
                else
                {
                    m_frigateShipEntity.assign<ecs::ActiveEntityComponent>();
                }
            }

            ImGui::Checkbox("Huker Ship", &hukerShip);
            ImGui::SameLine(150);
            if (ImGui::Button("Toggle##2"))
            {
                if (hukerShip)
                {
                    m_hukerShipEntity.remove<ecs::ActiveEntityComponent>();
                }
                else
                {
                    m_hukerShipEntity.assign<ecs::ActiveEntityComponent>();
                }
            }

            ImGui::Checkbox("Pirate Ship", &pirateShip);
            ImGui::SameLine(150);
            if (ImGui::Button("Toggle##3"))
            {
                if (pirateShip)
                {
                    m_pirateShipEntity.remove<ecs::ActiveEntityComponent>();
                }
                else
                {
                    m_pirateShipEntity.assign<ecs::ActiveEntityComponent>();
                }
            }

            ImGui::Checkbox("Trader Ship", &traderShip);
            ImGui::SameLine(150);
            if (ImGui::Button("Toggle##4"))
            {
                if (traderShip)
                {
                    m_traderShipEntity.remove<ecs::ActiveEntityComponent>();
                }
                else
                {
                    m_traderShipEntity.assign<ecs::ActiveEntityComponent>();
                }
            }

            // close
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
