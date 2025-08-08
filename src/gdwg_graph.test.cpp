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
		auto g = gdwg::Graph<int, std::string>{1, 2, 3};
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
	}
	SECTION("Move Constructor") {
		auto g = gdwg::Graph<int, std::string>{1, 2, 3};
		auto g2 = std::move(g);

		CHECK(g.empty());

		CHECK(g2.is_node(1));
		CHECK(g2.is_node(2));
		CHECK(g2.is_node(3));

		// TODO: check iterator created before move is NOT invalidated.
	}
	SECTION("Move Assignment") {
		auto g = gdwg::Graph<int, std::string>{1, 2, 3};
		auto g2 = gdwg::Graph<int, std::string>{4, 5, 6};
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

	SECTION("Copy Constructor") {
		auto g = gdwg::Graph<int, std::string>{1, 2, 3};
		auto g2 = g;

		CHECK(g2.is_node(1));
		CHECK(g2.is_node(2));
		CHECK(g2.is_node(3));

		// this is just to check we don't accidentally invalidates g
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));

		CHECK(g == g2);

		// TODO: check iterator created before move is NOT invalidated.
	}

	SECTION("Copy Constructor") {
		auto g = gdwg::Graph<int, std::string>{1, 2, 3};
		auto g2 = gdwg::Graph<int, std::string>{g};

		CHECK(g2.is_node(1));
		CHECK(g2.is_node(2));
		CHECK(g2.is_node(3));

		// this is just to check we don't accidentally invalidates g
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));

		CHECK(g == g2);
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

	SECTION("insert_edge") { // checks for duplicate edges
		auto g = gdwg::Graph<int, std::string>{};
		auto n = 5;
		g.insert_node(n);
		CHECK(g.is_node(n));
	}

	SECTION("replace_node") {
		auto g = gdwg::Graph<int, std::string>{1, 2, 3, 4};
		//     v-----------|
		//  |--1 --> 2 --> 3
		//  |--^     |---> 4
		//           |-----^
		REQUIRE(g.insert_edge(1, 2, "hello"));
		REQUIRE(g.insert_edge(2, 3, "hi"));
		REQUIRE(g.insert_edge(2, 4, "h"));
		REQUIRE(g.insert_edge(2, 4, std::nullopt));
		REQUIRE(g.insert_edge(3, 1, std::nullopt));
		REQUIRE(g.insert_edge(1, 1, std::nullopt));

		// Old data not exist
		CHECK_THROWS_AS(g.replace_node(0, 1), std::runtime_error);
		CHECK_THROWS_WITH(g.replace_node(0, 1),
		                  "Cannot call gdwg::Graph<N, E>::replace_node on a node that doesn't exist");

		// New data exists in a node already"
		CHECK_FALSE(g.replace_node(1, 4));
		CHECK_FALSE(g.replace_node(1, 1));

		// "Replace 1 with 0"
		CHECK(g.replace_node(1, 0));

		CHECK(g.is_node(0));
		CHECK_FALSE(g.is_node(1));

		CHECK(g.is_connected(0, 2));
		CHECK(g.is_connected(0, 0));
		CHECK(g.is_connected(3, 0));

		// Replace 2 with 1
		CHECK(g.replace_node(2, 1));
		CHECK(g.is_node(1));
		CHECK_FALSE(g.is_node(2));

		CHECK(g.is_connected(1, 3));
		CHECK(g.is_connected(1, 4));
	}

	SECTION("merge_replace_node") {
		SECTION("Test 1") {
			auto g = gdwg::Graph<int, std::string>{1, 2, 3, 4};
			//     v-----------|
			//  |--1 --> 2 --> 3
			//  |--^     |---> 4
			//           |-----^
			REQUIRE(g.insert_edge(1, 2, "hello"));
			REQUIRE(g.insert_edge(2, 3, "hi"));
			REQUIRE(g.insert_edge(2, 4, "h"));
			REQUIRE(g.insert_edge(2, 4, std::nullopt));
			REQUIRE(g.insert_edge(3, 1, std::nullopt));
			REQUIRE(g.insert_edge(1, 1, std::nullopt));

			// Old data / new data not exist
			CHECK_THROWS_AS(g.merge_replace_node(0, 1), std::runtime_error);
			CHECK_THROWS_WITH(g.merge_replace_node(0, 1),
			                  "Cannot call gdwg::Graph<N, E>::merge_replace_node on old or new data if they don't "
			                  "exist in the graph");
			CHECK_THROWS_WITH(g.merge_replace_node(1, 0),
			                  "Cannot call gdwg::Graph<N, E>::merge_replace_node on old or new data if they don't "
			                  "exist in the graph");

			// merge node 1 to 3
			CHECK_NOTHROW(g.merge_replace_node(1, 3));

			//           v-----|--|
			//           2 --> 3<--
			//           |---> 4
			//           |-----^
			CHECK(g.is_connected(3, 3));
			CHECK(g.is_connected(3, 2));
			CHECK(g.is_connected(2, 3));
		}
		SECTION("Test 2") {
			auto g = gdwg::Graph<int, int>{1, 2, 3, 4};
			REQUIRE(g.insert_edge(1, 2, 1));
			REQUIRE(g.insert_edge(1, 3, 2));
			REQUIRE(g.insert_edge(1, 4, 3));

			// merge node 1 to 3
			CHECK_NOTHROW(g.merge_replace_node(1, 2));

			CHECK(g.is_connected(2, 2));
			CHECK(g.is_connected(2, 3));
			CHECK(g.is_connected(2, 4));
		}

		SECTION("Test 3") {
			auto g = gdwg::Graph<int, int>{1, 2, 3, 4};
			REQUIRE(g.insert_edge(1, 2, 1));
			REQUIRE(g.insert_edge(1, 2, 2));
			REQUIRE(g.insert_edge(1, 3, 2));
			REQUIRE(g.insert_edge(1, 4, 3));
			REQUIRE(g.insert_edge(2, 2, 1));
			REQUIRE(g.insert_edge(2, 2, std::nullopt));

			// merge node 1 to 3
			CHECK_NOTHROW(g.merge_replace_node(1, 2));

			CHECK(g.is_connected(2, 2));
			CHECK(g.is_connected(2, 3));
			CHECK(g.is_connected(2, 4));

			auto from2to2 = g.edges(2, 2);
			CHECK(from2to2.size() == 3);
		}
	}

	SECTION("erase_node") {
		auto g = gdwg::Graph<int, std::string>{1, 2, 3, 4};
		// v-----------|
		// 1 --> 2 --> 3
		//       |---> 4
		REQUIRE(g.insert_edge(1, 2, "hello"));
		REQUIRE(g.insert_edge(2, 3, "hi"));
		REQUIRE(g.insert_edge(2, 4, "h"));
		REQUIRE(g.insert_edge(1, 1, std::nullopt));

		CHECK_FALSE(g.erase_node(0));
		CHECK_FALSE(g.erase_node(5));

		REQUIRE(g.erase_node(2));

		CHECK_FALSE(g.is_node(2));

		auto connections_from_1 = g.connections(1);
		CHECK(connections_from_1.size() == 1);
		CHECK(connections_from_1[0] == 1);
	}

	SECTION("erase_edge") {
		auto g = gdwg::Graph<int, std::string>{1, 2, 3, 4};
		//     v-----------|
		//  |--1 --> 2 --> 3
		//  |--^     |---> 4
		//           |-----^
		REQUIRE(g.insert_edge(1, 2, "hello"));
		REQUIRE(g.insert_edge(2, 3, "hi"));
		REQUIRE(g.insert_edge(2, 4, "h"));
		REQUIRE(g.insert_edge(2, 4, std::nullopt));
		REQUIRE(g.insert_edge(3, 1, std::nullopt));
		REQUIRE(g.insert_edge(1, 1, std::nullopt));

		CHECK_FALSE(g.erase_edge(1, 3));
		CHECK_FALSE(g.erase_edge(2, 1));

		CHECK(g.is_connected(2, 4));
		CHECK(g.erase_edge(2, 4));

		CHECK(g.is_connected(2, 4));
		CHECK_FALSE(g.erase_edge(2, 4)); // can't erase old edge
		CHECK(g.erase_edge(2, 4, "h")); // erase the last edge from 2->4
		CHECK_FALSE(g.is_connected(2, 4));

		CHECK(g.is_connected(3, 1));
		CHECK(g.erase_edge(3, 1));
		CHECK_FALSE(g.is_connected(3, 1));

		CHECK(g.is_connected(1, 1));
		CHECK(g.erase_edge(1, 1));
		CHECK_FALSE(g.is_connected(1, 1));
	}

	SECTION("clear") {
		auto g = gdwg::Graph<int, std::string>{1, 2, 3, 4};
		//     v-----------|
		//  |--1 --> 2 --> 3
		//  |--^     |---> 4
		//           |-----^
		REQUIRE(g.insert_edge(1, 2, "hello"));
		REQUIRE(g.insert_edge(2, 3, "hi"));
		REQUIRE(g.insert_edge(2, 4, "h"));
		REQUIRE(g.insert_edge(2, 4, std::nullopt));
		REQUIRE(g.insert_edge(3, 1, std::nullopt));
		REQUIRE(g.insert_edge(1, 1, std::nullopt));

		CHECK(g.replace_node(1, 0));
		CHECK(g.replace_node(2, 1));

		g.clear();
		CHECK(g.empty());
	}
}

