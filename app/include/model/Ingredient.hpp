#pragma once

#include <cstring>
#include <string>

#include "database/file/parsing/common.hpp"

template <size_t MAX_LENGTH> class Ingredient {
private:
  std::string name;

public:
  explicit Ingredient(const std::string &name) : name(name) {
    if (name.length() > static_cast<std::string::size_type>(MAX_LENGTH))
      throw std::length_error("Ingredient name exceeds maximum length");
  }

public:
  [[nodiscard]] const std::string &getName() const { return name; }

  bool operator==(const Ingredient &rhs) const { return name == rhs.name; }
  bool operator<(const Ingredient &rhs) const {
    auto maxSize = std::min(name.length(), rhs.name.length());
    for (size_t i = 0; i < maxSize; ++i) {
      auto leftChar = name[i];
      auto rightChar = rhs.name[i];
      if (leftChar < rightChar)
        return true;
      else if (leftChar > rightChar)
        return false;
    }
    return false;
  }

  bool operator!=(const Ingredient &rhs) const { return !(rhs == *this); }
  bool operator>(const Ingredient &rhs) const { return rhs < *this; }
  bool operator<=(const Ingredient &rhs) const { return !(rhs < *this); }
  bool operator>=(const Ingredient &rhs) const { return !(*this < rhs); }
};

namespace std {
template <size_t MAX_LENGTH>
[[maybe_unused]] std::string
to_string(const Ingredient<MAX_LENGTH> &ingredient) {
  return ingredient.getName();
}
} // namespace std

template <size_t MAX_LENGTH> struct Serialize<Ingredient<MAX_LENGTH>> {
  FIXED_LENGTH_SERIALIZER(Ingredient<MAX_LENGTH>, MAX_LENGTH) {
    constexpr auto sizeInBytes = Serialize<Ingredient<MAX_LENGTH>>::length;
    std::array<uint8_t, sizeInBytes> charBuffer{0};
    std::memcpy(charBuffer.data(), it.getName().data(), it.getName().length());
    return charBuffer;
  }
};

template <size_t MAX_LENGTH> struct Deserialize<Ingredient<MAX_LENGTH>> {
  FIXED_LENGTH_DESERIALIZER(Ingredient<MAX_LENGTH>, MAX_LENGTH) {
    return Ingredient<MAX_LENGTH>(
        std::string(reinterpret_cast<const char *>(it.data())));
  }
};
