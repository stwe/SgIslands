// This file is part of the SgIslands package.
// 
// Filename: Config.hpp
// Created:  26.01.2019
// Updated:  26.01.2019
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
        std::string windowTitle;
        int windowWidth;
        int windowHeight;
        std::vector<Filename> fonts;
        Filename tileset;
        Filename map;
        Filename units;
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

            // get `<window>` element
            const auto windowElement{ XmlWrapper::GetFirstChildElement(appElement, "window") };

            // get the window options
            const auto windowTitle{ XmlWrapper::GetStringFromXmlElement(windowElement, "title") };
            const auto windowWidth{ XmlWrapper::GetIntFromXmlElement(windowElement, "width") };
            const auto windowHeight{ XmlWrapper::GetIntFromXmlElement(windowElement, "height") };

            t_options.windowTitle = windowTitle;
            t_options.windowWidth = windowWidth;
            t_options.windowHeight = windowHeight;

            //-------------------------------------------------
            // Fonts
            //-------------------------------------------------

            // get `<fonts>` element
            const auto fontsElement{ XmlWrapper::GetFirstChildElement(appElement, "fonts") };

            // get each `<font>`
            for (auto font{ fontsElement->FirstChildElement() }; font != nullptr; font = font->NextSiblingElement())
            {
                t_options.fonts.emplace_back(font->GetText());
            }

            //-------------------------------------------------
            // TileAtlas, Map && Units
            //-------------------------------------------------

            // get `<world>` element
            const auto worldElement{ XmlWrapper::GetFirstChildElement(appElement, "world") };

            // get the world options
            const auto tileAtlas{ XmlWrapper::GetStringFromXmlElement(worldElement, "tileAtlas") };
            const auto map{ XmlWrapper::GetStringFromXmlElement(worldElement, "map") };
            const auto units{ XmlWrapper::GetStringFromXmlElement(worldElement, "units") };

            t_options.tileset = tileAtlas;
            t_options.map = map;
            t_options.units = units;

            SG_ISLANDS_INFO("[Config::LoadAppOptions()] Options successfully loaded.");
        }

    protected:

    private:

    };
}
