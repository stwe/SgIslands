// This file is part of the SgIslands package.
// 
// Filename: Island.hpp
// Created:  20.01.2019
// Updated:  25.01.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>
#include "../core/Types.hpp"
#include "TileAtlas.hpp"
#include "IsoMath.hpp"
#include "Tile.hpp"

namespace sg::islands::iso
{
    struct IslandField
    {
        TileAtlas::TileId tileId{ -1 };
        int status{ -1 };
        bool clicked{ false };
    };

    using IslandFields = std::vector<IslandField>;

    class Island
    {
    public:
        //-------------------------------------------------
        // Ctor. && Dtor.
        //-------------------------------------------------

        Island() = delete;

        explicit Island(const core::Filename& t_filename)
        {
            LoadIslandFieldsFromFile(t_filename);
        }

        Island(const Island& t_other) = delete;
        Island(Island&& t_other) noexcept = delete;
        Island& operator=(const Island& t_other) = delete;
        Island& operator=(Island&& t_other) noexcept = delete;

        ~Island() noexcept = default;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * @brief Get the x starting position of the `Island` on a `Map`.
         * @return int
         */
        auto GetXOffset() const { return m_xOffset; }

        /**
         * @brief Get the y starting position of the `Island` on a `Map`.
         * @return int
         */
        auto GetYOffset() const { return m_yOffset; }

        /**
         * @brief Get the `Island` width in tiles.
         * @return int
         */
        auto GetWidth() const { return m_width; }

        /**
         * @brief Get the `Island` height in tiles.
         * @return int
         */
        auto GetHeight() const { return m_height; }

        /**
         * @brief Get the fields of the `Island`.
         * @return std::vector
         */
        IslandFields& GetIslandFields() noexcept { return m_islandFields; }

        /**
         * @brief Get the fields of the `Island`.
         * @return std::vector
         */
        const IslandFields& GetIslandFields() const noexcept { return m_islandFields; }

        /**
         * @brief Checks if a `Map` position is on the `Island`.
         * @param t_xMapPos The x-position on the `Map`.
         * @param t_yMapPos The y-position on the `Map`.
         * @return bool
         */
        bool IsMapPositionOnIsland(const int t_xMapPos, const int t_yMapPos) const noexcept
        {
            if (t_xMapPos < 0 || t_yMapPos < 0)
            {
                return false;
            }

            return t_xMapPos >= 0 + m_xOffset &&
                t_xMapPos <= m_width + m_xOffset - 1 &&
                t_yMapPos >= 0 + m_yOffset &&
                t_yMapPos <= m_height + m_yOffset - 1;
        }

        /**
         * @brief Get an IslandField by given `Map` position.
         * @param t_xMapPos The x-position on the `Map`.
         * @param t_yMapPos The y-position on the `Map`.
         * @return Reference to `IslandField`
         */
        IslandField& GetIslandFieldByMapPosition(const int t_xMapPos, const int t_yMapPos)
        {
            const auto islandFieldIndex{ IsoMath::From2DTo1D(t_xMapPos - m_xOffset, t_yMapPos - m_yOffset, m_width) };

            return m_islandFields[islandFieldIndex];
        }

        /**
         * @brief Get an IslandField by given `Map` position.
         * @param t_xMapPos The x-position on the `Map`.
         * @param t_yMapPos The y-position on the `Map`.
         * @return Const reference to `IslandField`
         */
        const IslandField& GetIslandFieldByMapPosition(const int t_xMapPos, const int t_yMapPos) const
        {
            const auto islandFieldIndex{ IsoMath::From2DTo1D(t_xMapPos - m_xOffset, t_yMapPos - m_yOffset, m_width) };

            return m_islandFields[islandFieldIndex];
        }

        //-------------------------------------------------
        // Setter
        //-------------------------------------------------

        void SetXOffset(const int t_xMapPos) { m_xOffset = t_xMapPos; }
        void SetYOffset(const int t_yMapPos) { m_yOffset = t_yMapPos; }

        //-------------------------------------------------
        // Draw
        //-------------------------------------------------

        void DrawIsland(sf::RenderWindow& t_window, const TileAtlas& t_tileAtlas)
        {
            for (auto y{ 0 }; y < m_height; ++y)
            {
                for (auto x{ 0 }; x < m_width; ++x)
                {
                    // from 2D to 1D
                    const auto index{ IsoMath::From2DTo1D(x, y, m_width) };

                    // get tileId
                    const auto tileId{ m_islandFields[index].tileId - 1 };

                    // determine the position of the tile on the map
                    const auto xMapPos{ x + m_xOffset };
                    const auto yMapPos{ y + m_yOffset };

                    Tile::DrawTile(tileId, xMapPos, yMapPos, t_window, t_tileAtlas);

                    if (m_islandFields[index].clicked)
                    {
                        Tile::DrawTile(TileAtlas::CLICKED_TILE, xMapPos, yMapPos, t_window, t_tileAtlas);
                    }
                }
            }
        }

    protected:

    private:
        /**
         * @brief The x starting position of the `Island` on a `Map`.
         */
        int m_xOffset{ -1 };

        /**
         * @brief The y starting position of the `Island` on a `Map`.
         */
        int m_yOffset{ -1 };

        /**
         * @brief The `Island` width in tiles.
         */
        int m_width{ -1 };

        /**
         * @brief The `Island` height in tiles.
         */
        int m_height{ -1 };

        /**
         * @brief The fields of the `Island`.
         */
        IslandFields m_islandFields;

        //-------------------------------------------------
        // Load Data
        //-------------------------------------------------

        void LoadIslandFieldsFromFile(const core::Filename& t_filename)
        {
            SG_ISLANDS_INFO("[Island::LoadIslandFieldsFromFile()] Open {} for reading island fields.", t_filename);

            tinyxml2::XMLDocument document;

            // load xml file
            core::XmlWrapper::LoadXmlFile(t_filename, document);

            // get `<map>` element
            const auto mapElement{ core::XmlWrapper::GetFirstChildElement(document, "map") };

            // get `<layer>` element
            const auto layerElement{ core::XmlWrapper::GetFirstChildElement(mapElement, "layer") };

            // read width && height
            core::XmlWrapper::QueryAttribute(layerElement, "width", &m_width);
            core::XmlWrapper::QueryAttribute(layerElement, "height", &m_height);

            SG_ISLANDS_INFO("[Island::LoadIslandFieldsFromFile()] Island width in tiles: {} ", m_width);
            SG_ISLANDS_INFO("[Island::LoadIslandFieldsFromFile()] Island height in tiles: {} ", m_height);

            // get `<data>` element
            auto dataElement{ core::XmlWrapper::GetFirstChildElement(layerElement, "data") };

            SG_ISLANDS_INFO("[Island::LoadIslandFieldsFromFile()] Loading tile Ids ...");

            // get each tile
            for (auto tile{ dataElement->FirstChildElement("tile") }; tile != nullptr; tile = tile->NextSiblingElement())
            {
                TileAtlas::TileId tileId;
                core::XmlWrapper::QueryAttribute(tile, "gid", &tileId);

                // create `IslandField`
                const auto status{ 1 };
                IslandField islandField{ tileId, status };

                // save `IslandField`
                m_islandFields.push_back(islandField);
            }

            assert(m_islandFields.size() == static_cast<size_t>(m_width * m_height));
            SG_ISLANDS_INFO("[Island::LoadIslandFieldsFromFile()] Successfully loaded {} island fields.", m_islandFields.size());
        }
    };
}
