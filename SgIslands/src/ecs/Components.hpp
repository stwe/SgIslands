// This file is part of the SgIslands package.
// 
// Filename: Components.hpp
// Created:  21.02.2019
// Updated:  23.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <SFML/System/Vector2.hpp>
#include "../iso/Assets.hpp"

namespace sg::islands::ecs
{
    struct PositionComponent
    {
        PositionComponent() = default;

        explicit PositionComponent(const sf::Vector2i& t_mapPosition)
            : mapPosition{ t_mapPosition }
        {
            screenPosition = iso::IsoMath::ToScreen(mapPosition, true);
        }

        sf::Vector2i mapPosition{ -1, -1 };
        sf::Vector2f screenPosition{ -1.0f, -1.0f };
    };

    struct TargetComponent
    {
        sf::Vector2i targetMapPosition{ -1, -1 };
        std::size_t nextWayPoint{ 0 };
        float lengthToTarget{ -1.0f };
        bool onTheWay{ false };
        std::vector<iso::Node> pathToTarget;
    };

    struct AssetsComponent
    {
        AssetsComponent() = default;

        AssetsComponent(const iso::AssetId t_actionAssetId, const iso::AssetId t_idleAssetId)
            : actionAssetId{ t_actionAssetId }
            , idleAssetId{ t_idleAssetId }
        {}

        iso::AssetId actionAssetId{ -1 };
        iso::AssetId idleAssetId{ -1 };
    };

    struct DirectionComponent
    {
        DirectionComponent() = default;

        explicit DirectionComponent(const iso::Assets::Direction t_direction)
            : direction{ t_direction }
        {}

        iso::Assets::Direction direction{ iso::Assets::DEFAULT_DIRECTION };
        sf::Vector2f spriteScreenNormalDirection{ -1.0f, -1.0f };
    };

    struct ActiveEntityComponent
    {
    };
}
