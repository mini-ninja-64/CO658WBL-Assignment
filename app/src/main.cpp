#include <chrono>
#include <iostream>
#include <random>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "database/RecipeDatabase.hpp"
#include "database/file/FileBackedBPlusTree.hpp"

#ifndef TESTING

#define CHUNK_SIZE 4096
#define ADDRESS_TYPE uint32_t

#define ENTRIES_COUNT 10000000

auto now = std::chrono::duration_cast<std::chrono::seconds>(
               std::chrono::system_clock::now().time_since_epoch())
               .count();
std::default_random_engine randomEngine(now);
std::uniform_int_distribution<uint32_t> randomEntry(0, ENTRIES_COUNT);
std::uniform_int_distribution<uint32_t> randomValue(0, 0xFFFFFFFF);

#define MAX_STRING_LENGTH 20
typedef Ingredient<MAX_STRING_LENGTH> FixedLengthIngredient;
int main() {
#define ORDER 500
  FileBackedBPlusTree<FixedLengthIngredient, ADDRESS_TYPE> ingredientTree(
      "./ingredients.graph_index", "./ingredients.graph_db", ORDER, CHUNK_SIZE,
      true);
  FileBackedBPlusTree<boost::uuids::uuid, ADDRESS_TYPE> recipeTree(
      "./recipes.graph_index", "./recipes.graph_db", ORDER, CHUNK_SIZE, true);

  RecipeDatabase<MAX_STRING_LENGTH, ADDRESS_TYPE> recipeDatabase(ingredientTree,
                                                                 recipeTree);

  FixedLengthIngredient peppers("peppers");
  FixedLengthIngredient rice("rice");
  FixedLengthIngredient ricePudding("rice pudding");
  FixedLengthIngredient custard("custard");

  auto recipe1 = Recipe("fry off peppers and add some rice");
  recipeDatabase.addRecipe({peppers, rice}, recipe1);
}

#endif