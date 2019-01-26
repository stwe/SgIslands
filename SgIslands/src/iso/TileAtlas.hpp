// This file is part of the SgIslands package.
// 
// Filename: TileAtlas.hpp
// Created:  20.01.2019
// Updated:  23.01.2019
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

        static constexpr auto DEEP_WATER_SOUTH{ 16 }; // 45
        static constexpr auto DEEP_WATER_EAST{ 17 };  // 135
        static constexpr auto DEEP_WATER_NORTH{ 18 }; // 225
        static constexpr auto DEEP_WATER_WEST{ 19 };  // 315

        static constexpr auto DEEP_WATER_TILE_WIDTH{ 640 };
        static constexpr auto DEEP_WATER_TILE_HEIGHT{ 320 };
        static constexpr auto DEEP_WATER_TILE_WIDTH_HALF{ 320.0f };
        static constexpr auto DEEP_WATER_TILE_HEIGHT_HALF{ 160.0f };

        static constexpr auto DEFAULT_TILE_WIDTH{ 64 };
        static constexpr auto DEFAULT_TILE_HEIGHT{ 32 };
        static constexpr auto DEFAULT_TILE_WIDTH_HALF{ 32.0f };
        static constexpr auto DEFAULT_TILE_HEIGHT_HALF{ 16.0f };

        // for debug
        static constexpr auto GRID_TILE{ 1000 };
        static constexpr auto CLICKED_TILE{ 2000 };

        //-------------------------------------------------
        // Ctor. && Dtor.
        //-------------------------------------------------

        TileAtlas() = delete;

        explicit TileAtlas(const core::Filename& t_filename)
        {
            LoadConfigFile(t_filename);
        }

        TileAtlas(const TileAtlas& t_other) = delete;
        TileAtlas(TileAtlas&& t_other) noexcept = delete;
        TileAtlas& operator=(const TileAtlas& t_other) = delete;
        TileAtlas& operator=(TileAtlas&& t_other) noexcept = delete;

        ~TileAtlas() noexcept = default;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        const sf::Texture& GetTileAtlasTexture(const TileId t_tileId) const
        {
            return m_textures.GetResource(t_tileId);
        }

    protected:

    private:
        /**
         * @brief Contains a texture for each unique tile of the same type.
         */
        core::TextureHolder m_textures;

        /**
         * @brief A counter used as Id.
         */
        inline static TileId m_tileId{ 0 };

        //-------------------------------------------------
        // Load Data
        //-------------------------------------------------

        /**
         * @brief Ceate a tile atlas.
         * @param t_filename The given xml config file.
         */
        void LoadConfigFile(const core::Filename& t_filename)
        {
            SG_ISLANDS_INFO("[TileAtlas::LoadConfigFile()] Open {} for creating a tile atlas.", t_filename);

            tinyxml2::XMLDocument document;

            // load xml file
            core::XmlWrapper::LoadXmlFile(t_filename, document);

            // get `<tiles>` element
            const auto tilesElement{core::XmlWrapper::GetFirstChildElement(document, "tiles") };

            // get `<terrain>` element
            const auto terrainElement{ core::XmlWrapper::GetFirstChildElement(tilesElement, "terrain") };

            // get the terrain base directory from the `<dir>` element
            const auto terrainDir{ core::XmlWrapper::GetStringFromXmlElement(terrainElement, "dir") };

            SG_ISLANDS_INFO("[TileAtlas::LoadConfigFile()] Loading packages from {} ...", terrainDir);

            // get `<packages>` element
            const auto packagesElement{ core::XmlWrapper::GetFirstChildElement(terrainElement, "packages") };

            // get each `<package>`
            for (auto package{ packagesElement->FirstChildElement() }; package != nullptr; package = package->NextSiblingElement())
            {
                // read package name
                const auto packageNameAttr{ core::XmlWrapper::GetAttribute(package, "name") };

                // create package base dir
                auto packageBaseDir{ terrainDir + packageNameAttr + "/" };

                // get `<curve_in>` element
                const auto curveInElement{ core::XmlWrapper::GetFirstChildElement(package, "curve_in") };

                // get `<curve_out>` element
                const auto curveOutElement{ core::XmlWrapper::GetFirstChildElement(package, "curve_out") };

                // get `<straight>` element
                const auto straightElement{ core::XmlWrapper::GetFirstChildElement(package, "straight") };

                // read load values
                bool loadCurveInAttr;
                core::XmlWrapper::QueryAttribute(curveInElement, "load", &loadCurveInAttr);

                bool loadCurveOutAttr;
                core::XmlWrapper::QueryAttribute(curveOutElement, "load", &loadCurveOutAttr);

                bool loadStraightAttr;
                core::XmlWrapper::QueryAttribute(straightElement, "load", &loadStraightAttr);

                // read files
                if (loadCurveInAttr)
                {
                    m_textures.Load(m_tileId++, packageBaseDir + "curve_in/45/0.png");
                    m_textures.Load(m_tileId++, packageBaseDir + "curve_in/135/0.png");
                    m_textures.Load(m_tileId++, packageBaseDir + "curve_in/225/0.png");
                    m_textures.Load(m_tileId++, packageBaseDir + "curve_in/315/0.png");
                }

                if (loadCurveOutAttr)
                {
                    m_textures.Load(m_tileId++, packageBaseDir + "curve_out/45/0.png");
                    m_textures.Load(m_tileId++, packageBaseDir + "curve_out/135/0.png");
                    m_textures.Load(m_tileId++, packageBaseDir + "curve_out/225/0.png");
                    m_textures.Load(m_tileId++, packageBaseDir + "curve_out/315/0.png");
                }

                if (loadStraightAttr)
                {
                    m_textures.Load(m_tileId++, packageBaseDir + "straight/45/0.png");
                    m_textures.Load(m_tileId++, packageBaseDir + "straight/135/0.png");
                    m_textures.Load(m_tileId++, packageBaseDir + "straight/225/0.png");
                    m_textures.Load(m_tileId++, packageBaseDir + "straight/315/0.png");
                }
            }

            // debug
            m_textures.Load(GRID_TILE, "res/gfx/grid.png"); // todo resource holder meldet keinen Fehler
            m_textures.Load(CLICKED_TILE, "res/gfx/clicked.png");

            SG_ISLANDS_INFO("[TileAtlas::LoadConfigFile()] Successfully loaded {} textures.", m_textures.GetNumberOfElements());
        }
    };
}
