// This file is part of the SgIslands package.
// 
// Filename: AssetMetaData.hpp
// Created:  15.02.2019
// Updated:  15.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <string>

namespace sg::islands::iso
{
    using AssetId = int;

    enum class AssetType
    {
        LAND_UNIT,
        WATER_UNIT,
        BUILDING,
        NONE
    };

    struct AssetMetaData
    {
        AssetId assetId{ -1 };
        std::string name;
        std::string dir;
        int frames{ -1 };
        AssetType assetType{AssetType::NONE };
        int tileWidth{ -1 };
        int tileHeight{ -1 };
    };
}
