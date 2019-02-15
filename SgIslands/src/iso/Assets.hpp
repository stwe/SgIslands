// This file is part of the SgIslands package.
// 
// Filename: Assets.hpp
// Created:  14.02.2019
// Updated:  15.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <array>
#include <map>
#include "Animation.hpp"
#include "AssetMetaData.hpp"
#include "../core/Types.hpp"
#include "../core/XmlWrapper.hpp"
#include "../core/Log.hpp"

namespace sg::islands::iso
{
    class Assets
    {
    public:
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

        using AssetMetaDataUniquePtr = std::unique_ptr<AssetMetaData>;
        using AssetsMetaDataMap = std::map<AssetId, AssetMetaDataUniquePtr>;

        using AssetMapKey = std::pair<AssetId, Direction>;
        using AnimationUniquePtr = std::unique_ptr<Animation>;
        using AssetsMap = std::map<AssetMapKey, AnimationUniquePtr>;

        //-------------------------------------------------
        // Ctor. && Dtor.
        //-------------------------------------------------

        Assets() = delete;

        explicit Assets(const core::Filename& t_filename)
        {
            LoadConfigFile(t_filename);
        }

        Assets(const Assets& t_other) = delete;
        Assets(Assets&& t_other) noexcept = delete;
        Assets& operator=(const Assets& t_other) = delete;
        Assets& operator=(Assets&& t_other) noexcept = delete;

        ~Assets() noexcept = default;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * @brief Returns a const reference to the assets metadata map.
         * @return Const reference to `std::map`
         */
        const AssetsMetaDataMap& GetAssetsMetaDataMap() const noexcept { return m_assetsMetaDataMap; }

        /**
         * @brief Returns a const reference to the asset metadata.
         * @param t_assetId The Id of the asset.
         * @return Const reference to `AssetMetaData`
         */
        const AssetMetaData& GetAssetMetaData(const AssetId t_assetId) const { return *m_assetsMetaDataMap.at(t_assetId); }

        /**
         * @brief Returns a reference to the assets map.
         * @return Reference to `std::map`
         */
        AssetsMap& GetAssetsMap() noexcept { return m_assetsMap; }

        /**
         * @brief Returns a const reference to the assets map.
         * @return Const reference to `std::map`
         */
        const AssetsMap& GetAssetsMap() const noexcept { return m_assetsMap; }

        /**
         * @brief Returns a reference to the animation.
         * @param t_assetId The Id of the asset.
         * @param t_direction The direction of the asset.
         * @return Reference to `Animation`
         */
        Animation& GetAnimation(const AssetId t_assetId, const Direction t_direction)
        {
            return *m_assetsMap.at(std::make_pair(t_assetId, t_direction));
        }

        /**
         * @brief Returns a const reference to the animation.
         * @param t_assetId The ID of the asset.
         * @param t_direction The direction of the asset.
         * @return Const reference to `Animation`
         */
        const Animation& GetAnimation(const AssetId t_assetId, const Direction t_direction) const
        {
            return *m_assetsMap.at(std::make_pair(t_assetId, t_direction));
        }

