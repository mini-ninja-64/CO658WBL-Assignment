#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <filesystem>
#include <fstream>

#include "utils/templating.hpp"

namespace serialize {
    template<typename T>
    constexpr size_t fixedLengthInBytesImplementation(TypeTag<T>);

    template<typename T>
    constexpr size_t fixedLengthInBytes() {
        return serialize::fixedLengthInBytesImplementation(TypeTag<T>{});
    }

    template<typename T>
    using FixedLengthDataBuffer = std::array<uint8_t, fixedLengthInBytes<T>()>;

    template<typename T>
    FixedLengthDataBuffer<T> fixedLengthTypeImplementation(const T &element, TypeTag<T>);

    template<typename T>
    FixedLengthDataBuffer<T> fixedLengthType(const T &element) {
        return serialize::fixedLengthTypeImplementation(element, TypeTag<T>{});
    }

    template<typename T>
    std::streampos toStreamImplementation(const T &element, std::streampos position, std::fstream& fileStream, TypeTag<T>);

    template<typename T>
    std::streampos toStream(const T &element, std::streampos position, std::fstream &fileStream) {
        return serialize::toStreamImplementation(element, position, fileStream, TypeTag<T>{});
    }

}

// Generic helpers to make deserialization of fixed length types more expressive
namespace serialize {
//    template<typename T>
//    constexpr size_t fixedLengthInBytesImplementation(TypeTag<T> typeTag) {
//        return common::fixedLengthInBytesImplementation(typeTag);
//    }

    template<typename T>
    std::streampos toStreamImplementation(const T& element, std::streampos position, std::fstream& fileStream, TypeTag<T>) {
        auto serializedBuffer = serialize::fixedLengthType(element);

        fileStream.seekp(position);
        fileStream.write(reinterpret_cast<char *>(serializedBuffer.data()), serializedBuffer.size());

        return fileStream.tellp();
    }
}
