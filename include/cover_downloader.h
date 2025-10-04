#ifndef NETEASE_MUSIC_COVER_DOWNLOADER_H
#define NETEASE_MUSIC_COVER_DOWNLOADER_H

#include <string>
#include <vector>
#include <memory>

struct SongInfo {
    std::string imageUrl;
    std::string songName;
    std::string artistName;
};

struct AlbumInfo {
    std::string imageUrl;
    std::string albumName;
};

class CoverDownloader {
private:
    std::string baseUrl;
    
public:
    CoverDownloader();
    
    // Safe request function with retry logic
    std::vector<char> safeRequestBinary(const std::string& url, int retries = 2);
    std::string safeRequest(const std::string& url, int retries = 2);
    
    // Get song information
    SongInfo getSongInfo(const std::string& songId);
    
    // Get album information
    AlbumInfo getAlbumInfo(const std::string& albumId);
    
    // Download cover
    bool downloadCover(const std::string& idOrUrl, int formatType, int index, 
                      std::vector<std::string>& failList);
    
    // Human-like delay
    void humanDelay();
};

#endif // NETEASE_MUSIC_COVER_DOWNLOADER_H