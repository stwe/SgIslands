// This file is part of the SgIslands package.
// 
// Filename: Unit.hpp
// Created:  26.01.2019
// Updated:  27.01.2019
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
    class Unit
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

        Unit() = delete;

        explicit Unit(const core::Filename& t_filename)
        {
            LoadConfigFile(t_filename);
        }

        Unit(const Unit& t_other) = delete;
        Unit(Unit&& t_other) noexcept = delete;
        Unit& operator=(const Unit& t_other) = delete;
        Unit& operator=(Unit&& t_other) noexcept = delete;

        ~Unit() noexcept = default;

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
            SG_ISLANDS_INFO("[Unit::LoadConfigFile()] Open {} for reading units.", t_filename);

            tinyxml2::XMLDocument document;

            // load xml file
            core::XmlWrapper::LoadXmlFile(t_filename, document);

            // get `<animations>` element
            const auto animationsElement{ core::XmlWrapper::GetFirstChildElement(document, "animations") };

            // get `<units>` element
            const auto unitsElement{ core::XmlWrapper::GetFirstChildElement(animationsElement, "units") };

            // get the units base directory from the `<dir>` element
            const auto unitsDir{ core::XmlWrapper::GetStringFromXmlElement(unitsElement, "dir") };

            SG_ISLANDS_INFO("[Unit::LoadConfigFile()] Loading units from {} ...", unitsDir);

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

            SG_ISLANDS_INFO("[Unit::LoadConfigFile()] Successfully loaded {} animations for {} units.", m_animations.size(), m_animations.size() / NUMBER_OF_DIRECTIONS);
        }
    };
}
