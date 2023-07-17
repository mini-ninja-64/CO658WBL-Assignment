#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <filesystem>
#include <fstream>

namespace deserialize {
    template<typename T>
    constexpr size_t fixedLengthInBytes();
    template<typename T>
    using FixedLengthDataBuffer = std::span<uint8_t, deserialize::fixedLengthInBytes<T>()>;
    template<typename T>
    T fixedLengthType(FixedLengthDataBuffer<T> buffer);

    template<typename T>
    T fromStream(std::streampos position, std::fstream& fileStream);

    template<typename T>
    T fromStream(std::streampos position, std::fstream& fileStream) {
        constexpr auto sizeInBytes = fixedLengthInBytes<T>();
        std::array<uint8_t, sizeInBytes> buffer = {0};

        fileStream.seekg(position);
        fileStream.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
        if(fileStream.gcount() != buffer.size()) throw std::range_error("Filestream ended unexpectedly");

        return fixedLengthType<T>(buffer);
    }
}