        /**
         * @brief Computes the sprite `Direction` from a given vector.
         * @param t_vector A direction vector.
         * @return Direction
         */
        static Direction GetUnitDirectionByVec(const sf::Vector2f& t_vector)
        {
            /*
                        (NW)       (N)       (NE)
                         225       270       315
                          *         |         *
                            *       |       *
                              *     |     *
                                *   |   *
                                  * | *
              (W) 180 -------------- -------------- 360 / 0  / (E)
                                  * | *   +
                                *   |   *      +
                              *     |     *         +
                            *       |       *            +
                          *         |         *               +
                        135        90        45              22.5f (HALF_DIRECTION)
                        (SW)       (S)       (SE)
            */

            const auto angleRad{ atan2(t_vector.y, t_vector.x) };
            const auto angleDeg{ (angleRad / M_PI * 180) + (angleRad > 0 ? 0 : 360) };

            // 0 ... 22.5 -> E
            if (angleDeg <= HALF_DIRECTION && angleDeg >= 0)
            {
                return Direction::E_DIRECTION;
            }
            // 337.5 ... 360 -> E
            if (angleDeg <= 360 && angleDeg >= 315 + HALF_DIRECTION)
            {
                return Direction::E_DIRECTION;
            }

            if (angleDeg <= 315 + HALF_DIRECTION && angleDeg >= 270 + HALF_DIRECTION)
            {
                return Direction::NE_DIRECTION;
            }

            if (angleDeg <= 270 + HALF_DIRECTION && angleDeg >= 225 + HALF_DIRECTION)
            {
                return Direction::N_DIRECTION;
            }

            if (angleDeg <= 225 + HALF_DIRECTION && angleDeg >= 180 + HALF_DIRECTION)
            {
                return Direction::NW_DIRECTION;
            }

            if (angleDeg <= 180 + HALF_DIRECTION && angleDeg >= 135 + HALF_DIRECTION)
            {
                return Direction::W_DIRECTION;
            }

            if (angleDeg <= 135 + HALF_DIRECTION && angleDeg >= 90 + HALF_DIRECTION)
            {
                return Direction::SW_DIRECTION;
            }

            if (angleDeg <= 90 + HALF_DIRECTION && angleDeg >= 45 + HALF_DIRECTION)
            {
                return Direction::S_DIRECTION;
            }

            if (angleDeg <= 45 + HALF_DIRECTION && angleDeg >= HALF_DIRECTION)
            {
                return Direction::SE_DIRECTION;
            }

            SG_ISLANDS_WARN("[Assets::GetDirectionByVec()] Return the default direction (E_DIRECTION) for angle {}.", angleDeg);

            return Direction::E_DIRECTION;
        }

    protected:

    private:
        AssetsMetaDataMap m_assetsMetaDataMap;
        AssetsMap m_assetsMap;

        //-------------------------------------------------
        // Load Data
        //-------------------------------------------------

        /**
         * @brief Create the metadata of an asset from xml element.
         * @param t_element The xml element.
         * @return AssetId
         */
        AssetId CreateAssetMetaDataFromXml(tinyxml2::XMLElement* t_element)
        {
            // get id
            AssetId idAttr;
            core::XmlWrapper::QueryAttribute(t_element, "id", &idAttr);

            // get name
            const auto nameAttr{ core::XmlWrapper::GetAttribute(t_element, "name") };

            // get dir
            const auto dirAttr{ core::XmlWrapper::GetAttribute(t_element, "dir") };

            // get frames
            int framesAttr;
            core::XmlWrapper::QueryAttribute(t_element, "frames", &framesAttr);

            // get type
            const auto typeAttr{ core::XmlWrapper::GetAttribute(t_element, "type") };

            // convert type
            auto assetType{ AssetType::NONE };
            if (typeAttr == "Land_Unit")
            {
                assetType = AssetType::LAND_UNIT;
            }
            else if (typeAttr == "Water_Unit")
            {
                assetType = AssetType::WATER_UNIT;
            }
            else if (typeAttr == "Building")
            {
                assetType = AssetType::BUILDING;
            }

            assert(assetType != AssetType::NONE);

            // get tile width
            int tileWidthAttr;
            core::XmlWrapper::QueryAttribute(t_element, "tile_width", &tileWidthAttr);

            // get tile height
            int tileHeightAttr;
            core::XmlWrapper::QueryAttribute(t_element, "tile_height", &tileHeightAttr);

            auto assetMetaDataUniquePtr{ std::make_unique<AssetMetaData>() };
            assert(assetMetaDataUniquePtr);

            assetMetaDataUniquePtr->assetId = idAttr;
            assetMetaDataUniquePtr->name = nameAttr;
            assetMetaDataUniquePtr->dir = dirAttr;
            assetMetaDataUniquePtr->frames = framesAttr;
            assetMetaDataUniquePtr->assetType = assetType;
            assetMetaDataUniquePtr->tileWidth = tileWidthAttr;
            assetMetaDataUniquePtr->tileHeight = tileHeightAttr;

            m_assetsMetaDataMap.emplace(idAttr, std::move(assetMetaDataUniquePtr));

            return idAttr;
        }

