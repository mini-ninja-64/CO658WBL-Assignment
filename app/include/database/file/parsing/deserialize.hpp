#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <filesystem>
#include <fstream>

#include "utils/templating.hpp"

namespace deserialize {
    template<typename T>
    constexpr size_t fixedLengthInBytesImplementation(TypeTag<T>);
    template<typename T>
    constexpr size_t fixedLengthInBytes() {
        return deserialize::fixedLengthInBytesImplementation(TypeTag<T>{});
    }

    template<typename T>
    using FixedLengthDataBuffer = std::span<uint8_t, deserialize::fixedLengthInBytes<T>()>;
    template<typename T>
    T fixedLengthTypeImplementation(FixedLengthDataBuffer<T> buffer, TypeTag<T>);
    template<typename T>
    T fixedLengthType(FixedLengthDataBuffer<T> buffer) {
        return deserialize::fixedLengthTypeImplementation(buffer, TypeTag<T>{});
    }

    template<typename T>
    T fromStreamImplementation(std::streampos position, std::fstream& fileStream, TypeTag<T>);
    template<typename T>
    T fromStream(std::streampos position, std::fstream& fileStream) {
        return deserialize::fromStreamImplementation(position, fileStream, TypeTag<T>{});
    }
}

// Generic helpers to make deserialization of fixed length types more expressive
namespace deserialize {
//    template<typename T>
//    constexpr size_t fixedLengthInBytesImplementation(TypeTag<T> typeTag) {
//        return common::fixedLengthInBytesImplementation(typeTag);
//    }

    template<typename T>
    T fromStreamImplementation(std::streampos position, std::fstream& fileStream, TypeTag<T>) {
        constexpr auto sizeInBytes = deserialize::fixedLengthInBytes<T>();
        std::array<uint8_t, sizeInBytes> buffer = {0};

        fileStream.seekg(position);
        fileStream.read(reinterpret_cast<char *>(buffer.data()), buffer.size());
        if(fileStream.gcount() != buffer.size()) throw std::range_error("Filestream ended unexpectedly");

        return deserialize::fixedLengthType<T>(buffer);
    }
}
