#pragma once
#include <zlib.h>
#include <string>
#include <stdexcept>
#include <cstring>

namespace Compression
{
    std::vector<unsigned char> gzip_compress(const std::string& str) {
        z_stream zs{};
        deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY);

        zs.next_in = (Bytef*)str.data();
        zs.avail_in = str.size();

        std::vector<unsigned char> outbuffer(1024);
        std::vector<unsigned char> compressed;

        int ret;
        do {
            zs.next_out = outbuffer.data();
            zs.avail_out = outbuffer.size();
            ret = deflate(&zs, Z_FINISH);
            compressed.insert(compressed.end(), outbuffer.data(), outbuffer.data() + (outbuffer.size() - zs.avail_out));
        } while (ret == Z_OK);

        deflateEnd(&zs);

        if (ret != Z_STREAM_END) {
            throw std::runtime_error("gzip compression failed");
        }

        return compressed;
    }
}
