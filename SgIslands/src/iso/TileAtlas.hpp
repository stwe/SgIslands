// This file is part of the SgIslands package.
// 
// Filename: TileAtlas.hpp
// Created:  20.01.2019
// Updated:  09.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include "../core/XmlWrapper.hpp"
#include "../core/ResourceHolder.hpp"

namespace sg::islands::iso
{
    class TileAtlas
    {
    public:
        using TileId = int;

        //-------------------------------------------------
        // Const / Known Ids
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

        static constexpr auto DEEP_WATER_TILE_WIDTH{ 640 };
        static constexpr auto DEEP_WATER_TILE_HEIGHT{ 320 };
        static constexpr auto DEEP_WATER_TILE_WIDTH_HALF{ 320.0f };
        static constexpr auto DEEP_WATER_TILE_HEIGHT_HALF{ 160.0f };

        static constexpr auto DEFAULT_TILE_WIDTH{ 64 };
        static constexpr auto DEFAULT_TILE_HEIGHT{ 32 };
        static constexpr auto DEFAULT_TILE_WIDTH_HALF{ 32.0f };
        static constexpr auto DEFAULT_TILE_HEIGHT_HALF{ 16.0f };

        static constexpr auto GRID_TILE{ 1000 };
        static constexpr auto CLICKED_TILE{ 2000 };

        //-------------------------------------------------
        // Ctor. && Dtor.
        //-------------------------------------------------

        TileAtlas() = delete;

        explicit TileAtlas(
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

        const sf::Texture& GetBackgroundTileGraphic(const TileId t_tileId) const
        {
            return m_backgroundTileset.GetResource(t_tileId);
        }

        const sf::Texture& GetTerrainTileGraphic(const TileId t_tileId) const
        {
            return m_terrainTileset.GetResource(t_tileId);
        }

        const sf::Texture& GetMiscTileGraphic(const TileId t_tileId) const
        {
            return m_miscTileset.GetResource(t_tileId);
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
    };
}
