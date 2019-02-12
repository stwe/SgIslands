// This file is part of the SgIslands package.
// 
// Filename: Animation.hpp
// Created:  26.01.2019
// Updated:  12.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Time.hpp>

namespace sg::islands::iso
{
    class Animation
    {
    public:
        static constexpr auto FRAME_TIME{ 0.2f };

        using Frame = sf::Texture;
        using Frames = std::vector<Frame>;

        //-------------------------------------------------
        // Ctor. && Dtor.
        //-------------------------------------------------

        Animation() = delete;

        Animation(const int t_tileHeight, const TerrainType& t_terrainType, const sf::Time& t_frameTime = sf::seconds(FRAME_TIME))
            : m_tileHeight{ t_tileHeight }
            , m_terrainType{ t_terrainType }
            , m_frameTime{ t_frameTime }
        {}

        Animation(const Animation& t_other) = delete;
        Animation(Animation&& t_other) noexcept = delete;
        Animation& operator=(const Animation& t_other) = delete;
        Animation& operator=(Animation&& t_other) noexcept = delete;

        ~Animation() noexcept = default;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        const Frames& GetFrames() const noexcept { return m_frames; }
        const sf::Sprite& GetSprite() const noexcept { return m_sprite; }
        sf::Sprite& GetSprite() noexcept { return m_sprite; }

        auto GetTileHeight() const { return m_tileHeight; }
        auto GetTerrainType() const { return m_terrainType; }

        //-------------------------------------------------
        // Add Frame
        //-------------------------------------------------

        void AddFrame(const core::Filename& t_filename)
        {
            Frame frame;

            // load frame texture
            const auto result{ frame.loadFromFile(t_filename) };
            if (!result)
            {
                throw std::runtime_error("[Animation::AddFrame()] Failed to load texture " + t_filename);
            }

            // save frame
            m_frames.push_back(frame);
        }

        //-------------------------------------------------
        // Update
        //-------------------------------------------------

        void Update(const sf::Time& t_dt)
        {
            // add delta time
            m_currentTime += t_dt;

            // if current time is bigger then the frame time advance one frame
            if (m_currentTime >= m_frameTime)
            {
                // reset time, but keep the remainder
                m_currentTime = sf::microseconds(m_currentTime.asMicroseconds() % m_frameTime.asMicroseconds());

                // get next Frame index
                if (m_currentFrame + 1 < m_frames.size())
                {
                    m_currentFrame++;
                }
                else
                {
                    // animation has ended
                    m_currentFrame = 0; // reset to start
                }

                // set the current frame
                m_sprite.setTexture(m_frames[m_currentFrame]);
            }
        }

    protected:

    private:
        Frames m_frames;
        sf::Sprite m_sprite;

        int m_tileHeight{ -1 };
        TerrainType m_terrainType;

        std::size_t m_currentFrame{ 0 };

        sf::Time m_frameTime;
        sf::Time m_currentTime{ sf::Time::Zero };
    };
}
