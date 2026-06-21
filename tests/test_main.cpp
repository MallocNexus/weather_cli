#include <catch2/catch_session.hpp>

int main(int argc, char* argv[]) {
    // Run the Catch session with parsed arguments
    return Catch::Session().run(argc, argv);
}
