// This file is part of the SgIslands package.
// 
// Filename: SgException.hpp
// Created:  15.02.2019
// Updated:  15.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include <sstream>
#include <stdexcept>
#include <string>

namespace sg::islands::core
{
    class SgException : public std::runtime_error
    {
    public:
        SgException(const std::string& t_arg, const std::string& t_file, const int t_line)
            : std::runtime_error(t_arg)
        {
            std::ostringstream o;
            o << t_file << ": " << t_line << ": " << t_arg;
            m_msg = o.str();
        }

        SgException(const SgException& t_other) = delete;
        SgException(SgException&& t_other) noexcept = delete;
        SgException& operator=(const SgException& t_other) = delete;
        SgException& operator=(SgException&& t_other) noexcept = delete;

        ~SgException() noexcept = default;

        const char *what() const noexcept override
        {
            return m_msg.c_str();
        }

    protected:

    private:
        std::string m_msg;
    };

#define THROW_SG_EXCEPTION(arg) throw sg::islands::core::SgException(arg, __FILE__, __LINE__);
}
