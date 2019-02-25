// This file is part of the SgIslands package.
// 
// Filename: AssetAnimation.hpp
// Created:  24.02.2019
// Updated:  25.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <string>
#include <map>
#include "Animation.hpp"

namespace sg::islands::iso
{
    using AnimationId = int;
    using AnimationName = std::string;
    using AnimationUniquePtr = std::unique_ptr<Animation>;

    enum class Direction
    {
        N_DIRECTION = 90,
        NE_DIRECTION = 45,
        E_DIRECTION = 0,
        SE_DIRECTION = 315,
        S_DIRECTION = 270,
        SW_DIRECTION = 225,
        W_DIRECTION = 180,
        NW_DIRECTION = 135
    };

    static constexpr auto NUMBER_OF_UNIT_DIRECTIONS{ 8 };
    static constexpr auto NUMBER_OF_BUILDING_DIRECTIONS{ 4 };

    static constexpr auto HALF_DIRECTION{ 45.0f / 2.0f };

    static constexpr std::array<Direction, NUMBER_OF_UNIT_DIRECTIONS> UNIT_DIRECTIONS{
        Direction::N_DIRECTION,
        Direction::NE_DIRECTION,
        Direction::E_DIRECTION,
        Direction::SE_DIRECTION,
        Direction::S_DIRECTION,
        Direction::SW_DIRECTION,
        Direction::W_DIRECTION,
        Direction::NW_DIRECTION
    };

    static constexpr std::array<Direction, NUMBER_OF_BUILDING_DIRECTIONS> BUILDING_DIRECTIONS{
        Direction::NE_DIRECTION,
        Direction::SE_DIRECTION,
        Direction::SW_DIRECTION,
        Direction::NW_DIRECTION
    };

    static constexpr auto DEFAULT_DIRECTION{ Direction::NE_DIRECTION };

    struct AssetAnimation
    {
        AnimationId animationId{ -1 };
        AnimationName animationName;
        std::string animationDir;
        int frames{ -1 };

        /**
         * @brief An `Animation` for every direction.
         */
        std::map<Direction, AnimationUniquePtr> animationForDirections;
    };
}
