// This file is part of the SgIslands package.
// 
// Filename: Asset.hpp
// Created:  15.02.2019
// Updated:  25.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include "AssetAnimation.hpp"

namespace sg::islands::iso
{
    using AssetId = int;
    using AssetName = std::string;
    using AssetAnimationUniquePtr = std::unique_ptr<AssetAnimation>;

    enum class AssetType
    {
        LAND_UNIT,
        WATER_UNIT,
        BUILDING,
        NONE
    };

    struct Asset
    {
        AssetId assetId{ -1 };
        AssetName assetName;
        AssetType assetType{ AssetType::NONE };
        int tileWidth{ -1 };
        int tileHeight{ -1 };

        /**
         * @brief An `Asset` can contain multiple `AssetAnimationUniquePtr`.
         *        The key is an `AnimationName`, such `Work`, `Move` or `Idle`.
         */
        std::map<AnimationName, AssetAnimationUniquePtr> assetAnimations;
    };
}
