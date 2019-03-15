// This file is part of the SgIslands package.
// 
// Filename: TileAtlas.hpp
// Created:  20.01.2019
// Updated:  15.03.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include "../core/XmlWrapper.hpp"
#include "../core/ResourceHolder.hpp"
#include "IsoMath.hpp"

namespace sg::islands::iso
{
    class TileAtlas
    {
    public:
        using TileId = int;

        //-------------------------------------------------
        // Known Ids
        //-------------------------------------------------

        /*

         45   135   225   315
         /\    /\    o\    /\
        /  \  /  o  /  \  /  \
        \  /  \  /  \  /  o  /
         \o    \/    \/    \/

        */

        static constexpr auto DEEP_WATER_SOUTH_TILE{ 16 }; // 45
        static constexpr auto DEEP_WATER_EAST_TILE{ 17 };  // 135
        static constexpr auto DEEP_WATER_NORTH_TILE{ 18 }; // 225
        static constexpr auto DEEP_WATER_WEST_TILE{ 19 };  // 315

        static constexpr auto GRID_TILE{ 1000 };
        static constexpr auto CLICKED_TILE{ 2000 };
        static constexpr auto ENTITY_TILE{ 3000 };
        static constexpr auto BUOY_TILE{ 4000 };

        //-------------------------------------------------
        // Ctor. && Dtor.
        //-------------------------------------------------

        TileAtlas() = delete;

        TileAtlas(
            const core::Filename& t_filenameBackgroundTileset,
            const core::Filename& t_filenameTerrainTileset,
            const core::Filename& t_filenameMiscTileset
        )
        {
            LoadTilesetFromFile(t_filenameBackgroundTileset, m_backgroundTileset);
            LoadTilesetFromFile(t_filenameTerrainTileset, m_terrainTileset);
            LoadTilesetFromFile(t_filenameMiscTileset, m_miscTileset);
        }

        TileAtlas(const TileAtlas& t_other) = delete;
        TileAtlas(TileAtlas&& t_other) noexcept = delete;
        TileAtlas& operator=(const TileAtlas& t_other) = delete;
        TileAtlas& operator=(TileAtlas&& t_other) noexcept = delete;

        ~TileAtlas() noexcept = default;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * @brief Returns the deep water background texture to the passed tile id.
         * @param t_tileId A tile id.
         * @return Const reference to sf::Texture
         */
        const sf::Texture& GetBackgroundTileGraphic(const TileId t_tileId) const
        {
            return m_backgroundTileset.GetResource(t_tileId);
        }

        /**
         * @brief Returns the terrain texture to the passed tile id.
         * @param t_tileId A tile id.
         * @return Const reference to sf::Texture
         */
        const sf::Texture& GetTerrainTileGraphic(const TileId t_tileId) const
        {
            return m_terrainTileset.GetResource(t_tileId);
        }

        /**
         * @brief Returns the texture to the passed tile id.
         * @param t_tileId A tile id.
         * @return Const reference to sf::Texture
         */
        const sf::Texture& GetMiscTileGraphic(const TileId t_tileId) const
        {
            return m_miscTileset.GetResource(t_tileId);
        }

        //-------------------------------------------------
        // Draw
        //-------------------------------------------------

        /**
         * @brief Draws a deep water background tile.
         * @param t_xMapPos The x-map position.
         * @param t_yMapPos The y-map position.
         * @param t_window Reference to the `RenderWindow`.
         */
        void DrawBackgroundTile(
            const int t_xMapPos,
            const int t_yMapPos,
            sf::RenderWindow& t_window
        ) const
        {
            const auto& deepWaterSouth{ GetBackgroundTileGraphic(DEEP_WATER_SOUTH_TILE) };

            sf::Sprite sprite;
            sprite.setTexture(deepWaterSouth);

            auto screenPosition{ IsoMath::ToScreen(t_xMapPos, t_yMapPos, false, IsoMath::DEEP_WATER_TILE_WIDTH_HALF, IsoMath::DEEP_WATER_TILE_HEIGHT_HALF) };

            // adjust "origin" of the isometric
            screenPosition.x -= IsoMath::DEEP_WATER_TILE_WIDTH_HALF;
            screenPosition.y += IsoMath::DEFAULT_TILE_HEIGHT;
            sprite.setPosition(screenPosition.x, screenPosition.y);

            t_window.draw(sprite);
        }

