// This file is part of the SgIslands package.
// 
// Filename: Assets.hpp
// Created:  14.02.2019
// Updated:  24.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <array>
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
        using AssetName = std::string;
        using AssetUniquePtr = std::unique_ptr<Asset>;
        using AssetsMap = std::map<AssetName, AssetUniquePtr>;

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
         * @return Const reference to `std::map`
         */
        const AssetsMap& GetAssetsMap() const noexcept { return m_assetsMap; }

        /**
         * @brief Returns a reference to the `AssetsMap`.
         * @return Reference to `std::map`
         */
        AssetsMap& GetAssetsMap() noexcept { return m_assetsMap; }

        /**
         * @brief Returns a reference to an `Asset`.
         * @param t_assetName The name of the `Asset`.
         * @return Reference to `Asset`
         */
        Asset& GetAsset(const AssetName& t_assetName)
        {
            try
            {
                return *m_assetsMap.at(t_assetName);
            }
            catch (const std::out_of_range& exception)
            {
                SG_ISLANDS_ERROR("[Assets::GetAsset()] Out of range exception: ", exception.what());
                THROW_SG_EXCEPTION("[Assets::GetAsset()] Key: " + t_assetName + " not found.");
            }
        }

        /**
         * @brief Returns a reference to the `Animation`.
         * @param t_assetName The name of the asset.
         * @param t_animationName The name of the animation.
         * @param t_direction The direction of the asset.
         * @return Reference to `Animation`
         */
        Animation& GetAnimation(const AssetName& t_assetName, const AnimationName& t_animationName, const Direction t_direction)
        {
            auto& asset{ GetAsset(t_assetName) };

            try
            {
                return *asset.assetAnimations.at(t_animationName)->animationForDirections.at(t_direction);
            }
            catch (const std::out_of_range& exception)
            {
                SG_ISLANDS_ERROR("[Assets::GetAnimation()] Out of range exception: ", exception.what());
                THROW_SG_EXCEPTION("[Assets::GetAnimation()] Animation " + t_animationName + " for asset " + t_assetName + " not found.");
            }
        }

        /**
         * @brief Computes the sprite `Direction` of a unit from a given vector.
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

            SG_ISLANDS_WARN("[Assets::GetDirectionByVec()] Return the default direction (NE_DIRECTION) for angle {}.", angleDeg);

            return DEFAULT_DIRECTION;
        }

    protected:

    private:
        AssetsMap m_assetsMap;

        //-------------------------------------------------
        // Load Data
        //-------------------------------------------------

        /**
         * @brief Create an asset from xml element.
         * @param t_element The xml element.
         * @return AssetName
         */
        AssetName CreateAssetFromXml(tinyxml2::XMLElement* t_element)
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

            auto assetUniquePtr{ std::make_unique<Asset>() };
            assert(assetUniquePtr);

            assetUniquePtr->assetId = idAttr;
            assetUniquePtr->assetName = nameAttr;
            assetUniquePtr->assetType = assetType;
            assetUniquePtr->tileWidth = tileWidthAttr;
            assetUniquePtr->tileHeight = tileHeightAttr;

            m_assetsMap.emplace(nameAttr, std::move(assetUniquePtr));

            return nameAttr;
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

            // get each `<asset>`
            for (auto asset{ unitsElement->FirstChildElement("asset") }; asset != nullptr; asset = asset->NextSiblingElement())
            {
                // create asset
                const auto assetName{ CreateAssetFromXml(asset) };

                // get created asset
                auto& currentAsset{ GetAsset(assetName) };

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

                    auto assetAnimationUniquePtr{ std::make_unique<AssetAnimation>() };
                    assert(assetAnimationUniquePtr);

                    assetAnimationUniquePtr->animationId = idAttr;
                    assetAnimationUniquePtr->animationName = nameAttr;
                    assetAnimationUniquePtr->animationDir = dirAttr;
                    assetAnimationUniquePtr->frames = framesAttr;

                    // create an `Animation` for each direction
                    for (const auto& direction : UNIT_DIRECTIONS)
                    {
                        // create animation dir
                        const auto unitDir{ unitsDir + dirAttr };

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
                            animationUniquePtr->AddFrame(unitDir + directionDir + "/" + filename);
                        }

                        // add `Animation`
                        assetAnimationUniquePtr->animationForDirections.emplace(direction, std::move(animationUniquePtr));
                    }

                    // add `AssetAnimation`
                    currentAsset.assetAnimations.emplace(nameAttr, std::move(assetAnimationUniquePtr));
                }
            }

            // read buildings ...

            // get `<buildings>` element
            const auto buildingsElement{ core::XmlWrapper::GetFirstChildElement(assetsElement, "buildings") };

            // get the buildings base directory from the `<dir>` element
            const auto buildingsDir{ core::XmlWrapper::GetStringFromXmlElement(buildingsElement, "dir") };

            SG_ISLANDS_INFO("[Assets::LoadConfigFile()] Loading buildings from {} ...", buildingsDir);

            // get each `<asset>`
            for (auto asset{ buildingsElement->FirstChildElement("asset") }; asset != nullptr; asset = asset->NextSiblingElement())
            {
                // create asset
                const auto assetName{ CreateAssetFromXml(asset) };

                // get created asset
                auto& currentAsset{ GetAsset(assetName) };

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

                    auto assetAnimationUniquePtr{ std::make_unique<AssetAnimation>() };
                    assert(assetAnimationUniquePtr);

                    assetAnimationUniquePtr->animationId = idAttr;
                    assetAnimationUniquePtr->animationName = nameAttr;
                    assetAnimationUniquePtr->animationDir = dirAttr;
                    assetAnimationUniquePtr->frames = framesAttr;

                    // create an `Animation` for each direction
                    for (const auto& direction : BUILDING_DIRECTIONS)
                    {
                        // create animation dir
                        const auto buildingDir{ buildingsDir + dirAttr };

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
                            animationUniquePtr->AddFrame(buildingDir + directionDir + "/" + filename);
                        }

                        // add `Animation`
                        assetAnimationUniquePtr->animationForDirections.emplace(direction, std::move(animationUniquePtr));
                    }

                    // add `AssetAnimation`
                    currentAsset.assetAnimations.emplace(nameAttr, std::move(assetAnimationUniquePtr));
                }
            }

            SG_ISLANDS_INFO("[Assets::LoadConfigFile()] Successfully loaded {} assets.", m_assetsMap.size());
        }
    };
}
