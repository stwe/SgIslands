// This file is part of the SgIslands package.
// 
// Filename: Components.hpp
// Created:  21.02.2019
// Updated:  16.03.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <SFML/System/Vector2.hpp>
#include <utility>
#include "../iso/Node.hpp"
#include "../iso/IsoMath.hpp"

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

    struct AssetComponent
    {
        AssetComponent() = default;

        explicit AssetComponent(iso::AssetName t_assetName)
            : assetName{ std::move(t_assetName) }
        {}

        AssetComponent(const iso::AssetId t_assetId, iso::AssetName t_assetName)
            : assetId{ t_assetId }
            , assetName{std::move(t_assetName)}
        {}

        iso::AssetId assetId{ -1 };
        iso::AssetName assetName;
    };

    struct DirectionComponent
    {
        DirectionComponent() = default;

        explicit DirectionComponent(const iso::Direction t_direction)
            : direction{ t_direction }
        {}

        iso::Direction direction{ iso::DEFAULT_DIRECTION };
        sf::Vector2f spriteScreenNormalDirection{ -1.0f, -1.0f };
    };

    struct ActiveEntityComponent
    {
    };

    struct RenderComponent
    {
        bool render{ true };
    };

    struct WaterUnitComponent
    {
        WaterUnitComponent() = default;

        explicit WaterUnitComponent(const std::string& t_name)
            : name{ t_name }
        {}

        std::string name;
    };

    struct LandUnitComponent
    {
        LandUnitComponent() = default;

        explicit LandUnitComponent(const std::string& t_name)
            : name{ t_name }
        {}

        std::string name;
    };

    struct BuildingComponent
    {
        BuildingComponent() = default;

        explicit BuildingComponent(const std::string& t_name)
            : name{ t_name }
        {}

        std::string name;
    };
}
