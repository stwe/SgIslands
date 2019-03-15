// This file is part of the SgIslands package.
// 
// Filename: Events.hpp
// Created:  12.03.2019
// Updated:  15.03.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <entityx/entityx.h>

namespace sg::islands::ecs
{
    struct CollisionEvent : public entityx::Event<CollisionEvent>
    {
        CollisionEvent(const entityx::Entity::Id t_leftEntityId, const entityx::Entity::Id t_rightEntityId)
            : leftEntityId{ t_leftEntityId }
            , rightEntityId{ t_rightEntityId }
        {}

        entityx::Entity::Id leftEntityId;
        entityx::Entity::Id rightEntityId;
    };
}
