// This file is part of the SgIslands package.
// 
// Filename: ResourceHolder.hpp
// Created:  20.01.2019
// Updated:  15.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <cassert>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <map>
#include <memory>
#include "Types.hpp"

namespace sg::islands::core
{
    template <typename TId, typename TResource>
    class ResourceHolder
    {
    public:
        using ResourceUniquePtr = std::unique_ptr<TResource>;
        using ResourceMap = std::map<TId, ResourceUniquePtr>;

        //-------------------------------------------------
        // Ctor. && Dtor.
        //-------------------------------------------------

        ResourceHolder() = default;

        ResourceHolder(const ResourceHolder& t_other) = delete;
        ResourceHolder(ResourceHolder&& t_other) noexcept = delete;
        ResourceHolder& operator=(const ResourceHolder& t_other) = delete;
        ResourceHolder& operator=(ResourceHolder&& t_other) noexcept = delete;

        ~ResourceHolder() noexcept = default;

        //-------------------------------------------------
        // Getter
        //-------------------------------------------------

        TResource& GetResource(const TId t_id)
        {
            try
            {
                return *m_resourceMap.at(t_id);
            }
            catch (const std::out_of_range& exception)
            {
                SG_ISLANDS_ERROR("[ResourceHolder::GetResource()] Out of range exception: ", exception.what());
                THROW_SG_EXCEPTION("[ResourceHolder::GetResource()] Invalid key.");
            }
        }

        const TResource& GetResource(const TId t_id) const
        {
            try
            {
                return *m_resourceMap.at(t_id);
            }
            catch (const std::out_of_range& exception)
            {
                SG_ISLANDS_ERROR("[ResourceHolder::GetResource()] Out of range exception: ", exception.what());
                THROW_SG_EXCEPTION("[ResourceHolder::GetResource()] Invalid key.");
            }
        }

        auto GetNumberOfElements() const { return m_resourceMap.size(); }

        //-------------------------------------------------
        // Load
        //-------------------------------------------------

        void Load(const TId t_id, const Filename& t_filename)
        {
            ResourceUniquePtr resourceUniquePtr{ std::make_unique<TResource>() };
            assert(resourceUniquePtr);

            if (!resourceUniquePtr->loadFromFile(t_filename))
            {
                THROW_SG_EXCEPTION("[ResourceHolder::Load()] Failed to load resource " + t_filename);
            }

            m_resourceMap.emplace(t_id, std::move(resourceUniquePtr));
        }

    protected:

    private:
        ResourceMap m_resourceMap;
    };

    using TextureHolder = ResourceHolder<TextureId, sf::Texture>;
    using Tileset = ResourceHolder<TextureId, sf::Texture>;
    using FontHolder = ResourceHolder<FontId, sf::Font>;
}
