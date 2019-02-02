// This file is part of the SgIslands package.
// 
// Filename: Map.hpp
// Created:  20.01.2019
// Updated:  02.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include "Island.hpp"
#include "IsoMath.hpp"

namespace sg::islands::iso
{
    using IslandUniquePtr = std::unique_ptr<Island>;
    using Islands = std::vector<IslandUniquePtr>;

    class Map
    {
    public:
        //-------------------------------------------------
        // Ctor. && Dtor.
        //-------------------------------------------------

        Map() = delete;

        explicit Map(const core::Filename& t_filename)
        {
            LoadMapFile(t_filename);
        }

        Map(const Map& t_other) = delete;
        Map(Map&& t_other) noexcept = delete;
        Map& operator=(const Map& t_other) = delete;
        Map& operator=(Map&& t_other) noexcept = delete;

        ~Map() noexcept = default;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * @brief Get the `Map` width in tiles.
         * @return 
         */
        auto GetMapWidth() const { return m_width; }

        /**
         * @brief Get the `Map` height in tiles.
         * @return 
         */
        auto GetMapHeight() const { return m_height; }

        /**
         * @brief Get the `Island` objects of the `Map`.
         * @return std::vector
         */
        Islands& GetIslands() noexcept { return m_islands; }

        /**
         * @brief Get the `Island` objects of the `Map`.
         * @return std::vector
         */
        const Islands& GetIslands() const noexcept { return m_islands; }

        //-------------------------------------------------
        // Draw
        //-------------------------------------------------

        void DrawMapGrid(sf::RenderWindow& t_window, const TileAtlas& t_tileAtlas, const core::FontHolder& t_fontHolder)
        {
            const auto& grid{ t_tileAtlas.GetTileAtlasTexture(TileAtlas::GRID_TILE) };

            sf::Text text;
            text.setFont(t_fontHolder.GetResource(1));
            text.setCharacterSize(10);

            const auto width{ m_width * TileAtlas::DEEP_WATER_TILE_WIDTH / TileAtlas::DEFAULT_TILE_WIDTH };
            const auto height{ m_height * TileAtlas::DEEP_WATER_TILE_HEIGHT / TileAtlas::DEFAULT_TILE_HEIGHT };

            for (auto y{ 0 }; y < height; ++y)
            {
                for (auto x{ 0 }; x < width; ++x)
                {
                    sf::Sprite sprite;
                    sprite.setTexture(grid);

                    auto screenPosition{ IsoMath::ToScreen(x, y) };

                    // adjust "origin" of the isometric
                    screenPosition.x -= TileAtlas::DEFAULT_TILE_WIDTH_HALF;
                    sprite.setPosition(screenPosition.x, screenPosition.y);

                    t_window.draw(sprite);

                    auto xs{ std::to_string(x) };
                    auto ys{ std::to_string(y) };

                    // x: red color
                    text.setString(xs + ", ");
                    text.setFillColor(sf::Color::Red);
                    text.setPosition(screenPosition.x + 18, screenPosition.y + 40);
                    t_window.draw(text);

                    // y: blue color
                    text.setString(ys);
                    text.setFillColor(sf::Color::Blue);
                    text.setPosition(screenPosition.x + 36, screenPosition.y + 40);
                    t_window.draw(text);
                }
            }
        }

        void DrawDeepWater(sf::RenderWindow& t_window, const TileAtlas& t_tileAtlas)
        {
            const auto& deepWaterSouth{ t_tileAtlas.GetTileAtlasTexture(TileAtlas::DEEP_WATER_SOUTH) };

            /*
            const auto& deepWaterEast{ t_tileAtlas.GetTileAtlasTexture(TileAtlas::DEEP_WATER_EAST) };
            const auto& deepWaterNorth{ t_tileAtlas.GetTileAtlasTexture(TileAtlas::DEEP_WATER_NORTH) };
            const auto& deepWaterWest{ t_tileAtlas.GetTileAtlasTexture(TileAtlas::DEEP_WATER_WEST) };
            */

            for (auto y{ 0 }; y < m_height; ++y)
            {
                for (auto x{ 0 }; x < m_width; ++x)
                {
                    sf::Sprite sprite;
                    sprite.setTexture(deepWaterSouth);

                    auto screenPosition{ IsoMath::ToScreen(x, y, TileAtlas::DEEP_WATER_TILE_WIDTH_HALF, TileAtlas::DEEP_WATER_TILE_HEIGHT_HALF) };

                    // adjust "origin" of the isometric
                    screenPosition.x -= TileAtlas::DEEP_WATER_TILE_WIDTH_HALF;
                    screenPosition.y += TileAtlas::DEFAULT_TILE_HEIGHT;

                    sprite.setPosition(screenPosition.x, screenPosition.y);

                    t_window.draw(sprite);
                }
            }
        }

        void DrawMap(sf::RenderWindow& t_window, const TileAtlas& t_tileAtlas)
        {
            for (const auto& island : m_islands)
            {
                island->DrawIsland(t_window, t_tileAtlas);
            }
        }

    protected:

    private:
        /**
         * @brief The `Map` width in tiles.
         */
        int m_width{ -1 };

        /**
         * @brief The `Map` height in tiles.
         */
        int m_height{ -1 };

        /**
         * @brief The `Island` objects of the `Map`.
         */
        Islands m_islands;

        //-------------------------------------------------
        // Load Data
        //-------------------------------------------------

        void LoadMapFile(const core::Filename& t_filename)
        {
            SG_ISLANDS_INFO("[Map::LoadMapFile()] Open {} for reading map islands.", t_filename);

            tinyxml2::XMLDocument document;

            // load xml file
            core::XmlWrapper::LoadXmlFile(t_filename, document);

            // get `<map>` element
            const auto mapElement{ core::XmlWrapper::GetFirstChildElement(document, "map") };

            // read width && height
            core::XmlWrapper::QueryAttribute(mapElement, "width", &m_width);
            core::XmlWrapper::QueryAttribute(mapElement, "height", &m_height);

            SG_ISLANDS_INFO("[Map::LoadMapFile()] Map width in tiles: {} ", m_width);
            SG_ISLANDS_INFO("[Map::LoadMapFile()] Map height in tiles: {} ", m_height);

            // get `<islands>` element
            auto islands{ core::XmlWrapper::GetFirstChildElement(mapElement, "islands") };

            // get each island
            for (auto island{ islands->FirstChildElement("island") }; island != nullptr; island = island->NextSiblingElement())
            {
                // read island map positions
                int xMapPosAttr;
                core::XmlWrapper::QueryAttribute(island, "x_map_pos", &xMapPosAttr);

                int yMapPosAttr;
                core::XmlWrapper::QueryAttribute(island, "y_map_pos", &yMapPosAttr);

                // read path to the island xml file
                const auto filename{ core::XmlWrapper::GetStringFromXmlElement(island, "filename") };

                // create `Island`
                auto islandUniquePtr{ std::make_unique<Island>(filename) };
                assert(islandUniquePtr);

                // set the map position of the island
                islandUniquePtr->SetXOffset(xMapPosAttr);
                islandUniquePtr->SetYOffset(yMapPosAttr);

                // save `Island`
                m_islands.push_back(std::move(islandUniquePtr));
            }

            SG_ISLANDS_INFO("[Map::LoadMapFile()] Successfully loaded {} islands.", m_islands.size());
        }
    };
}
