// This file is part of the SgIslands package.
// 
// Filename: Assets.hpp
// Created:  14.02.2019
// Updated:  03.03.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <vector>
#include "Asset.hpp"
#include "Animation.hpp"
#include "../core/Types.hpp"
#include "../core/XmlWrapper.hpp"
#include "../core/Log.hpp"

namespace sg::islands::iso
{
    class Assets
    {
    public:
        using AssetUniquePtr = std::unique_ptr<Asset>;
        using AssetsMap = std::vector<AssetUniquePtr>;
        using AssetsIdMap = std::map<AssetName, AssetId>;

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
         * @brief Returns a const reference to the `AssetsMap`.
         * @return Const reference to `std::vector`
         */
        const AssetsMap& GetAssetsMap() const noexcept { return m_assetsMap; }

        /**
         * @brief Returns a reference to the `AssetsMap`.
         * @return Reference to `std::vector`
         */
        AssetsMap& GetAssetsMap() noexcept { return m_assetsMap; }

        /**
         * @brief Returns a const reference to the `AssetsIdMap`.
         * @return Const reference to `std::map`
         */
        const AssetsIdMap& GetAssetsIdMap() const noexcept { return m_assetsIdMap; }

        /**
         * @brief Returns a reference to the `AssetsIdMap`.
         * @return Reference to `std::map`
         */
        AssetsIdMap& GetAssetsIdMap() noexcept { return m_assetsIdMap; }

        /**
         * @brief Returns a const reference to an `Asset` by Id.
         * @param t_assetId The Id of the `Asset`.
         * @return Const reference to `Asset`
         */
        const Asset& GetAsset(const AssetId& t_assetId) const { return *m_assetsMap[t_assetId]; }

        /**
         * @brief Returns a reference to an `Asset` by Id.
         * @param t_assetId The Id of the `Asset`.
         * @return Reference to `Asset`
         */
        Asset& GetAsset(const AssetId& t_assetId) { return *m_assetsMap[t_assetId]; }

        /**
         * @brief Returns a const reference to an `Asset` by name.
         * @param t_assetName The name of the `Asset`.
         * @return Const reference to `Asset`
         */
        const Asset& GetAsset(const AssetName& t_assetName) const { return *m_assetsMap[m_assetsIdMap.at(t_assetName)]; }

        /**
         * @brief Returns a reference to an `Asset` by name.
         * @param t_assetName The name of the `Asset`.
         * @return Reference to `Asset`
         */
        Asset& GetAsset(const AssetName& t_assetName) { return *m_assetsMap[m_assetsIdMap.at(t_assetName)]; }

        /**
         * @brief Returns a const reference to the `Animation`.
         * @param t_assetName The name of the `Asset` (e.g. `Pirate1`).
         * @param t_animationName The name of the `AssetAnimation` (e.g. `Move`, `Work` or `Idle`).
         * @param t_direction The `Direction` of the `Asset` (e.g. `NE_DIRECTION`).
         * @return Const reference to `Animation`
         */
        const Animation& GetAnimation(const AssetName& t_assetName, const AnimationName& t_animationName, const Direction t_direction) const
        {
            return *GetAsset(t_assetName).assetAnimations.at(t_animationName)->animationForDirections.at(t_direction);
        }

        /**
         * @brief Returns a reference to the `Animation`.
         * @param t_assetName The name of the `Asset` (e.g. `Pirate1`).
         * @param t_animationName The name of the `AssetAnimation` (e.g. `Move`, `Work` or `Idle`).
         * @param t_direction The `Direction` of the `Asset` (e.g. `NE_DIRECTION`).
         * @return Reference to `Animation`
         */
        Animation& GetAnimation(const AssetName& t_assetName, const AnimationName& t_animationName, const Direction t_direction)
        {
            return *GetAsset(t_assetName).assetAnimations.at(t_animationName)->animationForDirections.at(t_direction);
        }

        /**
         * @brief Returns a const reference to the `Animation`.
         * @param t_assetId The Id of the `Asset`.
         * @param t_animationName The name of the `AssetAnimation` (e.g. `Move`, `Work` or `Idle`).
         * @param t_direction The `Direction` of the `Asset` (e.g. `NE_DIRECTION`).
         * @return Const reference to `Animation`
         */
        const Animation& GetAnimation(const AssetId& t_assetId, const AnimationName& t_animationName, const Direction t_direction) const
        {
            return *GetAsset(t_assetId).assetAnimations.at(t_animationName)->animationForDirections.at(t_direction);
        }

        /**
         * @brief Returns reference to the `Animation`.
         * @param t_assetId The Id of the `Asset`.
         * @param t_animationName The name of the `AssetAnimation` (e.g. `Move`, `Work` or `Idle`).
         * @param t_direction The `Direction` of the `Asset` (e.g. `NE_DIRECTION`).
         * @return Reference to `Animation`
         */
        Animation& GetAnimation(const AssetId& t_assetId, const AnimationName& t_animationName, const Direction t_direction)
        {
            return *GetAsset(t_assetId).assetAnimations.at(t_animationName)->animationForDirections.at(t_direction);
        }

        /**
         * @brief Computes the sprite `Direction` of a unit from a given vector.
         * @param t_vector A direction vector.
         * @return `Direction`
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

            SG_ISLANDS_WARN("[Assets::GetDirectionByVec()] Return the default direction (NE_DIRECTION) for angle {}.", angleDeg);

            return DEFAULT_DIRECTION;
        }

    protected:

    private:
        /**
         * @brief Container for all assets. The id of the asset is the index.
         */
        AssetsMap m_assetsMap;

