#pragma once

#include <span>

template <typename T>
std::string joinString(std::span<T> elements, const std::string &delimiter) {
  std::string concatenatedString;
  size_t index = 0;
  for (const auto &element : elements) {
    concatenatedString += std::to_string(element);
    if (index + 1 < elements.size())
      concatenatedString.append(delimiter);
    index++;
  }

  return concatenatedString;
}