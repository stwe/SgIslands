// This file is part of the SgIslands package.
// 
// Filename: ResourceHolder.hpp
// Created:  20.01.2019
// Updated:  23.01.2019
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
#include "Log.hpp"

namespace sg::islands::core
{
    template <typename TId, typename TResource>
    class ResourceHolder
    {
    public:
        using ResourceUniquePtr = std::unique_ptr<TResource>;

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

        TResource& GetResource(TId t_id) { return *m_resourceMap.at(t_id); }
        const TResource& GetResource(TId t_id) const { return *m_resourceMap.at(t_id); }

        auto GetNumberOfElements() const { return m_resourceMap.size(); }

        //-------------------------------------------------
        // Load
        //-------------------------------------------------

        void Load(TId t_id, const Filename& t_filename)
        {
            ResourceUniquePtr resourceUniquePtr{ std::make_unique<TResource>() };
            assert(resourceUniquePtr);

            if (!resourceUniquePtr->loadFromFile(t_filename))
            {
                SG_ISLANDS_ERROR("[ResourceHolder::Load()]: Failed to load resource {}: " + t_filename);
            }

            InsertResource(t_id, std::move(resourceUniquePtr));
        }

        template <typename TParam>
        void Load(TId t_id, const Filename& t_filename, const TParam& t_param)
        {
            ResourceUniquePtr resourceUniquePtr{ std::make_unique<TResource>() };
            assert(resourceUniquePtr);

            if (!resourceUniquePtr->loadFromFile(t_filename))
            {
                SG_ISLANDS_ERROR("[ResourceHolder::Load()]: Failed to load resource {}: " + t_filename);
            }

            InsertResource(t_id, std::move(resourceUniquePtr));
        }

    protected:

    private:
        std::map<TId, ResourceUniquePtr> m_resourceMap;

        void InsertResource(TId t_id, ResourceUniquePtr t_resource)
        {
            m_resourceMap.emplace(t_id, std::move(t_resource));
        }
    };

    using TextureHolder = ResourceHolder<TextureId, sf::Texture>;
    using FontHolder = ResourceHolder<FontId, sf::Font>;
}
