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
        explicit Animation(sf::Sprite& t_sprite)
            : m_sprite{ t_sprite }
        {}

        auto GetLength() const { return m_totalLength; }

        void AddFrame(const Frame& t_frame)
        {
            m_totalLength += t_frame.duration;
            m_frames.push_back(t_frame); // todo
        }

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
        std::vector<Frame> m_frames;

        double m_totalLength{ 0.0 };
        double m_progress{ 0.0 };

        sf::Sprite& m_sprite;
    };
}
