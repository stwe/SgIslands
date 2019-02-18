// This file is part of the SgIslands package.
// 
// Filename: IsoMath.hpp
// Created:  22.01.2019
// Updated:  18.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

namespace sg::islands::iso
{
    /**
     * @brief Isometric Math
     * @url http://clintbellanger.net/articles/isometric_math/
     */
    class IsoMath
    {
    public:
        static constexpr auto DEEP_WATER_TILE_WIDTH{ 640 };
        static constexpr auto DEEP_WATER_TILE_HEIGHT{ 320 };
        static constexpr auto DEEP_WATER_TILE_WIDTH_HALF{ 320.0f };
        static constexpr auto DEEP_WATER_TILE_HEIGHT_HALF{ 160.0f };

        static constexpr auto DEFAULT_TILE_WIDTH{ 64 };
        static constexpr auto DEFAULT_TILE_HEIGHT{ 32 };
        static constexpr auto DEFAULT_TILE_WIDTH_HALF{ 32.0f };
        static constexpr auto DEFAULT_TILE_HEIGHT_HALF{ 16.0f };
        static constexpr auto DEFAULT_TILE_WIDTH_QUARTER{ 16.0f };

        /**
         * @brief Translate between 2D and 1D coordinates.
         * @param t_mapX The x position in a 2D map.
         * @param t_mapY The y position in a 2D map.
         * @param t_mapWidth The width of th 2D map.
         * @return int The 1D index.
         */
        static int From2DTo1D(const int t_mapX, const int t_mapY, const int t_mapWidth)
        {
            return t_mapY * t_mapWidth + t_mapX;
        }

        /**
         * @brief Isometric projection.
         * @param t_mapCoords The coordinates of the map.
         * @param t_correction Corrects the fact that counting the tiles starts from zero.
         * @param t_tileWidthHalf The half width of a tile.
         * @param t_tileHeightHalf The half height of a tile.
         * @return sf::Vector2f()
         */
        static auto ToScreen(
            const sf::Vector2i& t_mapCoords,
            const bool t_correction = false,
            const float t_tileWidthHalf = DEFAULT_TILE_WIDTH_HALF,
            const float t_tileHeightHalf = DEFAULT_TILE_HEIGHT_HALF
        )
        {
            //screen.x = (map.x - map.y) * TILE_WIDTH_HALF;
            //screen.y = (map.x + map.y) * TILE_HEIGHT_HALF;

            auto x{ t_mapCoords.x };
            auto y{ t_mapCoords.y };

            if (t_correction)
            {
                x++;
                y++;
            }

            const auto screenX{ (x - y) * t_tileWidthHalf };
            const auto screenY{ (x + y) * t_tileHeightHalf };

            return sf::Vector2f(screenX, screenY);
        }

        /**
         * @brief Isometric projection.
         * @param t_mapX The x-position in a map.
         * @param t_mapY The y-position in a map.
         * @param t_correction Corrects the fact that counting the tiles starts from zero.
         * @param t_tileWidthHalf The half width of a tile.
         * @param t_tileHeightHalf The half height of a tile.
         * @return sf::Vector2f()
         */
        static auto ToScreen(
            const int t_mapX,
            const int t_mapY,
            const bool t_correction = false,
            const float t_tileWidthHalf = DEFAULT_TILE_WIDTH_HALF,
            const float t_tileHeightHalf = DEFAULT_TILE_HEIGHT_HALF
        )
        {
            return ToScreen(sf::Vector2i(t_mapX, t_mapY), t_correction, t_tileWidthHalf, t_tileHeightHalf);
        }

        /**
         * @brief Convert screen pixels back to map coordinates.
         * @param t_screenCoords The screen pixel to convert.
         * @return sf::Vector2i()
         */
        static auto ToMap(const sf::Vector2f& t_screenCoords)
        {
            //map.x = (screen.x / TILE_WIDTH_HALF + screen.y / TILE_HEIGHT_HALF) / 2;
            //map.y = (screen.y / TILE_HEIGHT_HALF - (screen.x / TILE_WIDTH_HALF)) / 2;

            auto mapX{ (t_screenCoords.x / DEFAULT_TILE_WIDTH_HALF + t_screenCoords.y / DEFAULT_TILE_HEIGHT_HALF) / 2 };
            auto mapY{ (t_screenCoords.y / DEFAULT_TILE_HEIGHT_HALF - (t_screenCoords.x / DEFAULT_TILE_WIDTH_HALF)) / 2 };

            mapX--; // -1
            mapY--; // -1

            return sf::Vector2i(static_cast<int>(floor(mapX)), static_cast<int>(floor(mapY)));
        }

    protected:

    private:

    };
}
