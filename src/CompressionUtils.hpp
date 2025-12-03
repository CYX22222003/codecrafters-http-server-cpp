#pragma once
#include <string>
#include <stdexcept>
#include <zlib.h>

namespace Compression {
    std::string compress_gzip(const std::string& input) {
        z_stream zs{};
        if (deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED,
                         15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
            throw std::runtime_error("deflateInit2 failed");
        }

        zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(input.data()));
        zs.avail_in = input.size();

        std::string compressed;
        compressed.reserve(input.size() / 2); // optional hint

        std::vector<unsigned char> buffer(1024);

        int ret;
        do {
            zs.next_out = buffer.data();
            zs.avail_out = buffer.size();

            ret = deflate(&zs, Z_FINISH);
            if (ret != Z_OK && ret != Z_STREAM_END && ret != Z_BUF_ERROR) {
                deflateEnd(&zs);
                throw std::runtime_error("deflate failed");
            }

            compressed.append(reinterpret_cast<char*>(buffer.data()), 
                              buffer.size() - zs.avail_out);
        } while (ret != Z_STREAM_END);

        deflateEnd(&zs);
        return compressed; // binary-safe string
    }
}