        /**
         * @brief Load all assets from a given config file.
         * @param t_filename The xml file with asset configurations.
         */
        void LoadConfigFile(const core::Filename& t_filename)
        {
            SG_ISLANDS_INFO("[Assets::LoadConfigFile()] Open {} for reading assets.", t_filename);

            tinyxml2::XMLDocument document;

            // load xml file
            core::XmlWrapper::LoadXmlFile(t_filename, document);

            // get `<assets>` element
            const auto assetsElement{ core::XmlWrapper::GetFirstChildElement(document, "assets") };

            // read units ...

            // get `<units>` element
            const auto unitsElement{ core::XmlWrapper::GetFirstChildElement(assetsElement, "units") };

            // get the units base directory from the `<dir>` element
            const auto unitsDir{ core::XmlWrapper::GetStringFromXmlElement(unitsElement, "dir") };

            SG_ISLANDS_INFO("[Assets::LoadConfigFile()] Loading units from {} ...", unitsDir);

            // get each `<unit>`
            for (auto unit{ unitsElement->FirstChildElement("unit") }; unit != nullptr; unit = unit->NextSiblingElement())
            {
                // create metadata
                const auto assetId{ CreateAssetMetaDataFromXml(unit) };
                const auto& metadata{ GetAssetMetaData(assetId) };

                const auto unitDir{ unitsDir + metadata.dir };

                // for each unit direction
                for (const auto& direction : UNIT_DIRECTIONS)
                {
                    // create a new `Animation` for this unit direction
                    auto animation{ std::make_unique<Animation>(metadata) };

                    // create all frames for this `Animation`
                    for (auto i{ 0 }; i < metadata.frames; ++i)
                    {
                        // create filename with four leading zeros
                        const auto str{ std::to_string(i) };
                        const auto filename{ std::string(4 - str.length(), '0') + str + ".png" };

                        const auto directionDir{ std::to_string(static_cast<int>(direction)) };

                        // add frame
                        animation->AddFrame(unitDir + directionDir + "/" + filename);
                    }

                    // save `Animation` for the direction
                    m_assetsMap.emplace(std::make_pair(metadata.assetId, direction), std::move(animation));
                }
            }

            // read buildings ...

            // get `<buildings>` element
            const auto buildingsElement{ core::XmlWrapper::GetFirstChildElement(assetsElement, "buildings") };

            // get the buildings base directory from the `<dir>` element
            const auto buildingsDir{ core::XmlWrapper::GetStringFromXmlElement(buildingsElement, "dir") };

            SG_ISLANDS_INFO("[Assets::LoadConfigFile()] Loading buildings from {} ...", buildingsDir);

            // get each `<building>`
            for (auto building{ buildingsElement->FirstChildElement("building") }; building != nullptr; building = building->NextSiblingElement())
            {
                // create metadata
                const auto assetId{ CreateAssetMetaDataFromXml(building) };
                const auto& metadata{ GetAssetMetaData(assetId) };

                const auto buildingDir{ buildingsDir + metadata.dir };

                // for each building direction
                for (const auto& direction : BUILDING_DIRECTIONS)
                {
                    // create a new `Animation` for this building direction
                    auto animation{ std::make_unique<Animation>(metadata) };

                    // create all frames for this `Animation`
                    for (auto i{ 0 }; i < metadata.frames; ++i)
                    {
                        // create filename with four leading zeros
                        const auto str{ std::to_string(i) };
                        const auto filename{ std::string(4 - str.length(), '0') + str + ".png" };

                        const auto directionDir{ std::to_string(static_cast<int>(direction)) };

                        // add frame
                        animation->AddFrame(buildingDir + directionDir + "/" + filename);
                    }

                    // save `Animation` for the direction
                    m_assetsMap.emplace(std::make_pair(metadata.assetId, direction), std::move(animation));
                }
            }

            SG_ISLANDS_INFO("[Assets::LoadConfigFile()] Successfully loaded {} assets.", m_assetsMetaDataMap.size());
        }
    };
}
