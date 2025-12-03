#pragma once
#include <zlib.h>
#include <string>
#include <stdexcept>
#include <cstring>

namespace Compression
{
    std::string gzip_compress_string(const std::string &data, int level = Z_BEST_COMPRESSION)
    {
        z_stream zs;
        std::memset(&zs, 0, sizeof(zs));

        if (deflateInit2(&zs, level, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK)
        {
            throw std::runtime_error("deflateInit2 failed while compressing.");
        }

        zs.next_in = reinterpret_cast<Bytef *>(const_cast<char *>(data.data()));
        zs.avail_in = static_cast<uInt>(data.size());

        std::string compressed;
        char buffer[32768];

        int ret;
        do
        {
            zs.next_out = reinterpret_cast<Bytef *>(buffer);
            zs.avail_out = sizeof(buffer);

            ret = deflate(&zs, Z_FINISH);

            if (compressed.size() < zs.total_out)
            {
                compressed.append(buffer, sizeof(buffer) - zs.avail_out);
            }
        } while (ret == Z_OK);

        deflateEnd(&zs);

        if (ret != Z_STREAM_END)
        {
            throw std::runtime_error("Exception during zlib compression");
        }

        return compressed;
    }
}
