// This file is part of the SgIslands package.
// 
// Filename: BitmaskManager.hpp
// Created:  07.03.2019
// Updated:  09.03.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <map>
#include "Log.hpp"

namespace sg::islands::core
{
    class BitmaskManager
    {
    public:
        using Pixel = sf::Uint8;
        using Bitmask = std::vector<Pixel>;
        using Bitmasks = std::map<const sf::Texture*, Bitmask>;

        //-------------------------------------------------
        // Ctor. && Dtor.
        //-------------------------------------------------

        BitmaskManager() = default;

        BitmaskManager(const BitmaskManager& t_other) = delete;
        BitmaskManager(BitmaskManager&& t_other) noexcept = delete;
        BitmaskManager& operator=(const BitmaskManager& t_other) = delete;
        BitmaskManager& operator=(BitmaskManager&& t_other) noexcept = delete;

        ~BitmaskManager() noexcept = default;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        const Bitmask& GetBitmask(const sf::Texture* const t_texture)
        {
            try
            {
                return m_bitmasks.at(t_texture);
            }
            catch (const std::out_of_range& exception)
            {
                SG_ISLANDS_ERROR("[BitmaskManager::GetBitmask()] Out of range exception: ", exception.what());
                THROW_SG_EXCEPTION("[BitmaskManager::GetBitmask()] Invalid key.");
            }
        }

        Pixel GetPixel(const sf::Texture* const t_texture, const std::size_t t_x, const std::size_t t_y)
        {
            // todo
            if (t_x > t_texture->getSize().x || t_y > t_texture->getSize().y)
            {
                return 0;
            }

            return GetBitmask(t_texture)[t_x + t_y * t_texture->getSize().x];
        }

        static Pixel GetPixel(const Bitmask& t_bitmask, const sf::Texture* const t_texture, const std::size_t t_x, const std::size_t t_y)
        {
            // todo
            if (t_x > t_texture->getSize().x || t_y > t_texture->getSize().y)
            {
                return 0;
            }

            return t_bitmask[t_x + t_y * t_texture->getSize().x];
        }

        //-------------------------------------------------
        // Create
        //-------------------------------------------------

        void CreateTextureAndBitmask(sf::Texture& t_loadIntoTexture, const Filename& t_filename)
        {
            sf::Image image;
            if (!image.loadFromFile(t_filename))
            {
                SG_ISLANDS_ERROR("[BitmaskManager::CreateTextureAndBitmask()] Error loading image from file {}.", t_filename);
                THROW_SG_EXCEPTION("[BitmaskManager::CreateTextureAndBitmask()] Error loading image from file.");
            }

            if (!t_loadIntoTexture.loadFromImage(image))
            {
                SG_ISLANDS_ERROR("[BitmaskManager::CreateTextureAndBitmask()] Error loading texture from image.");
                THROW_SG_EXCEPTION("[BitmaskManager::CreateTextureAndBitmask()] Error loading texture from image.");
            }

            CreateBitmask(&t_loadIntoTexture, image);
        }

        void CreateBitmask(const sf::Texture* const t_texture, const sf::Image& t_image)
        {
            Bitmask bitmask;
            bitmask.resize(t_texture->getSize().y * t_texture->getSize().x);

            for (auto y{ 0u }; y < t_texture->getSize().y; ++y)
            {
                for (auto x{ 0u }; x < t_texture->getSize().x; ++x)
                {
                    bitmask[x + y * t_texture->getSize().x] = t_image.getPixel(x, y).a;
                }
            }

            m_bitmasks.emplace(t_texture, bitmask);
        }

    protected:

    private:
        Bitmasks m_bitmasks;
    };
}
