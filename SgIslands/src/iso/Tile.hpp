// This file is part of the SgIslands package.
// 
// Filename: Tile.hpp
// Created:  24.01.2019
// Updated:  09.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "TileAtlas.hpp"
#include "IsoMath.hpp"

namespace sg::islands::iso
{
    class Tile
    {
    public:
        // todo
        static void DrawTile(
            const TileAtlas::TileId t_tileId,
            const int t_xMapPos,
            const int t_yMapPos,
            sf::RenderWindow& t_window,
            const TileAtlas& t_tileAtlas
        )
        {
            // get texture
            const auto& texture{ t_tileAtlas.GetTerrainTileGraphic(t_tileId) }; // todo

            // convert the tile position to screen position
            auto screenPosition{ IsoMath::ToScreen(t_xMapPos, t_yMapPos) };

            // create sprite
            sf::Sprite sprite;

            // set texture
            sprite.setTexture(texture);

            // adjust "origin" of the isometric
            screenPosition.x -= TileAtlas::DEFAULT_TILE_WIDTH_HALF;
            sprite.setPosition(screenPosition.x, screenPosition.y);

            t_window.draw(sprite);
        }

    protected:

    private:

    };
}
