#include "cover_downloader.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
#include <regex>
#include <filesystem>
#include <vector>
#include <string>
#include <sstream>

// For HTTP requests, we need to include appropriate headers
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#endif

// Need to include Boost.Beast for HTTP operations
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>  // For SSL support in Beast
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl.hpp>

CoverDownloader::CoverDownloader() : baseUrl("https://music.163.com") {
}

std::vector<char> CoverDownloader::safeRequestBinary(const std::string& url, int retries) {
    namespace beast = boost::beast;
    namespace http = beast::http;
    namespace net = boost::asio;
    namespace ssl = net::ssl;
    using tcp = net::ip::tcp;

    // Parse URL
    std::string host, path;
    bool is_https = false;
    if (url.substr(0, 8) == "https://") {
        is_https = true;
        host = url.substr(8);
        size_t pos = host.find('/');
        if (pos != std::string::npos) {
            path = host.substr(pos);
            host = host.substr(0, pos);
        } else {
            path = "/";
        }
    } else if (url.substr(0, 7) == "http://") {
        host = url.substr(7);
        size_t pos = host.find('/');
        if (pos != std::string::npos) {
            path = host.substr(pos);
            host = host.substr(0, pos);
        } else {
            path = "/";
        }
    } else {
        return std::vector<char>(); // Return empty vector to indicate error
    }

    for (int attempt = 0; attempt <= retries; ++attempt) {
        try {
            if (attempt > 0) {
                double delay = Utils::getRandomDelay(2.0, 4.0);
                std::cout << "Waiting " << delay << " seconds before retrying..." << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(delay * 1000)));
            }

            net::io_context ioc;

            if (is_https) {
                // Create SSL context
                ssl::context ctx{ssl::context::tlsv12_client};
                ctx.set_verify_mode(ssl::verify_none); // In a real application, set appropriate verification

                // Look up the domain name
                tcp::resolver resolver{ioc};
                auto const results = resolver.resolve(host, "443");

                // Make the connection on the IP address we get from a lookup
                beast::tcp_stream stream{ioc};
                beast::get_lowest_layer(stream).connect(results);

                beast::ssl_stream<beast::tcp_stream&> ssl_stream{stream, ctx};

                // Set SNI Hostname (many hosts need this to handshake successfully)
                if (!SSL_set_tlsext_host_name(ssl_stream.native_handle(), host.c_str())) {
                    beast::error_code ec{static_cast<int>(::ERR_get_error()), net::error::get_ssl_category()};
                    throw beast::system_error{ec};
                }

                // Perform the SSL handshake
                ssl_stream.handshake(ssl::stream_base::client);

                // Set up an HTTP GET request message
                http::request<http::string_body> req{http::verb::get, path, 11};
                req.set(http::field::host, host);
                req.set(http::field::user_agent, Utils::getRandomUserAgent());
                req.set(http::field::accept, "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
                req.set(http::field::accept_language, "zh-CN,zh;q=0.9");
                req.set(http::field::accept_encoding, "identity");
                req.set(http::field::connection, "keep-alive");
                req.set(http::field::referer, "https://music.163.com/");

                // Send the HTTP request to the remote host
                http::write(ssl_stream, req);

                // This buffer is used for reading and must be persisted
                beast::flat_buffer buffer;

                // Declare a container to hold the response
                http::response<http::string_body> res;

                // Receive the HTTP response
                http::read(ssl_stream, buffer, res);

                // Gracefully close the stream
                beast::error_code ec;
                ssl_stream.shutdown(ec);

                if (ec == net::ssl::error::stream_truncated) {
                    // This is expected by the server, not an error
                    ec = {};
                }

                if (ec) {
                    throw beast::system_error{ec};
                }

                if (res.result_int() != 200) {
                    std::cout << "HTTP error: " << res.result_int() << std::endl;
                    if (attempt < retries) {
                        std::cout << "Preparing attempt " << (attempt + 2) << "..." << std::endl;
                        continue; // Try again
                    }
                    return std::vector<char>(); // Return empty vector
                }

                // Convert response body to vector<char>
                std::vector<char> response_data(res.body().begin(), res.body().end());
                return response_data;
            }
            else {
                // HTTP (non-SSL) version
                tcp::resolver resolver{ioc};
                beast::tcp_stream stream{ioc};

                // Look up the domain name
                auto const results = resolver.resolve(host, "80");

                // Make the connection on the IP address we get from a lookup
                beast::get_lowest_layer(stream).connect(results);

                // Set up an HTTP GET request message
                http::request<http::string_body> req{http::verb::get, path, 11};
                req.set(http::field::host, host);
                req.set(http::field::user_agent, Utils::getRandomUserAgent());
                req.set(http::field::accept, "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
                req.set(http::field::accept_language, "zh-CN,zh;q=0.9");
                req.set(http::field::accept_encoding, "gzip, deflate");
                req.set(http::field::connection, "keep-alive");

                // Randomly add Referer
                if (rand() % 2 == 0) {
                    req.set(http::field::referer, "https://music.163.com/");
                }

                // Send the HTTP request to the remote host
                http::write(stream, req);

                // This buffer is used for reading and must be persisted
                beast::flat_buffer buffer;

                // Declare a container to hold the response
                http::response<http::string_body> res;

                // Receive the HTTP response
                http::read(stream, buffer, res);

                // Gracefully close the socket
                beast::error_code ec;
                stream.socket().shutdown(tcp::socket::shutdown_both, ec);

                if (res.result_int() != 200) {
                    std::cout << "HTTP error: " << res.result_int() << std::endl;
                    if (attempt < retries) {
                        std::cout << "Preparing attempt " << (attempt + 2) << "..." << std::endl;
                        continue; // Try again
                    }
                    return std::vector<char>(); // Return empty vector
                }

                // Convert response body to vector<char>
                std::vector<char> response_data(res.body().begin(), res.body().end());
                return response_data;
            }
        } catch (const std::exception& e) {
            std::cout << "Attempt " << (attempt + 1) << " request failed: " << e.what() << std::endl;
            if (attempt < retries) {
                std::cout << "Preparing attempt " << (attempt + 2) << "..." << std::endl;
            }
        }
    }

    std::cout << "Multiple attempts failed, skipping this item" << std::endl;
    return std::vector<char>();
}

