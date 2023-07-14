#pragma once

#include <memory>
#include <filesystem>
#include "Ingredient.hpp"

class Database {
public:
    explicit Database(std::filesystem::path dbPath);

    Ingredient getIngredient(uint16_t id);
};