        /**
         * @brief Stores the corresponding id for each asset name.
         */
        AssetsIdMap m_assetsIdMap;

        //-------------------------------------------------
        // Load Data
        //-------------------------------------------------

        /**
         * @brief Create an `Asset` from xml element.
         * @param t_element The xml element.
         * @return AssetId
         */
        AssetId CreateAssetFromXml(const tinyxml2::XMLElement* const t_element)
        {
            // get id
            AssetId idAttr;
            core::XmlWrapper::QueryAttribute(t_element, "id", &idAttr);

            // get name
            const auto nameAttr{ core::XmlWrapper::GetAttribute(t_element, "name") };

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

            // create `Asset`
            auto assetUniquePtr{ std::make_unique<Asset>() };
            assert(assetUniquePtr);

            // set values
            assetUniquePtr->assetId = idAttr;
            assetUniquePtr->assetName = nameAttr;
            assetUniquePtr->assetType = assetType;
            assetUniquePtr->tileWidth = tileWidthAttr;
            assetUniquePtr->tileHeight = tileHeightAttr;

            // push back to a vector for access via Id
            m_assetsMap.push_back(std::move(assetUniquePtr));

            // this can be used to address an asset with its name
            m_assetsIdMap.emplace(nameAttr, idAttr);

            return idAttr;
        }

        /**
         * @brief Load `Asset`s from given xml element.
         * @tparam Size Number of directions.
         * @param t_element Xml element from which to read.
         * @param t_dir The base directory from the `<dir>` element.
         * @param t_directions An array with all directions.
         */
        template <std::size_t Size>
        void LoadAssetsFromElement(
            const tinyxml2::XMLElement* const t_element,
            const std::string& t_dir,
            const std::array<Direction, Size>& t_directions
        )
        {
            SG_ISLANDS_INFO("[Assets::LoadAssetsFromElement()] Loading assets from {} ...", t_dir);

            // get each `<asset>`
            for (auto asset{ t_element->FirstChildElement("asset") }; asset != nullptr; asset = asset->NextSiblingElement())
            {
                // create asset
                const auto assetId{ CreateAssetFromXml(asset) };

                // get each `<animation>`
                for (auto animation{ asset->FirstChildElement("animation") }; animation != nullptr; animation = animation->NextSiblingElement())
                {
                    // get id
                    AnimationId idAttr;
                    core::XmlWrapper::QueryAttribute(animation, "id", &idAttr);

                    // get name
                    const auto nameAttr{ core::XmlWrapper::GetAttribute(animation, "name") };

                    // get dir
                    const auto dirAttr{ core::XmlWrapper::GetAttribute(animation, "dir") };

                    // get frames
                    int framesAttr;
                    core::XmlWrapper::QueryAttribute(animation, "frames", &framesAttr);

                    // create `AssetAnimation`
                    auto assetAnimationUniquePtr{ std::make_unique<AssetAnimation>() };
                    assert(assetAnimationUniquePtr);

                    assetAnimationUniquePtr->animationId = idAttr;
                    assetAnimationUniquePtr->animationName = nameAttr;
                    assetAnimationUniquePtr->animationDir = dirAttr;
                    assetAnimationUniquePtr->frames = framesAttr;

                    // create an `Animation` for each direction
                    for (const auto& direction : t_directions)
                    {
                        // create animation dir
                        const auto dir{ t_dir + dirAttr };

                        // create a new `Animation` for this direction
                        auto animationUniquePtr{ std::make_unique<Animation>() };

                        // create all frames for this `Animation`
                        for (auto i{ 0 }; i < framesAttr; ++i)
                        {
                            // create filename with four leading zeros
                            const auto str{ std::to_string(i) };
                            const auto filename{ std::string(4 - str.length(), '0') + str + ".png" };

                            const auto directionDir{ std::to_string(static_cast<int>(direction)) };

                            // add frame
                            animationUniquePtr->AddFrame(dir + directionDir + "/" + filename);
                        }

                        // add `Animation` to `AssetAnimation`
                        assetAnimationUniquePtr->animationForDirections.emplace(direction, std::move(animationUniquePtr));
                    }

                    // add `AssetAnimation` to `Asset`
                    GetAsset(assetId).assetAnimations.emplace(nameAttr, std::move(assetAnimationUniquePtr));
                }
            }

            SG_ISLANDS_INFO("[Assets::LoadAssetsFromElement()] Successfully loaded assets.");
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

            LoadAssetsFromElement<NUMBER_OF_UNIT_DIRECTIONS>(unitsElement, unitsDir, UNIT_DIRECTIONS);

            // read buildings ...

            // get `<buildings>` element
            const auto buildingsElement{ core::XmlWrapper::GetFirstChildElement(assetsElement, "buildings") };

            // get the buildings base directory from the `<dir>` element
            const auto buildingsDir{ core::XmlWrapper::GetStringFromXmlElement(buildingsElement, "dir") };

            LoadAssetsFromElement<NUMBER_OF_BUILDING_DIRECTIONS>(buildingsElement, buildingsDir, BUILDING_DIRECTIONS);

            SG_ISLANDS_INFO("[Assets::LoadConfigFile()] Loaded {} assets.", m_assetsMap.size());
        }
    };
}
