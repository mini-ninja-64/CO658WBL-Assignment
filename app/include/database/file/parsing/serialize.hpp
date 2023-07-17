#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <filesystem>
#include <fstream>

namespace serialize {
    template<typename T>
    constexpr size_t fixedLengthInBytes();
    template<typename T>
    using FixedLengthDataBuffer = std::array<uint8_t, fixedLengthInBytes<T>()>;
    template<typename T>
    FixedLengthDataBuffer<T> fixedLengthType(const T& element);

    template<typename T>
    std::streampos toStream(const T& element, std::streampos position, std::fstream& fileStream);

    template<typename T>
    std::streampos toStream(const T& element, std::streampos position, std::fstream& fileStream) {
        auto serializedBuffer = fixedLengthType(element);

        fileStream.seekp(position);
        fileStream.write(reinterpret_cast<char *>(serializedBuffer.data()), serializedBuffer.size());

        return fileStream.tellp();
    }
}