
#ifndef PIXFIND_BACKEND_H
#define PIXFIND_BACKEND_H

#include <string>
#include <vector>

struct MediaInfo
{
    int id;
    char fileName[260];
    char fileType[10];
    char title[50];
    char tags[100];
    char date[20];
    char description[100];
};

std::string selectMediaFile();

bool saveMediaFile(
    std::string path,
    std::string title,
    std::string tags,
    std::string date,
    std::string description
);

std::vector<MediaInfo> getAllMedia();
std::vector<int> getFavoriteIDs();
bool toggleFavoriteByID(int id);

#endif
