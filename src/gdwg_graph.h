#ifndef GDWG_GRAPH_H
#define GDWG_GRAPH_H

#include <map>
#include <memory>
#include <set>
namespace gdwg {
	template<typename N, typename E>
	class Edge {
	 public:
	 private:
		// You may need to add data members and member functions
		template<typename G_N, typename G_E>
		friend class Graph;
	};

	class WeightedEdge {
	 public:
	 private:
	};

	class UnweightedEdge {
	 public:
	 private:
	};

	template<typename N, typename E>
	class Graph {
	 public:
		Graph();

		Graph(std::initializer_list<N> il);

		template<typename InputIt>
		Graph(InputIt first, InputIt last);

		// moves
		Graph(Graph&& other) noexcept;
		auto operator=(Graph&& other) noexcept -> Graph&;

		// copies
		Graph(Graph const& other);
		auto operator=(Graph const& other) -> Graph&;

		// =================MODIFIERS===================
		auto insert_node(N const& value) -> bool;

	 private:
		struct ValueComparator {
			auto operator()(std::unique_ptr<N> a, std::unique_ptr<N> b) const -> bool;
		};

		std::set<std::unique_ptr<N>, ValueComparator> nodes_;
		std::map<N*, std::set<Edge<N, E>>> adjacencyList_;
	};
} // namespace gdwg

#endif // GDWG_GRAPH_H