TEST_CASE("Accessors") {
	SECTION("is_connected") {
		auto g = gdwg::Graph<int, std::string>{1, 2, 3, 4};
		// v-----------|
		// 1 --> 2 --> 3
		//       |---> 4
		//       |-----^
		REQUIRE(g.insert_edge(1, 2, "hello"));
		REQUIRE(g.insert_edge(2, 3, "hi"));
		REQUIRE(g.insert_edge(2, 4, "h"));
		REQUIRE(g.insert_edge(2, 4, std::nullopt));
		REQUIRE(g.insert_edge(3, 1, std::nullopt));

		auto const error_msg = "Cannot call gdwg::Graph<N, E>::is_connected if src or dst node don't exist in "
		                       "the graph";
		SECTION("Success: valid queries") {
			CHECK(g.is_connected(1, 2));
			CHECK(g.is_connected(2, 3));
			CHECK(g.is_connected(2, 4));
			CHECK(g.is_connected(3, 1));

			CHECK(!g.is_connected(1, 4));
			CHECK(!g.is_connected(4, 1));
			CHECK(!g.is_connected(2, 1));
			CHECK(!g.is_connected(3, 2));
			CHECK(!g.is_connected(3, 4));
		}
		SECTION("Error: src or dst does not exist") {
			CHECK_THROWS_AS(g.is_connected(0, 2), std::runtime_error);
			CHECK_THROWS_WITH(g.is_connected(0, 2), error_msg);
			CHECK_THROWS_WITH(g.is_connected(1, 5), error_msg);
			CHECK_THROWS_WITH(g.is_connected(-1, 6), error_msg);
		}
	}
	SECTION("nodes()") {
		std::vector<std::string> temp;
		{
			auto g = gdwg::Graph<std::string, std::string>{"1", "2", "3", "4"};
			REQUIRE(g.insert_edge("1", "2", "hello"));
			REQUIRE(g.insert_edge("2", "3", "hi"));

			temp = g.nodes();
		}

		CHECK(temp.size() == 4);
		CHECK(temp[0] == "1");
		CHECK(temp[1] == "2");
		CHECK(temp[2] == "3");
		CHECK(temp[3] == "4");
	}

	SECTION("edge()") {
		auto g = gdwg::Graph<int, int>{1, 2, 3};
		/**
		 * ---v ---v
		 * |--1--> 2
		 * ---^ ---^
		 */
		g.insert_edge(1, 2, 3);
		g.insert_edge(1, 2, 4);
		g.insert_edge(1, 2, std::nullopt);
		g.insert_edge(1, 1, std::nullopt);
		g.insert_edge(1, 1, 1);
		auto edges_from_1_to_1 = g.edges(1, 1);
		CHECK(edges_from_1_to_1.size() == 2);

		CHECK((*edges_from_1_to_1[0]).get_weight() == std::nullopt);
		CHECK((*edges_from_1_to_1[0]).get_nodes() == std::make_pair(1, 1));

		CHECK((*edges_from_1_to_1[1]).get_weight() == 1);
		CHECK((*edges_from_1_to_1[1]).get_nodes() == std::make_pair(1, 1));

		auto edges_from_1_to_2 = g.edges(1, 2);

		CHECK((*edges_from_1_to_2[0]).get_weight() == std::nullopt);
		CHECK((*edges_from_1_to_2[0]).get_nodes() == std::make_pair(1, 2));

		CHECK((*edges_from_1_to_2[1]).get_weight() == 3);
		CHECK((*edges_from_1_to_2[1]).get_nodes() == std::make_pair(1, 2));

		CHECK((*edges_from_1_to_2[2]).get_weight() == 4);
		CHECK((*edges_from_1_to_2[2]).get_nodes() == std::make_pair(1, 2));

		CHECK_THROWS_AS(g.edges(1, 4), std::runtime_error);
		CHECK_THROWS_WITH(g.edges(1, 4),
		                  "Cannot call gdwg::Graph<N, E>::edges if src or dst node don't exist in the graph");
	}

	SECTION("connections()") {
		auto g = gdwg::Graph<int, int>{1, 2, 3, 4, 5};
		g.insert_edge(1, 1, std::nullopt);
		g.insert_edge(1, 1, 1);

		g.insert_edge(1, 2, std::nullopt);
		g.insert_edge(1, 2, 100);
		g.insert_edge(1, 2, 1);

		g.insert_edge(1, 3, std::nullopt);
		g.insert_edge(1, 3, 100);
		g.insert_edge(1, 3, 1);

		g.insert_edge(1, 5, std::nullopt);
		g.insert_edge(1, 5, 100);
		g.insert_edge(1, 5, 1);

		auto connections = g.connections(1);
		CHECK(connections.size() == 4);
		CHECK(connections[0] == 1);
		CHECK(connections[1] == 2);
		CHECK(connections[2] == 3);
		CHECK(connections[3] == 5);

		CHECK_THROWS_AS(g.connections(0), std::runtime_error);
		CHECK_THROWS_WITH(g.connections(0),
		                  "Cannot call gdwg::Graph<N, E>::connections if src doesn't exist in the graph");
	}
}