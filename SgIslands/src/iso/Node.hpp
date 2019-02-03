// This file is part of the SgIslands package.
// 
// Filename: Node.hpp
// Created:  03.02.2019
// Updated:  03.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <SFML/System/Vector2.hpp>

namespace sg::islands::iso
{
    struct Node
    {
        sf::Vector2i position;
        sf::Vector2i parentPosition;

        /**
         * @brief The distance between the node and the start node.
         */
        float g{ 0.0f };

        /**
         * @brief The heuristic - estimated distance from the node to the end node.
         */
        float h{ 0.0f };

        /**
         * @brief The total cost of the node.
         */
        float f{ 0.0f };
    };

    inline bool operator==(const Node& t_lhs, const Node& t_rhs)
    {
        return t_lhs.position.x == t_rhs.position.x &&
            t_lhs.position.y == t_rhs.position.y;
    }
}
