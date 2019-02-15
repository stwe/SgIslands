// This file is part of the SgIslands package.
// 
// Filename: XmlWrapper.hpp
// Created:  20.01.2019
// Updated:  15.02.2019
// Author:   stwe
// 
// License:  MIT
// 
// 2019 (c) stwe <https://github.com/stwe/SgIslands>

#pragma once

#include "tinyxml2/tinyxml2.h"
#include "Types.hpp"

namespace sg::islands::core
{
    class XmlWrapper
    {
    public:
        static void LoadXmlFile(const Filename& t_filename, tinyxml2::XMLDocument& t_document)
        {
            const auto result{ t_document.LoadFile(t_filename.c_str()) };
            if (result != tinyxml2::XML_SUCCESS)
            {
                THROW_SG_EXCEPTION("[XmlWrapper::LoadXmlFile()] XMLError: " + std::to_string(result));
            }
        }

        static std::string GetStringFromXmlElement(const tinyxml2::XMLElement* t_element, const std::string& t_name)
        {
            const auto xmlElement{ t_element->FirstChildElement(t_name.c_str()) };
            if (!xmlElement)
            {
                THROW_SG_EXCEPTION("[XmlWrapper::GetStringFromXmlElement()] Xml element is missing: " + t_name);
            }

            return std::string(xmlElement->GetText());
        }

        static int GetIntFromXmlElement(const tinyxml2::XMLElement* t_element, const std::string& t_name)
        {
            return static_cast<int>(std::strtol(GetStringFromXmlElement(t_element, t_name).c_str(), nullptr, 10));
        }

        static tinyxml2::XMLElement* GetFirstChildElement(tinyxml2::XMLDocument& t_document, const std::string& t_name)
        {
            const auto result{ t_document.FirstChildElement(t_name.c_str()) };
            if (!result)
            {
                THROW_SG_EXCEPTION("[XmlWrapper::GetFirstChildElement()] Xml element is missing: " + t_name);
            }

            return result;
        }

        static tinyxml2::XMLElement* GetFirstChildElement(tinyxml2::XMLElement* t_element, const std::string& t_name)
        {
            const auto result{ t_element->FirstChildElement(t_name.c_str()) };
            if (!result)
            {
                THROW_SG_EXCEPTION("[XmlWrapper::GetFirstChildElement()] Xml element is missing: " + t_name);
            }

            return result;
        }

        static std::string GetAttribute(const tinyxml2::XMLElement* t_element, const std::string& t_name)
        {
            const auto attr{ t_element->Attribute(t_name.c_str()) };
            if (!attr)
            {
                THROW_SG_EXCEPTION("[XmlWrapper::GetAttribute()] Error reading attribute: " + t_name);
            }

            return std::string(attr);
        }

        static void QueryAttribute(tinyxml2::XMLElement* t_element, const std::string& t_name, bool* t_value)
        {
            const auto error{ t_element->QueryAttribute(t_name.c_str(), t_value) };
            if (error != tinyxml2::XML_SUCCESS)
            {
                THROW_SG_EXCEPTION("[XmlWrapper::QueryAttribute()] Error reading attribute: " + t_name);
            }
        }

        static void QueryAttribute(tinyxml2::XMLElement* t_element, const std::string& t_name, int* t_value)
        {
            const auto error{ t_element->QueryAttribute(t_name.c_str(), t_value) };
            if (error != tinyxml2::XML_SUCCESS)
            {
                THROW_SG_EXCEPTION("[XmlWrapper::QueryAttribute()] Error reading attribute: " + t_name);
            }
        }

    protected:

    private:
    };
}
