// This file is part of the SgIslands package.
// 
// Filename: Map.hpp
// Created:  20.01.2019
// Updated:  04.02.2019
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
    using Obstacles = std::vector<int>;

    class Map
    {
    public:
        static constexpr auto OBSTACLE{ 1 };
        static constexpr auto EMPTY{ 0 };

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
         * @brief Get the `Map` width in deep water tiles.
         * @return int
         */
        auto GetDeepWaterWidth() const { return m_deepWaterWidth; }

        /**
         * @brief Get the `Map` height in deep water tiles.
         * @return int
         */
        auto GetDeepWaterHeight() const { return m_deepWaterHeight; }

        /**
         * @brief Get the `Map` width.
         * @return int
         */
        auto GetMapWidth() const { return m_mapWidth; }

        /**
         * @brief Get the `Map` height.
         * @return int
         */
        auto GetMapHeight() const { return m_mapHeight; }

        /**
         * @brief Get the `Island` objects of the `Map`.
         * @return Reference to std::vector
         */
        Islands& GetIslands() noexcept { return m_islands; }

        /**
         * @brief Get the `Island` objects of the `Map`.
         * @return Const reference to std::vector
         */
        const Islands& GetIslands() const noexcept { return m_islands; }

        /**
         * @brief Get the obstacle map using for ship units.
         * @return Reference to std::vector
         */
        Obstacles& GetObstaclesForShipUnits() noexcept { return m_obstaclesForShipUnits; }

        /**
         * @brief Get the obstacle map using for ship units.
         * @return Const reference to std::vector
         */
        const Obstacles& GetObstaclesForShipUnits() const noexcept { return m_obstaclesForShipUnits; }

        /**
         * @brief Get the obstacle map using for land units.
         * @return Reference to std::vector
         */
        Obstacles& GetObstaclesForLandUnits() noexcept { return m_obstaclesForLandUnits; }

        /**
         * @brief Get the obstacle map using for land units.
         * @return Const reference to std::vector
         */
        const Obstacles& GetObstaclesForLandUnits() const noexcept { return m_obstaclesForLandUnits; }

        //-------------------------------------------------
        // Draw
        //-------------------------------------------------

        void DrawMapGrid(sf::RenderWindow& t_window, const TileAtlas& t_tileAtlas, const core::FontHolder& t_fontHolder) const
        {
            const auto& grid{ t_tileAtlas.GetTileAtlasTexture(TileAtlas::GRID_TILE) };

            sf::Text text;
            text.setFont(t_fontHolder.GetResource(1));
            text.setCharacterSize(10);

            sf::Sprite sprite;
            sprite.setTexture(grid);

            for (auto y{ 0 }; y < m_mapHeight; ++y)
            {
                for (auto x{ 0 }; x < m_mapWidth; ++x)
                {
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

        void DrawDeepWater(sf::RenderWindow& t_window, const TileAtlas& t_tileAtlas) const
        {
            const auto& deepWaterSouth{ t_tileAtlas.GetTileAtlasTexture(TileAtlas::DEEP_WATER_SOUTH) };

            /*
            const auto& deepWaterEast{ t_tileAtlas.GetTileAtlasTexture(TileAtlas::DEEP_WATER_EAST) };
            const auto& deepWaterNorth{ t_tileAtlas.GetTileAtlasTexture(TileAtlas::DEEP_WATER_NORTH) };
            const auto& deepWaterWest{ t_tileAtlas.GetTileAtlasTexture(TileAtlas::DEEP_WATER_WEST) };
            */

            sf::Sprite sprite;
            sprite.setTexture(deepWaterSouth);

            for (auto y{ 0 }; y < m_deepWaterHeight; ++y)
            {
                for (auto x{ 0 }; x < m_deepWaterWidth; ++x)
                {
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

        void DrawObstaclesMap(
            sf::RenderWindow& t_window,
            const TileAtlas& t_tileAtlas,
            const core::FontHolder& t_fontHolder,
            const bool t_land = false
        )
        {
            const auto& grid{ t_tileAtlas.GetTileAtlasTexture(TileAtlas::GRID_TILE) };

            sf::Text text;
            text.setFont(t_fontHolder.GetResource(1));
            text.setCharacterSize(10);

            sf::Sprite sprite;
            sprite.setTexture(grid);

            for (auto y{ 0 }; y < m_mapHeight; ++y)
            {
                for (auto x{ 0 }; x < m_mapWidth; ++x)
                {
                    auto screenPosition{ IsoMath::ToScreen(x, y) };

                    // adjust "origin" of the isometric
                    screenPosition.x -= TileAtlas::DEFAULT_TILE_WIDTH_HALF;
                    sprite.setPosition(screenPosition.x, screenPosition.y);

                    t_window.draw(sprite);

                    // from 2D to 1D
                    const auto index{ IsoMath::From2DTo1D(x, y, m_mapWidth) };

                    std::string obstacleValue;
                    if (t_land)
                    {
                        obstacleValue = std::to_string(m_obstaclesForLandUnits[index]);
                    }
                    else
                    {
                        obstacleValue = std::to_string(m_obstaclesForShipUnits[index]);
                    }

                    text.setString(obstacleValue);
                    text.setFillColor(sf::Color::Cyan);
                    text.setPosition(screenPosition.x + 18, screenPosition.y + 40);

                    t_window.draw(text);
                }
            }
        }

    protected:

    private:
        /**
         * @brief The `Map` width in deep water tiles.
         */
        int m_deepWaterWidth{ -1 };

        /**
         * @brief The `Map` height in deep water tiles.
         */
        int m_deepWaterHeight{ -1 };

        /**
         * @brief The `Map` width in default tiles.
         */
        int m_mapWidth{ -1 };

        /**
         * @brief The `Map` height in default tiles.
         */
        int m_mapHeight{ -1 };

        /**
         * @brief The `Island` objects of the `Map`.
         */
        Islands m_islands;

        /**
         * @brief This obstacle map is used to find the path for a ship.
         */
        Obstacles m_obstaclesForShipUnits;

        /**
         * @brief This obstacle map is used to find the path for a land unit.
         */
        Obstacles m_obstaclesForLandUnits;

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
            core::XmlWrapper::QueryAttribute(mapElement, "width", &m_deepWaterWidth);
            core::XmlWrapper::QueryAttribute(mapElement, "height", &m_deepWaterHeight);

            // The tile for deep water is 10 times larger than a standard tile.
            // The size of the map is converted here to the width and height in default tiles.
            m_mapWidth = m_deepWaterWidth * TileAtlas::DEEP_WATER_TILE_WIDTH / TileAtlas::DEFAULT_TILE_WIDTH;
            m_mapHeight = m_deepWaterHeight * TileAtlas::DEEP_WATER_TILE_HEIGHT / TileAtlas::DEFAULT_TILE_HEIGHT;

            SG_ISLANDS_INFO("[Map::LoadMapFile()] Map width in deep water tiles: {} ", m_deepWaterWidth);
            SG_ISLANDS_INFO("[Map::LoadMapFile()] Map height in deep water tiles: {} ", m_deepWaterHeight);
            SG_ISLANDS_INFO("[Map::LoadMapFile()] Map width in default tiles: {} ", m_mapWidth);
            SG_ISLANDS_INFO("[Map::LoadMapFile()] Map height in default tiles: {} ", m_mapHeight);

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

            GenerateObstaclesMaps();

            SG_ISLANDS_INFO("[Map::LoadMapFile()] Successfully loaded {} islands.", m_islands.size());
        }

        //-------------------------------------------------
        // Obstacles Maps
        //-------------------------------------------------

        /**
         * @brief Generates two obstacles maps.
         *        One map is used to move ships and the other to move land units.
         */
        void GenerateObstaclesMaps()
        {
            SG_ISLANDS_INFO("[Map::LoadMapFile()] Generating obstacle maps ...");

            // prepare maps
            for (auto y{ 0 }; y < m_mapHeight; ++y)
            {
                for (auto x{ 0 }; x < m_mapWidth; ++x)
                {
                    m_obstaclesForShipUnits.push_back(EMPTY);
                    m_obstaclesForLandUnits.push_back(OBSTACLE);
                }
            }

            // add obstacles
            for (const auto& island : m_islands)
            {
                for (auto y{ 0 }; y < island->GetHeight(); ++y)
                {
                    for (auto x{ 0 }; x < island->GetWidth(); ++x)
                    {
                        // determine the position of the tile on the map
                        const auto xMapPos{ x + island->GetXOffset() };
                        const auto yMapPos{ y + island->GetYOffset() };

                        // from 2D to 1D
                        const auto index{ IsoMath::From2DTo1D(xMapPos, yMapPos, m_mapWidth) };

                        // set obstacle
                        m_obstaclesForShipUnits[index] = OBSTACLE;
                        m_obstaclesForLandUnits[index] = EMPTY;
                    }
                }
            }

            SG_ISLANDS_INFO("[Map::LoadMapFile()] The obstacle maps has been successfully generated.");
        }
    };
}
