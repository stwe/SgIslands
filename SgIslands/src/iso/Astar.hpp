// This file is part of the SgIslands package.
// 
// Filename: Astar.hpp
// Created:  02.02.2019
// Updated:  10.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <vector>
#include <stack>
#include "Node.hpp"
#include "IsoMath.hpp"

namespace sg::islands::iso
{
    class Astar
    {
    public:
        //-------------------------------------------------
        // Ctor. && Dtor.
        //-------------------------------------------------

        Astar() = delete;

        explicit Astar(Map& t_map)
            : m_map{ t_map }
            , m_width{ t_map.GetMapWidth() }
            , m_height{ t_map.GetMapHeight() }
        {
        }

        Astar(const Astar& t_other) = delete;
        Astar(Astar&& t_other) noexcept = delete;
        Astar& operator=(const Astar& t_other) = delete;
        Astar& operator=(Astar&& t_other) noexcept = delete;

        ~Astar() noexcept = default;

        //-------------------------------------------------
        // A* Pathfinding
        //-------------------------------------------------

        std::vector<Node> FindPath(const Node& t_node, const Node& t_endNode) const
        {
            std::vector<Node> empty;

            if (!IsValid(t_endNode))
            {
                SG_ISLANDS_DEBUG("[Astar::FindPath()] Destination is an obstacle.");
                return empty;
            }

            if (IsEndNode(t_node, t_endNode))
            {
                SG_ISLANDS_DEBUG("[Astar::FindPath()] You are the target.");
                return empty;
            }

            std::vector<bool> closedList;
            std::vector<Node> allList;

            for (auto y{ 0 }; y < m_height; ++y)
            {
                for (auto x{ 0 }; x < m_width; ++x)
                {
                    Node node;
                    node.position = sf::Vector2i(x, y);
                    node.parentPosition = sf::Vector2i(-1, -1);
                    node.g = std::numeric_limits<float>::max();
                    node.h = std::numeric_limits<float>::max();
                    node.f = std::numeric_limits<float>::max();

                    allList.push_back(node);
                    closedList.push_back(false);
                }
            }

            assert(closedList.size() == static_cast<std::size_t>(m_width * m_height));
            assert(allList.size() == static_cast<std::size_t>(m_width * m_height));

            // init start node
            auto xPos{ t_node.position.x };
            auto yPos{ t_node.position.y };
            auto index{ IsoMath::From2DTo1D(xPos, yPos, m_width) };

            allList[index].parentPosition.x = xPos;
            allList[index].parentPosition.y = yPos;
            allList[index].g = 0.0f;
            allList[index].h = 0.0f;
            allList[index].f = 0.0f;

            // add start node to open list
            std::vector<Node> openList;
            openList.push_back(allList[index]);

            // loop until find the end node
            while (!openList.empty() && openList.size() < static_cast<std::size_t>(m_width * m_height))
            {
                Node node;
                do
                {
                    auto tmp{ std::numeric_limits<float>::max() };
                    std::vector<Node>::iterator itNode;
                    for (auto it{ openList.begin() }; it != openList.end(); it = std::next(it))
                    {
                        const auto openListNode{ *it };
                        if (openListNode.f < tmp)
                        {
                            tmp = openListNode.f;
                            itNode = it;
                        }
                    }

                    node = *itNode;
                    openList.erase(itNode);
                }
                while (!IsValid(node));

                xPos = node.position.x;
                yPos = node.position.y;
                index = IsoMath::From2DTo1D(xPos, yPos, m_width);

                closedList[index] = true;

                // for each neighbour
                for (auto yOffset{ -1 }; yOffset <= 1; ++yOffset)
                {
                    for (auto xOffset{ -1 }; xOffset <= 1; ++xOffset)
                    {
                        const auto newXPos{ xPos + xOffset };
                        const auto newYPos{ yPos + yOffset };
                        const auto newIndex{ IsoMath::From2DTo1D(newXPos, newYPos, m_width) };

                        if (IsValid(newXPos, newYPos))
                        {
                            if (IsEndNode(newXPos, newYPos, t_endNode))
                            {
                                allList[newIndex].parentPosition.x = xPos;
                                allList[newIndex].parentPosition.y = yPos;

                                return MakePath(allList, t_endNode);
                            }

                            if (!closedList[newIndex])
                            {
                                const auto newG{ node.g + 1.0f };
                                const auto newH{ CalculateHeuristic(newXPos, newYPos, t_endNode) };
                                const auto newF{ newG + newH };

                                // check if this path is better than the one already present
                                if (allList[newIndex].f == std::numeric_limits<float>::max() ||
                                    allList[newIndex].f > newF)
                                {
                                    // update the details of this neighbour node
                                    allList[newIndex].parentPosition.x = xPos;
                                    allList[newIndex].parentPosition.y = yPos;
                                    allList[newIndex].g = newG;
                                    allList[newIndex].h = newH;
                                    allList[newIndex].f = newF;

                                    openList.push_back(allList[newIndex]);
                                }
                            }
                        }
                        else
                        {
                            //SG_ISLANDS_WARN("[Astar::FindPath()] Not valid position x: {}, y: {}", newXPos, newYPos);
                        }
                    }
                }
            }

            SG_ISLANDS_WARN("[Astar::FindPath()] Target not found.");

            return empty;
        }

