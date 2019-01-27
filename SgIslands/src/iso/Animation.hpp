// This file is part of the SgIslands package.
// 
// Filename: Animation.hpp
// Created:  26.01.2019
// Updated:  27.01.2019
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
    struct Frame
    {
        sf::Texture texture;
        double duration{ 0.0 };
    };

    class Animation
    {
    public:
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

        const Frames& GetFrames() const noexcept { return m_frames; }
        sf::Sprite& GetSprite() noexcept { return m_sprite; }

        auto GetLength() const { return m_totalLength; }

        //-------------------------------------------------
        // Add Frames
        //-------------------------------------------------

        void AddFrame(const core::Filename& t_filename, const double t_duration)
        {
            Frame frame;

            // load frame texture
            const auto result{ frame.texture.loadFromFile(t_filename) };
            if (!result)
            {
                SG_ISLANDS_ERROR("[Animation::AddFrame()] Error loading texture.");
                exit(EXIT_FAILURE);
            }

            // set duration
            frame.duration = t_duration;
            m_totalLength += frame.duration;

            // save frame
            m_frames.push_back(frame);
        }

        //-------------------------------------------------
        // Update
        //-------------------------------------------------

        void Update(sf::Time t_dt)
        {
            m_progress += t_dt.asSeconds();
            auto p{ m_progress };

            for (auto i{ 0u }; i < m_frames.size(); ++i)
            {
                p -= m_frames[i].duration;

                if (p > 0.0 && &(m_frames[i]) == &(m_frames.back())) {
                    i = 0;    // start over from the beginning
                    continue; // break off the loop and start where i is
                }

                // if we have progressed OR if we're on the last frame, apply and stop.
                if (p <= 0.0 || &(m_frames[i]) == &m_frames.back())
                {
                    m_sprite.setTexture(m_frames[i].texture);
                    m_sprite.setScale(0.25f, 0.25f);
                    break; // we found our frame
                }
            }
        }

    protected:

    private:
        Frames m_frames;
        sf::Sprite m_sprite;

        double m_totalLength{ 0.0 };
        double m_progress{ 0.0 };
    };
}
