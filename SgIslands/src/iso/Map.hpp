// This file is part of the SgIslands package.
// 
// Filename: Map.hpp
// Created:  20.01.2019
// Updated:  10.02.2019
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
    enum class TerrainType
    {
        DEEP_WATER,
        LAND,
        //OBSTACLE
    };

    struct MapField
    {
        TileAtlas::TileId terrainTileId;
        TerrainType terrainType;
        bool selected;
    };

    using IslandUniquePtr = std::unique_ptr<Island>;
    using Islands = std::vector<IslandUniquePtr>;
    using TerrainMap = std::vector<MapField>;

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
            GenerateTerrainMap();
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
         * @brief Get the `Map` width in default tiles.
         * @return int
         */
        auto GetMapWidth() const { return m_mapWidth; }

        /**
         * @brief Get the `Map` height in default tiles.
         * @return int
         */
        auto GetMapHeight() const { return m_mapHeight; }

        /**
         * @brief Get the `Island` objects of the `Map`.
         * @return Reference to `std::vector`
         */
        Islands& GetIslands() noexcept { return m_islands; }

        /**
         * @brief Get the `Island` objects of the `Map`.
         * @return Const reference to `std::vector`
         */
        const Islands& GetIslands() const noexcept { return m_islands; }

        /**
         * @brief Checks if the target is a deep water tile.
         * @param t_mapX The x-map position.
         * @param t_mapY The y-map position.
         * @return bool
         */
        auto IsDeepWater(const int t_mapX, const int t_mapY)
        {
            return m_terrainMap[IsoMath::From2DTo1D(t_mapX, t_mapY, m_mapWidth)].terrainType == TerrainType::DEEP_WATER;
        }

        /**
         * @brief Checks if the target is a land tile.
         * @param t_mapX The x-map position.
         * @param t_mapY The y-map position.
         * @return bool
         */
        auto IsLand(const int t_mapX, const int t_mapY)
        {
            return m_terrainMap[IsoMath::From2DTo1D(t_mapX, t_mapY, m_mapWidth)].terrainType == TerrainType::LAND;
        }

        //-------------------------------------------------
        // TerrainMap
        //-------------------------------------------------

        /**
         * @brief Creates a map with all information about the terrain.
         */
        void GenerateTerrainMap()
        {
            // "clear" map with deep water
            for (auto y{ 0 }; y < m_mapHeight; ++y)
            {
                for (auto x{ 0 }; x < m_mapWidth; ++x)
                {
                    MapField mapField{};

                    mapField.terrainTileId = -1;
                    mapField.terrainType = TerrainType::DEEP_WATER;
                    mapField.selected = false;

                    m_terrainMap.push_back(mapField);
                }
            }

            assert(m_terrainMap.size() == static_cast<std::size_t>(m_mapWidth * m_mapHeight));

            // go through all islands
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

                        // set terrain
                        m_terrainMap[index].terrainTileId = island->GetIslandFieldByMapPosition(xMapPos, yMapPos).tileId;
                        m_terrainMap[index].terrainType = TerrainType::LAND;
                    }
                }
            }

            SG_ISLANDS_INFO("[Map::GenerateTerrainMap()] Map with terrain information created.");
        }

        //-------------------------------------------------
        // Draw
        //-------------------------------------------------

        void DrawGrid(sf::RenderWindow& t_window, const TileAtlas& t_tileAtlas, const core::FontHolder& t_fontHolder) const
        {
            sf::Text text;
            text.setFont(t_fontHolder.GetResource(1));
            text.setCharacterSize(10);

            for (auto y{ 0 }; y < m_mapHeight; ++y)
            {
                for (auto x{ 0 }; x < m_mapWidth; ++x)
                {

                    t_tileAtlas.DrawMiscTile(TileAtlas::GRID_TILE, x, y, t_window);

                    const auto screenPosition{ IsoMath::ToScreen(x, y) };

                    auto xs{ std::to_string(x) };
                    auto ys{ std::to_string(y) };

                    // x: red color
                    text.setString(xs + ",");
                    text.setFillColor(sf::Color::Red);
                    text.setPosition(screenPosition.x - 16, screenPosition.y + 40);
                    t_window.draw(text);

                    // y: blue color
                    text.setString(ys);
                    text.setFillColor(sf::Color::Blue);
                    text.setPosition(screenPosition.x + 1, screenPosition.y + 40);
                    t_window.draw(text);
                }
            }
        }

        void DrawTerrain(sf::RenderWindow& t_window, const TileAtlas& t_tileAtlas)
        {
            // draw deep water background
            for (auto y{ 0 }; y < m_deepWaterHeight; ++y)
            {
                for (auto x{ 0 }; x < m_deepWaterWidth; ++x)
                {
                    t_tileAtlas.DrawBackgroundTile(x, y, t_window);
                }
            }

            // draw `Island`s
            for (const auto& island : m_islands)
            {
                island->DrawIsland(t_window, t_tileAtlas);
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
         * @brief Stores informations about the terrain.
         */
        TerrainMap m_terrainMap;

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

            // read deep water width && height
            core::XmlWrapper::QueryAttribute(mapElement, "deep_water_width", &m_deepWaterWidth);
            core::XmlWrapper::QueryAttribute(mapElement, "deep_water_height", &m_deepWaterHeight);

            // The tile for deep water is 10 times larger than a standard tile.
            // The size of the map is converted here to the width and height in default tiles.
            m_mapWidth = m_deepWaterWidth * IsoMath::DEEP_WATER_TILE_WIDTH / IsoMath::DEFAULT_TILE_WIDTH;
            m_mapHeight = m_deepWaterHeight * IsoMath::DEEP_WATER_TILE_HEIGHT / IsoMath::DEFAULT_TILE_HEIGHT;

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

            SG_ISLANDS_INFO("[Map::LoadMapFile()] Successfully loaded {} islands.", m_islands.size());
        }
    };
}
