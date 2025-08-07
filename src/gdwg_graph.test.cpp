#include "gdwg_graph.h"

#include <catch2/catch.hpp>

TEST_CASE("Constructors") {}

TEST_CASE("Modifiers") {
	SECTION("insert_node") {
		auto g = gdwg::Graph<int, std::string>{};
		auto n = 5;
		g.insert_node(n);
		CHECK(g.is_node(n));
	}
}

TEST_CASE("Accessors") {
	SECTION("is_node") {}
}