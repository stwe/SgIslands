// This file is part of the SgIslands package.
// 
// Filename: VecMath.hpp
// Created:  02.02.2019
// Updated:  02.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <SFML/System/Vector2.hpp>
#include <cmath>

namespace sg::islands::iso
{
    class VecMath
    {
    public:
        static float Length(const sf::Vector2f& t_vector)
        {
            return sqrt(t_vector.x * t_vector.x + t_vector.y * t_vector.y);
        }

        static void Normalize(sf::Vector2f& t_vector)
        {
            const auto length{ Length(t_vector) };
            if (length == 0)
            {
                return;
            }

            t_vector.x = t_vector.x / length;
            t_vector.y = t_vector.y / length;
        }

        static sf::Vector2f Direction(const sf::Vector2f& t_current, const sf::Vector2f& t_target)
        {
            return t_target - t_current;
        }

        static float DistanceToTarget(const sf::Vector2f& t_current, const sf::Vector2f& t_target)
        {
            return Length(Direction(t_current, t_target));
        }

        static float DistanceToTarget(const sf::Vector2f& t_vector)
        {
            return Length(t_vector);
        }

    protected:

    private:

    };
}
