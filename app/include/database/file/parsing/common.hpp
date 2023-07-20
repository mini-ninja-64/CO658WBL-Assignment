#pragma once

#include <array>
#include <span>
#include <fstream>

#include "serialize.hpp"
#include "deserialize.hpp"

template<typename T>
concept FixedLength = requires (const T& element, std::streampos position, std::span<uint8_t, Deserialize<T>::length> deserializableBuffer) {
    { Serialize<T>::length } -> std::same_as<size_t>;
    { Serialize<T>::toBytes() } -> std::same_as<std::array<uint8_t, Serialize<T>::length>>;
    { Deserialize<T>::fromBytes(position, deserializableBuffer) } -> std::same_as<T>;
};

#define FIXED_LENGTH_SERIALIZER(TYPE, LENGTH) \
static constexpr size_t length = LENGTH; \
static std::streampos toStream(const TYPE& element, std::streampos position, std::fstream& fileStream) { \
    auto serializedBuffer = Serialize<TYPE>::toBytes(element); \
    fileStream.seekp(position); \
    fileStream.write(reinterpret_cast<char *>(serializedBuffer.data()), serializedBuffer.size()); \
    return fileStream.tellp(); \
} \
static std::array<uint8_t, length> toBytes([[maybe_unused]] TYPE it)

#define FIXED_LENGTH_DESERIALIZER(TYPE, LENGTH) \
static constexpr size_t length = LENGTH; \
static TYPE fromStream(std::streampos position, std::fstream& fileStream) { \
    std::array<uint8_t, LENGTH> buffer = {0}; \
    fileStream.seekg(position); \
    fileStream.read(reinterpret_cast<char *>(buffer.data()), LENGTH); \
    if(fileStream.gcount() != buffer.size()) throw std::range_error("Filestream ended unexpectedly"); \
    return fromBytes(buffer); \
} \
static TYPE fromBytes([[maybe_unused]] std::span<uint8_t, length> it)