    protected:

    private:
        Map& m_map;

        int m_width{ -1 };
        int m_height{ -1 };

        //-------------------------------------------------
        // Helper
        //-------------------------------------------------

        std::vector<Node> MakePath(const std::vector<Node>& t_nodes, const Node& t_endNode) const
        {
            auto xPos{ t_endNode.position.x };
            auto yPos{ t_endNode.position.y };
            auto index{ IsoMath::From2DTo1D(xPos, yPos, m_width) };

            std::stack<Node> path;
            std::vector<Node> usablePath;

            while (
                !(t_nodes[index].parentPosition.x == xPos &&
                    t_nodes[index].parentPosition.y == yPos) &&
                t_nodes[index].position.x != -1 && t_nodes[index].position.y != -1
                )
            {
                path.push(t_nodes[index]);
                const auto tempX{ t_nodes[index].parentPosition.x };
                const auto tempY{ t_nodes[index].parentPosition.y };
                xPos = tempX;
                yPos = tempY;
                index = IsoMath::From2DTo1D(xPos, yPos, m_width);
            }

            path.push(t_nodes[index]);

            while (!path.empty())
            {
                auto top{ path.top() };
                path.pop();
                usablePath.push_back(top);
            }

            return usablePath;
        }

        /**
         * @brief Calculate the Euclidean distance from a position to the end node.
         * @param t_x The x-position.
         * @param t_y The y-position.
         * @param t_endNode The end node.
         * @return float
         */
        static float CalculateHeuristic(const int t_x, const int t_y, const Node& t_endNode)
        {
            const auto xd{ t_endNode.position.x - t_x };
            const auto yd{ t_endNode.position.y - t_y };

            return static_cast<float>(sqrt(xd * xd + yd * yd)); // todo remove sqrt
        }

        /**
         * @brief Checks if a position is valid.
         * @param t_x The x-position.
         * @param t_y The y-position.
         * @return bool
         */
        bool IsValid(const int t_x, const int t_y) const
        {
            if (t_x < 0 || t_y < 0 || t_x >= m_width || t_y >= m_height)
            {
                return false;
            }

            // todo
            return m_map.IsDeepWater(t_x, t_y);
        }

        /**
         * @brief Checks if a position is valid.
         * @param t_node A `Node`.
         * @return bool
         */
        bool IsValid(const Node& t_node) const
        {
            return IsValid(t_node.position.x, t_node.position.y);
        }

        /**
         * @brief Checks if the passed `Node` is the end `Node`.
         * @param t_node A `Node`
         * @param t_endNode The end `Node`
         * @return bool
         */
        static bool IsEndNode(const Node& t_node, const Node& t_endNode)
        {
            return t_node == t_endNode;
        }

        /**
         * @brief Checks if the passed position is the end `Node`.
         * @param t_x The x-position.
         * @param t_y The y-position.
         * @param t_endNode The end `Node`.
         * @return bool
         */
        static bool IsEndNode(const int t_x, const int t_y, const Node& t_endNode)
        {
            return t_x == t_endNode.position.x && t_y == t_endNode.position.y;
        }
    };
}
