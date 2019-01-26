// This file is part of the SgIslands package.
// 
// Filename: Log.hpp
// Created:  20.01.2019
// Updated:  23.01.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace sg::islands::core
{
    class Log
    {
    public:
        using LoggerSharedPtr = std::shared_ptr<spdlog::logger>;

        static void Init()
        {
            spdlog::set_pattern("%^[%T] %n: %v%$");
            m_coreLogger = spdlog::stdout_color_mt("SG_ISLANDS");
            m_coreLogger->set_level(spdlog::level::trace);
        }

        static LoggerSharedPtr& GetCoreLogger() { return m_coreLogger; }
    protected:

    private:
        inline static LoggerSharedPtr m_coreLogger;
    };

    #define SG_ISLANDS_TRACE(...) ::sg::islands::core::Log::GetCoreLogger()->trace(__VA_ARGS__)
    #define SG_ISLANDS_DEBUG(...) ::sg::islands::core::Log::GetCoreLogger()->debug(__VA_ARGS__)
    #define SG_ISLANDS_INFO(...)  ::sg::islands::core::Log::GetCoreLogger()->info(__VA_ARGS__)
    #define SG_ISLANDS_WARN(...)  ::sg::islands::core::Log::GetCoreLogger()->warn(__VA_ARGS__)
    #define SG_ISLANDS_ERROR(...) ::sg::islands::core::Log::GetCoreLogger()->error(__VA_ARGS__)
    #define SG_ISLANDS_CRITICAL(...) ::sg::islands::core::Log::GetCoreLogger()->critical(__VA_ARGS__)
}
