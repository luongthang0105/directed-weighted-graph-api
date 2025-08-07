#include "gdwg_graph.h"

#include <catch2/catch.hpp>
#include <vector>

TEST_CASE("Constructors") {
	SECTION("Default constructor") {
		auto g = gdwg::Graph<int, std::string>{};
		CHECK(g.empty());
	}
	SECTION("InputIt Constructor") {
		auto nodes = std::vector<int>{1, 2, 3};
		auto g = gdwg::Graph<int, std::string>{nodes.begin(), nodes.end()};
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
	}
	SECTION("Intializer List Constructor") {
		auto g = gdwg::Graph<int, std::string>{{1, 2, 3}};
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
	}
	SECTION("Move Constructor") {
		auto g = gdwg::Graph<int, std::string>{{1, 2, 3}};
		auto g2 = std::move(g);

		CHECK(g.empty());

		CHECK(g2.is_node(1));
		CHECK(g2.is_node(2));
		CHECK(g2.is_node(3));

		// TODO: check iterator created before move is NOT invalidated.
	}
	SECTION("Move Assignment") {
		auto g = gdwg::Graph<int, std::string>{{1, 2, 3}};
		auto g2 = gdwg::Graph<int, std::string>{{4, 5, 6}};
		g2 = std::move(g);

		CHECK(g.empty());

		CHECK(g2.is_node(1));
		CHECK(g2.is_node(2));
		CHECK(g2.is_node(3));

		CHECK(!g2.is_node(4));
		CHECK(!g2.is_node(5));
		CHECK(!g2.is_node(6));
		// TODO: check iterator created before move is NOT invalidated.
	}
}

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