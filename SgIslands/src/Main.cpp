// This file is part of the SgIslands package.
// 
// Filename: Main.cpp
// Created:  20.01.2019
// Updated:  15.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#define _USE_MATH_DEFINES

#include "core/Application.hpp"
#include "core/Log.hpp"

int main()
{
    sg::islands::core::Log::Init();
    SG_ISLANDS_INFO("[main()] Logger initialized.");

    try
    {
        sg::islands::core::Application app{ "res/config/app.xml" };
        app.Run();
    }
    catch (const std::exception& e)
    {
        SG_ISLANDS_ERROR("Exception: {}", e.what());
    }

    return 0;
}
