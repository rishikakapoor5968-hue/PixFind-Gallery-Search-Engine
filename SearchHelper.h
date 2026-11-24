
#ifndef SEARCH_HELPER_H
#define SEARCH_HELPER_H

#include "pixfind_backend.h"

#include <string>
#include <vector>
#include <cctype>

// Case-insensitive text matching
inline bool containsIgnoreCase(
    const char* field,
    const std::string& query)
{
    if (field == nullptr)
        return false;

    std::string text = field;
    std::string keyword = query;

    for (char& ch : text)
    {
        ch = static_cast<char>(
            std::tolower(static_cast<unsigned char>(ch))
        );
    }

    for (char& ch : keyword)
    {
        ch = static_cast<char>(
            std::tolower(static_cast<unsigned char>(ch))
        );
    }

    return text.find(keyword) != std::string::npos;
}

// Search by title, filename, tags and description
inline std::vector<int> searchMediaRecords(
    const std::vector<MediaInfo>& media,
    const std::string& query)
{
    std::vector<int> results;

    if (query.empty())
        return results;

    for (int i = 0; i < static_cast<int>(media.size()); i++)
    {
        const MediaInfo& item = media[i];

        if (containsIgnoreCase(item.title, query) ||
            containsIgnoreCase(item.fileName, query) ||
            containsIgnoreCase(item.tags, query) ||
            containsIgnoreCase(item.description, query))
        {
            results.push_back(i);
        }
    }

    return results;
}

#endif
