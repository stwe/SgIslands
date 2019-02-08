// This file is part of the SgIslands package.
// 
// Filename: UnitAnimations.hpp
// Created:  26.01.2019
// Updated:  08.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <map>
#include <memory>
#include "../core/XmlWrapper.hpp"
#include "Animation.hpp"

namespace sg::islands::iso
{
    class UnitAnimations
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

        static constexpr auto NUMBER_OF_DIRECTIONS{ 8 };
        static constexpr auto HALF_DIRECTION{ 45.0f / 2.0f };

        static constexpr std::array<Direction, NUMBER_OF_DIRECTIONS> DIRECTIONS {
            Direction::N_DIRECTION,
            Direction::NE_DIRECTION,
            Direction::E_DIRECTION,
            Direction::SE_DIRECTION,
            Direction::S_DIRECTION,
            Direction::SW_DIRECTION,
            Direction::W_DIRECTION,
            Direction::NW_DIRECTION
        };

        static constexpr auto FRAME_DURATION{ 0.2 };

        using UnitId = int;
        using UnitMapKey = std::pair<UnitId, Direction>;
        using AnimationUniquePtr = std::unique_ptr<Animation>;
        using Animations = std::map<UnitMapKey, AnimationUniquePtr>;

        //-------------------------------------------------
        // Ctor. && Dtor.
        //-------------------------------------------------

        UnitAnimations() = delete;

        explicit UnitAnimations(const core::Filename& t_filename)
        {
            LoadConfigFile(t_filename);
        }

        UnitAnimations(const UnitAnimations& t_other) = delete;
        UnitAnimations(UnitAnimations&& t_other) noexcept = delete;
        UnitAnimations& operator=(const UnitAnimations& t_other) = delete;
        UnitAnimations& operator=(UnitAnimations&& t_other) noexcept = delete;

        ~UnitAnimations() noexcept = default;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * @brief Returns a reference to the whole animations container.
         * @return std::map
         */
        Animations& GetAnimations() noexcept { return m_animations; }

        /**
         * @brief Returns a const reference to the whole animations container.
         * @return std::map
         */
        const Animations& GetAnimations() const noexcept { return m_animations; }

        /**
         * @brief Returns a reference to the animation.
         * @param t_unitId The Id of the unit.
         * @param t_direction The direction of the unit.
         * @return Animation
         */
        Animation& GetAnimation(const UnitId t_unitId, const Direction t_direction)
        {
            return *m_animations.at(std::make_pair(t_unitId, t_direction));
        }

        /**
         * @brief Returns a const reference to the animation.
         * @param t_unitId The ID of the unit.
         * @param t_direction The direction of the unit.
         * @return Animation
         */
        const Animation& GetAnimation(const UnitId t_unitId, const Direction t_direction) const
        {
            return *m_animations.at(std::make_pair(t_unitId, t_direction));
        }

        /**
         * @brief Computes the sprite `Direction` from a given direction vector.
         * @param t_vector A direction vector.
         * @return Direction
         */
        static Direction GetDirectionByVec(const sf::Vector2f& t_vector)
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

            SG_ISLANDS_WARN("[UnitAnimations::GetDirectionByVec()] Return the default direction (E_DIRECTION) for angle {}.", angleDeg);

            return Direction::E_DIRECTION;
        }

    protected:

    private:
        /**
         * @brief Saves the animations of a moveable unit.
         */
        Animations m_animations;

        //-------------------------------------------------
        // Load Data
        //-------------------------------------------------

        /**
         * @brief Load `Unit`s.
         * @param t_filename The given xml config file.
         */
        void LoadConfigFile(const core::Filename& t_filename)
        {
            SG_ISLANDS_INFO("[UnitAnimations::LoadConfigFile()] Open {} for reading units.", t_filename);

            tinyxml2::XMLDocument document;

            // load xml file
            core::XmlWrapper::LoadXmlFile(t_filename, document);

            // get `<animations>` element
            const auto animationsElement{ core::XmlWrapper::GetFirstChildElement(document, "animations") };

            // get `<units>` element
            const auto unitsElement{ core::XmlWrapper::GetFirstChildElement(animationsElement, "units") };

            // get the units base directory from the `<dir>` element
            const auto unitsDir{ core::XmlWrapper::GetStringFromXmlElement(unitsElement, "dir") };

            SG_ISLANDS_INFO("[UnitAnimations::LoadConfigFile()] Loading units from {} ...", unitsDir);

            // get each `<unit>`
            for (auto unit{ unitsElement->FirstChildElement("unit") }; unit != nullptr; unit = unit->NextSiblingElement())
            {
                int idAttr;
                core::XmlWrapper::QueryAttribute(unit, "id", &idAttr);

                int framesAttr;
                core::XmlWrapper::QueryAttribute(unit, "frames", &framesAttr);

                auto dirAttr{ core::XmlWrapper::GetAttribute(unit, "dir") };
                auto unitDir{ unitsDir + dirAttr };

                // for each direction
                for (auto& direction : DIRECTIONS)
                {
                    // create new `Animation` for this direction
                    auto animation{ std::make_unique<Animation>() };

                    // create all frames for this `Animation`
                    for (auto i{ 0 }; i < framesAttr; ++i)
                    {
                        // create filename with four leading zeros
                        auto str{ std::to_string(i) };
                        auto filename{ std::string(4 - str.length(), '0') + str + ".png" };

                        auto directionDir{ std::to_string(static_cast<int>(direction)) };

                        // add frame
                        animation->AddFrame(unitDir + directionDir + "/" + filename, FRAME_DURATION);
                    }

                    // save `Animation` for the direction
                    m_animations.emplace(std::make_pair(idAttr, direction), std::move(animation));
                }
            }

            SG_ISLANDS_INFO("[UnitAnimations::LoadConfigFile()] Successfully loaded {} animations for {} units.", m_animations.size(), m_animations.size() / NUMBER_OF_DIRECTIONS);
        }
    };
}
