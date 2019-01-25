#include <SFML/Graphics.hpp>
#include <iostream>
#include "core/Log.hpp"
#include "iso/TileAtlas.hpp"
#include "iso/Map.hpp"

int main()
{
    // init log
    sg::islands::core::Log::Init();
    IS_CORE_DEBUG("Logger initialized.");

    // generate tile atlas
    const sg::islands::iso::TileAtlas tileAtlas{ "res/config/tileset.xml" };

    // map
    sg::islands::iso::Map map{ "res/config/example_map.xml" };

    // window
    sf::RenderWindow window(sf::VideoMode(1024, 768), "SgIslands");

    // view
    sf::View islandView;

    while (window.isOpen())
    {
        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    IS_CORE_DEBUG("Left Mouse");

                    const auto mousePosition{ sf::Mouse::getPosition(window) };
                    const auto mPos{ window.mapPixelToCoords(mousePosition) };
                    const auto pos{ sg::islands::iso::IsoMath::ToMap(mPos) };

                    IS_CORE_DEBUG("map x: {}", pos.x);
                    IS_CORE_DEBUG("map y: {}", pos.y);

                    for (auto& island : map.GetIslands())
                    {
                        auto onIsland{ island->IsMapPositionOnIsland(pos.x, pos.y) };

                        IS_CORE_DEBUG("HasTile: {}", onIsland);

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
                islandView.move(0, -40);
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S)
            {
                islandView.move(0, 40);
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A)
            {
                islandView.move(-40, 0);
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D)
            {
                islandView.move(40, 0);
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R)
            {
                islandView.rotate(25.0f);
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Z)
            {
                islandView.zoom(1.5f);
            }
        }

        window.clear();

        window.setView(islandView);
        map.DrawDeepWater(window, tileAtlas);
        map.DrawMap(window, tileAtlas);
        map.DrawMapGrid(window, tileAtlas);

        window.display();
    }

    return 0;
}
