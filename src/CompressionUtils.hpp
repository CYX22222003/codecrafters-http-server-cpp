#pragma once
#include <zlib.h>
#include <string>
#include <stdexcept>
#include <cstring>

namespace Compression
{
    std::string compress_gzip(const std::string& data) {
        z_stream zs{};
        if (deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, 16 + MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
            throw std::runtime_error("deflateInit2 failed");
        }

        zs.next_in = (Bytef*)data.data();
        zs.avail_in = data.size();

        std::string out;
        char buffer[4096];

        int ret;
        do {
            zs.next_out = reinterpret_cast<Bytef*>(buffer);
            zs.avail_out = sizeof(buffer);

            ret = deflate(&zs, Z_FINISH);
            out.append(buffer, sizeof(buffer) - zs.avail_out);
        } while (ret == Z_OK);

        deflateEnd(&zs);

        if (ret != Z_STREAM_END) {
            throw std::runtime_error("gzip compression failed");
        }

        return out;
    }
} 
