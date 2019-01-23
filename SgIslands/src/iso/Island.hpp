// This file is part of the SgIslands package.
// 
// Filename: Island.hpp
// Created:  20.01.2019
// Updated:  23.01.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <vector>
#include "../core/Types.hpp"
#include "TileAtlas.hpp"
#include "IsoMath.hpp"

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

        auto GetXMapPos() const { return m_xMapPos; }
        auto GetYMapPos() const { return m_yMapPos; }

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

        //-------------------------------------------------
        // Setter
        //-------------------------------------------------

        void SetXMapPos(const int t_xMapPos) { m_xMapPos = t_xMapPos; }
        void SetYMapPos(const int t_yMapPos) { m_yMapPos = t_yMapPos; }

        //-------------------------------------------------
        // Draw
        //-------------------------------------------------

        void DrawIsland(sf::RenderWindow& t_window, const TileAtlas& t_tileAtlas)
        {
            for (auto y{ 0 }; y < m_height; ++y)
            {
                for (auto x{ 0 }; x < m_width; ++x)
                {
                    const auto index{ IsoMath::From2DTo1D(x, y, m_width) };
                    const auto& texture{ t_tileAtlas.GetTileAtlasTexture(m_islandFields[index].tileId - 1) };

                    // set position on the map
                    const auto xp{ x + m_xMapPos };
                    const auto yp{ y + m_yMapPos };

                    auto screenPosition{ IsoMath::ToScreen(xp, yp) };

                    sf::Sprite sprite;
                    sprite.setTexture(texture);

                    // adjust "origin" of the isometric
                    screenPosition.x -= TileAtlas::DEFAULT_TILE_WIDTH_HALF;
                    sprite.setPosition(screenPosition.x, screenPosition.y);

                    t_window.draw(sprite);

                    if (m_islandFields[index].clicked)
                    {
                        const auto& clickedTexture{ t_tileAtlas.GetTileAtlasTexture(TileAtlas::CLICKED_TILE) };
                        sf::Sprite clickedSprite;
                        clickedSprite.setTexture(clickedTexture);
                        clickedSprite.setPosition(screenPosition.x, screenPosition.y);
                        t_window.draw(clickedSprite);
                    }
                }
            }
        }

    protected:

    private:
        /**
         * @brief The x-position on the `Map`.
         */
        int m_xMapPos{ -1 };

        /**
         * @brief The y-position on the `Map`.
         */
        int m_yMapPos{ -1 };

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
            IS_CORE_INFO("[Island::LoadIslandFieldsFromFile()] Open {} for reading island fields.", t_filename);

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

            IS_CORE_INFO("[Island::LoadIslandFieldsFromFile()] Island width in tiles: {} ", m_width);
            IS_CORE_INFO("[Island::LoadIslandFieldsFromFile()] Island height in tiles: {} ", m_height);

            // get `<data>` element
            auto dataElement{ core::XmlWrapper::GetFirstChildElement(layerElement, "data") };

            IS_CORE_INFO("[Island::LoadIslandFieldsFromFile()] Loading tile Ids ...");

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
            IS_CORE_INFO("[Island::LoadIslandFieldsFromFile()] Successfully loaded {} island fields.", m_islandFields.size());
        }
    };
}
