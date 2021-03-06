// This file is part of the SgIslands package.
// 
// Filename: Animation.hpp
// Created:  26.01.2019
// Updated:  25.02.2019
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

        Animation() = default;

        Animation(const Animation& t_other) = delete;
        Animation(Animation&& t_other) noexcept = delete;
        Animation& operator=(const Animation& t_other) = delete;
        Animation& operator=(Animation&& t_other) noexcept = delete;

        ~Animation() noexcept = default;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        /**
         * @brief Get a const reference to the `Animation` frames.
         * @return Const reference to `std::vector`
         */
        const Frames& GetFrames() const noexcept { return m_frames; }

        /**
         * @brief Get a reference to the `Animation` sprite.
         * @return Reference to `sf::Sprite`
         */
        sf::Sprite& GetSprite() noexcept { return m_sprite; }

        /**
         * @brief Get a const reference to the `Animation` sprite.
         * @return Const reference to `sf::Sprite`
         */
        const sf::Sprite& GetSprite() const noexcept { return m_sprite; }

        /**
         * @brief Returns the number of frames.
         * @return unsigned int
         */
        std::size_t GetNumberOfFrames() const { return m_frames.size(); }

        //-------------------------------------------------
        // Setter
        //-------------------------------------------------

        /**
         * @brief Set the frame duration time.
         * @param t_frameTime The frame time.
         */
        void SetFrameTime(const sf::Time& t_frameTime) { m_frameTime = t_frameTime; }

        /**
         * @brief Set a frame number.
         * @param t_frameNumber The frame number to set.
         */
        void SetFrameNumber(const std::size_t t_frameNumber)
        {
            assert(t_frameNumber < m_frames.size());

            m_sprite.setTexture(m_frames[t_frameNumber]);
        }

        //-------------------------------------------------
        // Add
        //-------------------------------------------------

        /**
         * @brief Add a texture as frame.
         * @param t_filename The file path.
         */
        void AddFrame(const core::Filename& t_filename)
        {
            Frame frame;

            // load frame texture
            const auto result{ frame.loadFromFile(t_filename) };
            if (!result)
            {
                THROW_SG_EXCEPTION("[Animation::AddFrame()] Failed to load texture " + t_filename);
            }

            // save frame
            m_frames.push_back(frame);
        }

        //-------------------------------------------------
        // Update
        //-------------------------------------------------

        /**
         * @brief Update this `Animation` and set the right frame/texture.
         * @param t_dt The delta time.
         */
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
        sf::Time m_frameTime{ sf::seconds(FRAME_TIME) };

        Frames m_frames;
        sf::Sprite m_sprite;

        std::size_t m_currentFrame{ 0 };
        sf::Time m_currentTime{ sf::Time::Zero };
    };
}
