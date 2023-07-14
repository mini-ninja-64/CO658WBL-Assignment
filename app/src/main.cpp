#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <chrono>
#include "Database.hpp"

#define RECIPES_PER_INGREDIENT 99999

#include "BPlusTree.hpp"


#ifndef TESTING

int main() {
    Database database(std::filesystem::path("./test.db"));
//    database;
}

#endif

//    std::cout << "Generating recipe list" << std::endl;
//    std::vector<int> recipeVector;
//    for (int i = 0; i < 100000; ++i) {
//        recipeVector.push_back(i);
//    }
//
//    std::cout << "Generating ingredients list" << std::endl;
//    srand(time(nullptr));
//    std::unordered_map<int, std::unordered_set<int>*> ingredients;
//    for (int ingredientIndex = 0; ingredientIndex < 10000; ++ingredientIndex) {
//        auto associatedRecipes = new std::unordered_set<int>();
//        for (int i = 0; i < RECIPES_PER_INGREDIENT; ++i) {
//            associatedRecipes->insert(rand() % recipeVector.size());
//        }
//        ingredients[ingredientIndex] = associatedRecipes;
//    }
//
//
//    std::cout << "Fetching ingredients" << std::endl;
//    std::array<int, 10> ingredientsToQuery = { 999, 1, 56, 506, 690, 420, 1000, 7888, 3000, 256 };
//    std::array<std::unordered_set<int>*, 10> recipesCollections = { nullptr };
//    size_t recipeIndex = 0;
//    for (const auto &ingredientToQuery: ingredientsToQuery) {
//        auto t1 = std::chrono::high_resolution_clock::now();
//        recipesCollections[recipeIndex] = ingredients[ingredientToQuery];
//        auto t2 = std::chrono::high_resolution_clock::now();
//        std::cout << "It took " << std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count() << "ns to fetch the recipe list for " << ingredientToQuery << std::endl;
//        recipeIndex++;
//    }
//
//
//    std::cout << "calculating ingredients union" << std::endl;
//    auto firstRecipeSet = recipesCollections[0];
//    std::unordered_set<int> unionedRecipes(*firstRecipeSet);
//    std::unordered_set<int> deletedRecipes;
//
//    auto t1 = std::chrono::high_resolution_clock::now();
//    for (size_t recipeSetIndex = 1; recipeSetIndex < recipesCollections.size(); ++recipeSetIndex) {
//        for (const auto &recipe: unionedRecipes) {
//            bool recipePresent = recipesCollections[recipeSetIndex]->contains(recipe);
//            if (!recipePresent) deletedRecipes.insert(recipe);
//        }
//    }
//    auto t2 = std::chrono::high_resolution_clock::now();
//    std::cout << "It took " << std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count() << "ms to calculate the union recipe list" << std::endl;
//
//    std::cout << unionedRecipes.size() << std::endl;
//    std::cout << deletedRecipes.size() << std::endl;
//}