#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "database/file/FileBackedBPlusTree.hpp"

#include "database/file/parsing/baseParsers.hpp"

#include "model/Ingredient.hpp"
#include "model/Recipe.hpp"

#include "utils/bitwise.hpp"

template <size_t MAX_LENGTH, typename ADDRESS> class RecipeDatabase {
private:
  FileBackedBPlusTree<Ingredient<MAX_LENGTH>, ADDRESS> ingredientTree;
  FileBackedBPlusTree<boost::uuids::uuid, ADDRESS> recipeTree;
  boost::uuids::random_generator uuidGenerator{};

public:
  RecipeDatabase(
      FileBackedBPlusTree<Ingredient<MAX_LENGTH>, ADDRESS> &ingredientTree,
      FileBackedBPlusTree<boost::uuids::uuid, ADDRESS> &recipeTree)
      : ingredientTree(std::move(ingredientTree)),
        recipeTree(std::move(recipeTree)) {}

  void addRecipe(const std::vector<Ingredient<MAX_LENGTH>> &ingredients,
                 const Recipe &recipe) {
    auto recipeId = uuidGenerator();

    std::vector<uint8_t> recipeBuffer(recipe.getContents().begin(),
                                      recipe.getContents().end());

    recipeTree.insert(recipeId, recipeBuffer);

    for (const auto &ingredient : ingredients) {
      auto ingredientData = ingredientTree.find(ingredient);
      auto uuidBytes = Serialize<boost::uuids::uuid>::toBytes(recipeId);
      if (ingredientData) {
        std::vector<uint8_t> newIngredientData = ingredientData.value();
        newIngredientData.insert(newIngredientData.end(), uuidBytes.begin(),
                                 uuidBytes.end());
        ingredientTree.overwriteData(ingredient, newIngredientData);
      } else {
        std::vector<uint8_t> uuidBytesVector(uuidBytes.begin(),
                                             uuidBytes.end());
        ingredientTree.insert(ingredient, uuidBytesVector);
      }
    }
  }

  std::optional<std::vector<Recipe>>
  getRecipes(const Ingredient<MAX_LENGTH> &ingredient) {
    auto ingredientData = ingredientTree.find(ingredient);
    if (ingredientData) {
      std::vector<boost::uuids::uuid> recipeIds;
      constexpr auto uuidLength = Deserialize<boost::uuids::uuid>::length;
      for (std::vector<uint8_t>::size_type i = 0; i < ingredientData->size();
           i += uuidLength) {
        std::span<uint8_t, uuidLength> uuidBuffer(ingredientData->data() + i,
                                                  uuidLength);
        recipeIds.push_back(
            Deserialize<boost::uuids::uuid>::fromBytes(uuidBuffer));
      }
      std::vector<Recipe> recipes;
      for (const auto &recipeId : recipeIds) {
        auto recipeData = recipeTree.find(recipeId);
        if (!recipeData)
          throw std::domain_error(
              "Recipe referenced by ingredient is not present in database");
        std::string recipeContents(recipeData->begin(), recipeData->end());
        auto recipe = Recipe(recipeContents);
        recipes.push_back(recipe);
      }
      return recipes;
    }
    return std::nullopt;
  }
};
