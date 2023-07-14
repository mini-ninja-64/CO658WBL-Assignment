#include <iostream>
#include "Database.hpp"

Database::Database(std::filesystem::path dbPath) {
    std::cout << dbPath.extension() << std::endl;
}


