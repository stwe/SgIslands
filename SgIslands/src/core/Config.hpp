// This file is part of the SgIslands package.
// 
// Filename: Config.hpp
// Created:  26.01.2019
// Updated:  09.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <cassert>
#include "Types.hpp"
#include "XmlWrapper.hpp"
#include "Log.hpp"

namespace sg::islands::core
{
    struct AppOptions
    {
        // window
        std::string windowTitle;
        int windowWidth{ -1 };
        int windowHeight{ -1 };

        // fonts
        std::vector<Filename> fonts;

        // tilesets
        Filename backgroundTileset;
        Filename terrainTileset;
        Filename miscTileset;

        // map
        Filename map;

        // animations
        Filename unitAnimations;
    };

    class Config
    {
    public:
        static void LoadAppOptions(const Filename& t_filename, AppOptions& t_options)
        {
            SG_ISLANDS_INFO("[Config::LoadAppOptions()] Open {} for reading options.", t_filename);

            tinyxml2::XMLDocument document;

            // load xml file
            XmlWrapper::LoadXmlFile(t_filename, document);

            // get `<app>` element
            const auto appElement{ XmlWrapper::GetFirstChildElement(document, "app") };

            //-------------------------------------------------
            // Window options
            //-------------------------------------------------

            SG_ISLANDS_INFO("[Config::LoadAppOptions()] Set window options.");

            // get `<window>` element
            const auto windowElement{ XmlWrapper::GetFirstChildElement(appElement, "window") };

            // get the window options
            const auto windowTitle{ XmlWrapper::GetStringFromXmlElement(windowElement, "title") };
            const auto windowWidth{ XmlWrapper::GetIntFromXmlElement(windowElement, "width") };
            const auto windowHeight{ XmlWrapper::GetIntFromXmlElement(windowElement, "height") };

            SG_ISLANDS_INFO("[Config::LoadAppOptions()] Window title: {}.", windowTitle);

            t_options.windowTitle = windowTitle;

            assert(windowWidth);
            assert(windowHeight);

            SG_ISLANDS_INFO("[Config::LoadAppOptions()] Window width: {}.", windowWidth);
            SG_ISLANDS_INFO("[Config::LoadAppOptions()] Window height: {}.", windowHeight);

            t_options.windowWidth = windowWidth;
            t_options.windowHeight = windowHeight;

            //-------------------------------------------------
            // Fonts
            //-------------------------------------------------

            SG_ISLANDS_INFO("[Config::LoadAppOptions()] Load font options.");

            // get `<fonts>` element
            const auto fontsElement{ XmlWrapper::GetFirstChildElement(appElement, "fonts") };

            // get each `<font>`
            for (auto font{ fontsElement->FirstChildElement() }; font != nullptr; font = font->NextSiblingElement())
            {
                SG_ISLANDS_INFO("[Config::LoadAppOptions()] Font: {}.", font->GetText());
                t_options.fonts.emplace_back(font->GetText());
            }

            assert(!t_options.fonts.empty());

            //-------------------------------------------------
            // TileAtlas, Map && Animations
            //-------------------------------------------------

            SG_ISLANDS_INFO("[Config::LoadAppOptions()] Load world options.");

            // get `<world>` element
            const auto worldElement{ XmlWrapper::GetFirstChildElement(appElement, "world") };

            // get `<tileAtlas>` element
            const auto tileAtlasElement{ XmlWrapper::GetFirstChildElement(worldElement, "tileAtlas") };

            // tilesets
            const auto background{ XmlWrapper::GetStringFromXmlElement(tileAtlasElement, "background") };
            const auto terrain{ XmlWrapper::GetStringFromXmlElement(tileAtlasElement, "terrain") };
            const auto misc{ XmlWrapper::GetStringFromXmlElement(tileAtlasElement, "misc") };

            SG_ISLANDS_INFO("[Config::LoadAppOptions()] Background tileset: {}.", background);
            SG_ISLANDS_INFO("[Config::LoadAppOptions()] Terrain tileset: {}.", terrain);
            SG_ISLANDS_INFO("[Config::LoadAppOptions()] Misc tileset: {}.", misc);

            t_options.backgroundTileset = background;
            t_options.terrainTileset = terrain;
            t_options.miscTileset = misc;

            const auto map{ XmlWrapper::GetStringFromXmlElement(worldElement, "map") };
            const auto unitAnimations{ XmlWrapper::GetStringFromXmlElement(worldElement, "unitAnimations") };

            SG_ISLANDS_INFO("[Config::LoadAppOptions()] Map: {}.", map);
            SG_ISLANDS_INFO("[Config::LoadAppOptions()] Unit animations: {}.", unitAnimations);

            t_options.map = map;
            t_options.unitAnimations = unitAnimations;

            SG_ISLANDS_INFO("[Config::LoadAppOptions()] Options successfully loaded.");
        }

    protected:

    private:

    };
}
