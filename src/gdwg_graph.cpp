#include "gdwg_graph.h"
#include <utility>

namespace gdwg {
	template<typename N, typename E>
	Graph<N, E>::Graph()
	: nodes_{}
	, adjacency_list_{} {}

	template<typename N, typename E>
	Graph<N, E>::Graph(std::initializer_list<N> il)
	: Graph(il.begin(), il.end()) {}

	template<typename N, typename E>
	template<typename InputIt>
	Graph<N, E>::Graph(InputIt first, InputIt last) {
		for (auto it = first; it != last; ++it) {
			insert_node(*it);
		}
	}

	template<typename N, typename E>
	Graph<N, E>::Graph(Graph&& other) noexcept
	: nodes_(std::exchange(other.nodes_, {}))
	, adjacency_list_(std::exchange(other.adjacency_list_, {})) {}

	template<typename N, typename E>
	auto Graph<N, E>::operator=(Graph&& other) noexcept -> Graph& {
		if (this == &other) {
			return *this;
		}

		std::swap(nodes_, other.nodes_);
		std::swap(adjacency_list_, other.adjacency_list_);

		other.adjacency_list_.clear();
		other.nodes_.clear();

		return *this;
	}

	template<typename N, typename E>
	Graph<N, E>::Graph(Graph const& other) {
		for (auto& [node_ptr, edges] : other.adjacency_list_) {
			auto clone_edge_set = std::set<std::unique_ptr<Edge<N, E>>>{};
			auto clone_node = std::make_unique<N>(*node_ptr);

			for (auto& edge : edges) {
				clone_edge_set.insert(*edge);
			}

			adjacency_list_.insert(std::make_pair(clone_node.get(), clone_edge_set));
			nodes_.insert(std::move(clone_node));
		}
	}

	template<typename N, typename E>
	auto Graph<N, E>::operator=(Graph const& other) -> Graph& {
		if (this == &other) {
			return *this;
		}

		this.swap(Graph(other));
		return *this;
	}

	template<typename N, typename E>
	auto Graph<N, E>::swap(Graph& other) noexcept -> void {
		std::swap(nodes_, other.nodes_);
		std::swap(adjacency_list_, other.adjacency_list_);
	}

	template<typename N, typename E>
	auto Graph<N, E>::ValueComparator::operator()(std::unique_ptr<N> a, std::unique_ptr<N> b) const -> bool {
		return *a < *b;
	}

	template<typename N, typename E>
	auto Graph<N, E>::insert_node(N const& value) -> bool {
		if (nodes_.contains(value)) {
			return false;
		}
		nodes_.insert(std::make_unique<N>(value));
		return true;
	}

} // namespace gdwg