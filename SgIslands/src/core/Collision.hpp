#pragma once

#include <SFML/Graphics/Sprite.hpp>
//#include <SFML/Graphics/RectangleShape.hpp>
#include "BitmaskManager.hpp"

namespace sg::islands::core
{
    class Collision
    {
    public:
        /**
         * @brief Test for a collision between two sprites by comparing the alpha values of overlapping pixels.
         *        Supports scaling and rotation.
         * @param t_sprite1 A Sprite.
         * @param t_sprite2 Other Sprite.
         * @param t_alphaLimit The threshold at which a pixel becomes "solid". If AlphaLimit is 127, a pixel with
         *                     alpha value 128 will cause a collision and a pixel with alpha value 126 will not.
         * @param t_bitmaskManager A reference to the BitmaskManager.
         * @return bool
         */
        static auto PixelPerfect(
            const sf::Sprite& t_sprite1,
            const sf::Sprite& t_sprite2,
            const sf::Uint8 t_alphaLimit,
            BitmaskManager& t_bitmaskManager
        )
        {
            sf::FloatRect intersection;

            if (t_sprite1.getGlobalBounds().intersects(t_sprite2.getGlobalBounds(), intersection))
            {
                const auto& o1SubRect{ t_sprite1.getTextureRect() };
                const auto& o2SubRect{ t_sprite2.getTextureRect() };

                const auto& mask1{ t_bitmaskManager.GetBitmask(t_sprite1.getTexture()) };
                const auto& mask2{ t_bitmaskManager.GetBitmask(t_sprite2.getTexture()) };

                const auto il{ static_cast<int>(intersection.left) };
                const auto iw{ static_cast<int>(intersection.width) };

                const auto it{ static_cast<int>(intersection.top) };
                const auto ih{ static_cast<int>(intersection.height) };

                /*
                t_rect = sf::RectangleShape(sf::Vector2f(intersection.width, intersection.height));
                t_rect.setPosition(intersection.left, intersection.top);
                t_rect.setFillColor(sf::Color(0, 0, 128, 64));
                */
 
                for (auto i{ il }; i < il + iw; ++i)
                {
                    for (auto j{ it }; j < it + ih; ++j)
                    {
                        const auto o1V{ t_sprite1.getInverseTransform().transformPoint(static_cast<float>(i), static_cast<float>(j)) };
                        const auto o2V{ t_sprite2.getInverseTransform().transformPoint(static_cast<float>(i), static_cast<float>(j)) };

                        if (o1V.x > 0 && o1V.y > 0 && o2V.x > 0 && o2V.y > 0 &&
                            o1V.x < o1SubRect.width && o1V.y < o1SubRect.height &&
                            o2V.x < o2SubRect.width && o2V.y < o2SubRect.height)
                        {
                            if (BitmaskManager::GetPixel(mask1, t_sprite1.getTexture(), static_cast<int>(o1V.x) + o1SubRect.left, static_cast<int>(o1V.y) + o1SubRect.top) > t_alphaLimit &&
                                BitmaskManager::GetPixel(mask2, t_sprite2.getTexture(), static_cast<int>(o2V.x) + o2SubRect.left, static_cast<int>(o2V.y) + o2SubRect.top) > t_alphaLimit)
                            {
                                return true;
                            }
                        }
                    }
                }
            }

            return false;
        }

    protected:

    private:

    };
}
