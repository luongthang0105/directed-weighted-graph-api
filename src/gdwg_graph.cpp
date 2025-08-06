#include "gdwg_graph.h"
#include <utility>

namespace gdwg {
	template<typename N, typename E>
	Graph<N, E>::Graph()
	: nodes_{}
	, adjacencyList_{} {}

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
	, adjacencyList_(std::exchange(other.adjacencyList_, {})) {}

	template<typename N, typename E>
	auto Graph<N, E>::operator=(Graph&& other) noexcept -> Graph& {
		if (this == &other) {
			return *this;
		}

		std::swap(nodes_, other.nodes_);
		std::swap(adjacencyList_, other.adjacencyList_);

		other.adjacencyList_.clear();
		other.nodes_.clear();

		return *this;
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