std::string CoverDownloader::safeRequest(const std::string& url, int retries) {
    auto data = safeRequestBinary(url, retries);
    if (data.empty()) {
        return "";
    }

    // Convert binary data to string (for HTML content)
    return std::string(data.begin(), data.end());
}

SongInfo CoverDownloader::getSongInfo(const std::string& songId) {
    std::string url = "https://music.163.com/song?id=" + songId;
    std::string response = safeRequest(url);

    if (response.empty()) {
        return { "", "song_" + songId, "Unknown Artist" };
    }

    SongInfo info;

    // Use regex to extract information
    std::regex title_regex(R"xxx(<meta property="og:title" content="([^"]+)")xxx");
    std::smatch title_match;
    if (std::regex_search(response, title_match, title_regex)) {
        std::string title_content = title_match[1].str();
        size_t pos = title_content.rfind(" - ");
        if (pos != std::string::npos) {
            info.songName = title_content.substr(pos + 3);
        } else {
            info.songName = title_content;
        }
    } else {
        info.songName = "song_" + songId;
    }

    // Try multiple artist regex patterns
    std::vector<std::string> artist_patterns = {
        R"xxx(<meta property="og:music:artist" content="([^"]+)")xxx",
        R"xxx(<p class="des s-fc4">歌手：<span title="([^"]+)">)xxx",
        R"xxx(<div class="f-thide s-fc4"><span title="([^"]+)">)xxx"
    };

    bool artist_found = false;
    std::string raw_artist;

    for (const auto& pattern : artist_patterns) {
        std::regex artist_regex(pattern);
        std::smatch artist_match;
        if (std::regex_search(response, artist_match, artist_regex)) {
            raw_artist = artist_match[1].str();
            artist_found = true;
            break;
        }
    }

    if (artist_found) {
        // Process artist name
        std::string result;

        // Split and process artist names (handle multiple artists separated by /)
        std::regex split_regex("/");
        std::sregex_token_iterator iter(raw_artist.begin(), raw_artist.end(), split_regex, -1);
        std::sregex_token_iterator end;

        for (; iter != end; ++iter) {
            std::string part = *iter;
            // Remove spaces
            part.erase(std::remove_if(part.begin(), part.end(), ::isspace), part.end());
            if (!result.empty()) result += "_";
            result += part;
        }

        info.artistName = result.empty() ? "Unknown Artist" : result;
    } else {
        info.artistName = "Unknown Artist";
    }

    // Fix image URL extraction regex
    std::regex img_regex(R"xxx(<meta property="og:image" content="([^"]+?)")xxx");
    std::smatch img_match;
    if (std::regex_search(response, img_match, img_regex)) {
        std::string img_url = img_match[1].str();
        // Remove URL parameters
        size_t param_pos = img_url.find("?");
        if (param_pos != std::string::npos) {
            info.imageUrl = img_url.substr(0, param_pos);
        } else {
            info.imageUrl = img_url;
        }
    }

    return info;
}

AlbumInfo CoverDownloader::getAlbumInfo(const std::string& albumId) {
    std::string url = "https://music.163.com/album?id=" + albumId;
    std::string response = safeRequest(url);

    if (response.empty()) {
        return { "", "album_" + albumId };
    }

    AlbumInfo info;

    // Extract album name
    std::regex title_regex(R"xxx(<meta property="og:title" content="([^"]+)")xxx");
    std::smatch title_match;
    if (std::regex_search(response, title_match, title_regex)) {
        info.albumName = title_match[1].str();
    } else {
        info.albumName = "album_" + albumId;
    }

    // Fix album cover extraction regex
    std::regex img_regex(R"xxx(<img[^>]*class="j-img"[^>]*src="([^"]+?)")xxx");
    std::smatch img_match;
    if (!std::regex_search(response, img_match, img_regex)) {
        // Alternative selector
        std::regex img_regex_alt(R"xxx(<img[^>]*data-src="([^"]+?)")xxx");
        if (!std::regex_search(response, img_match, img_regex_alt)) {
            // Another alternative selector
            std::regex img_regex_og(R"xxx(<meta property="og:image" content="([^"]+?)")xxx");
            std::regex_search(response, img_match, img_regex_og);
        }
    }

    if (img_match.size() > 1) {
        std::string img_url = img_match[1].str();
        size_t param_pos = img_url.find("?");
        if (param_pos != std::string::npos) {
            info.imageUrl = img_url.substr(0, param_pos);
        } else {
            info.imageUrl = img_url;
        }
    }

    return info;
}

bool CoverDownloader::downloadCover(const std::string& idOrUrl, int formatType, int index,
                                   std::vector<std::string>& failList) {
    auto [item_id, content_type] = Utils::extractIdAndType(idOrUrl);
    if (item_id.empty()) {
        std::cout << "[" << index << "] Cannot recognize link or ID: " << idOrUrl << std::endl;
        failList.push_back(idOrUrl);
        return false;
    }

    try {
        // Create covers directory
        std::filesystem::create_directories("covers");

        if (content_type == "album") {
            AlbumInfo info = getAlbumInfo(item_id);
            if (info.imageUrl.empty()) {
                std::cout << "[" << index << "] Album cover image not found: " << item_id << std::endl;
                failList.push_back(item_id);
                return false;
            }

            std::string filename = Utils::sanitizeFilename(info.albumName) + ".jpg";
            auto response_data = safeRequestBinary(info.imageUrl);

            if (response_data.empty()) {
                std::cout << "[" << index << "] Failed to download album cover: " << item_id << std::endl;
                failList.push_back(item_id);
                return false;
            }

            // Save file
            std::ofstream file("covers/" + filename, std::ios::binary);
            if (file.is_open()) {
                file.write(response_data.data(), response_data.size());
                file.close();
                std::cout << "[" << index << "] Album \"" << info.albumName << "\" cover saved" << std::endl;
            } else {
                std::cout << "[" << index << "] Cannot create file: " << filename << std::endl;
                failList.push_back(item_id);
                return false;
            }

            humanDelay();
            return true;
        }
        else if (content_type == "song") {
            SongInfo info = getSongInfo(item_id);
            if (info.imageUrl.empty()) {
                std::cout << "[" << index << "] Song cover image not found: " << item_id << std::endl;
                failList.push_back(item_id);
                return false;
            }

            std::string filename = Utils::generateFilename(index, info.artistName, info.songName, formatType);
            auto response_data = safeRequestBinary(info.imageUrl);

            if (response_data.empty()) {
                std::cout << "[" << index << "] Failed to download song cover: " << item_id << std::endl;
                failList.push_back(item_id);
                return false;
            }

            // Save file
            std::ofstream file("covers/" + filename, std::ios::binary);
            if (file.is_open()) {
                file.write(response_data.data(), response_data.size());
                file.close();
                std::cout << "[" << index << "] " << info.artistName << " - " << info.songName << " saved" << std::endl;
            } else {
                std::cout << "[" << index << "] Cannot create file: " << filename << std::endl;
                failList.push_back(item_id);
                return false;
            }

            humanDelay();
            return true;
        }
        else {
            std::cout << "[" << index << "] Unsupported type: " << content_type << std::endl;
            failList.push_back(item_id);
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "[" << index << "] Error occurred: " << e.what() << std::endl;
        failList.push_back(item_id);
        return false;
    }
}

void CoverDownloader::humanDelay() {
    double delay = Utils::getRandomDelay(1.0, 2.0);
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(delay * 1000)));
}