        /**
         * @brief Draws a terrain tile.
         * @param t_tileId The tile id.
         * @param t_xMapPos The x-map position.
         * @param t_yMapPos The y-map position.
         * @param t_window Reference to the `RenderWindow`.
         */
        void DrawTerrainTile(
            const TileId t_tileId,
            const int t_xMapPos,
            const int t_yMapPos,
            sf::RenderWindow& t_window
        ) const
        {
            const auto& texture{ GetTerrainTileGraphic(t_tileId) };
            DrawTile(t_xMapPos, t_yMapPos, texture, t_window);
        }

        /**
         * @brief Draws a tile.
         * @param t_tileId The tile id.
         * @param t_xMapPos The x-map position.
         * @param t_yMapPos The y-map position.
         * @param t_window Reference to the `RenderWindow`.
         */
        void DrawMiscTile(
            const TileId t_tileId,
            const int t_xMapPos,
            const int t_yMapPos,
            sf::RenderWindow& t_window
        ) const
        {
            const auto& texture{ GetMiscTileGraphic(t_tileId) };
            DrawTile(t_xMapPos, t_yMapPos, texture, t_window);
        }

    protected:

    private:
        core::Tileset m_backgroundTileset;
        core::Tileset m_terrainTileset;
        core::Tileset m_miscTileset;

        //-------------------------------------------------
        // Load Data
        //-------------------------------------------------

        /**
         * @brief Add textures to the tile atlas.
         * @param t_filename The given xml config file.
         * @param t_tileset The loaded textures.
         */
        void LoadTilesetFromFile(const core::Filename& t_filename, core::Tileset& t_tileset) const
        {
            SG_ISLANDS_INFO("[TileAtlas::LoadTilesetFromFile()] Open {} to add textures to the tile atlas.", t_filename);

            tinyxml2::XMLDocument document;

            // load xml file
            core::XmlWrapper::LoadXmlFile(t_filename, document);

            // get `<tileset>` element
            const auto tilesetElement{core::XmlWrapper::GetFirstChildElement(document, "tileset") };

            SG_ISLANDS_INFO("[TileAtlas::LoadTilesetFromFile()] Loading tileset ...");

            // get each `<tile>`
            for (auto tile{ tilesetElement->FirstChildElement("tile") }; tile != nullptr; tile = tile->NextSiblingElement())
            {
                // read tile id
                int idAttr;
                core::XmlWrapper::QueryAttribute(tile, "id", &idAttr);

                // get `<image>` element
                const auto imageElement{ core::XmlWrapper::GetFirstChildElement(tile, "image") };

                // read path
                auto pathAttr{ core::XmlWrapper::GetAttribute(imageElement, "path") };

                // load and add texture
                t_tileset.Load(idAttr, pathAttr);
            }

            SG_ISLANDS_INFO("[TileAtlas::LoadTilesetFromFile()] Successfully loaded {} textures.", t_tileset.GetNumberOfElements());
        }

        //-------------------------------------------------
        // Draw Tile
        //-------------------------------------------------

        /**
         * @brief Draws a tile.
         * @param t_xMapPos The x-map position.
         * @param t_yMapPos The y-map position.
         * @param t_texture Const reference to a `sf::Texture`.
         * @param t_window Reference to the `RenderWindow`.
         */
        void DrawTile(
            const int t_xMapPos,
            const int t_yMapPos,
            const sf::Texture& t_texture,
            sf::RenderWindow& t_window
        ) const
        {
            auto screenPosition{ IsoMath::ToScreen(t_xMapPos, t_yMapPos) };

            sf::Sprite sprite;
            sprite.setTexture(t_texture);

            // adjust "origin" of the isometric
            screenPosition.x -= IsoMath::DEFAULT_TILE_WIDTH_HALF;
            sprite.setPosition(screenPosition.x, screenPosition.y);

            t_window.draw(sprite);
        }
    };
}
