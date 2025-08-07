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
	auto Graph<N, E>::UniquePtrValueComparator::operator()(const std::unique_ptr<N>& a, const std::unique_ptr<N>& b) const
	    -> bool {
		return *a < *b;
	}

	template<typename N, typename E>
	auto Graph<N, E>::RawPtrValueComparator::operator()(const N*& a, const N*& b) const -> bool {
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
	template<typename N, typename E>
	[[nodiscard]] auto Graph<N, E>::operator==(Graph const& other) -> bool {
		if (nodes_.size() != other.nodes_.size())
			return false;
		if (adjacency_list_.size() != other.adjacency_list_.size())
			return false;

		// comparing each node
		for (auto this_it = nodes_.begin(), other_it = other.nodes_.begin();
		     this_it != nodes_.end() && other_it != other.nodes_.end();
		     this_it++, other_it++)
		{
			if (**this_it != **other_it) {
				return false;
			}
		}

		for (auto this_it = adjacency_list_.begin(), other_it = other.adjacency_list_.begin();
		     this_it != adjacency_list_.end() && other_it != other.adjacency_list_.end();
		     this_it++, other_it++)
		{
			auto& [this_node_ptr, this_edge_set] = *this_it;
			auto& [other_node_ptr, other_edge_set] = *other_it;

			if (*this_node_ptr != *other_node_ptr)
				return false;

			for (auto this_edge_it = this_edge_set.begin(), other_edge_it = other_edge_set.begin();
			     this_edge_it != this_edge_set.end() && other_edge_it != other_edge_set.end();
			     this_edge_it++, other_edge_it++)
			{
				if (**this_edge_it != **other_edge_it) {
					return false;
				}
			}
		}
		return true;
	}

	template<typename N, typename E>
	auto Edge<N, E>::UniquePtrEdgeComparator::operator()(const std::unique_ptr<Edge<N, E>>& a,
	                                                     const std::unique_ptr<Edge<N, E>>& b) const -> bool {
		auto const& edge_a = *a;
		auto const& edge_b = *b;
		if (*edge_a.src_ == *edge_b.src_) {
			if (*edge_a.dst == *edge_b.dst) {
				// it is actually UB where two edges of the same src and dst, within a graph, are both unweighted.
				// However, if either is unweighted, then it is considered "smaller"
				if (!edge_a.get_weight().has_value())
					return true;
				if (!edge_b.get_weight().has_value())
					return false;

				return edge_a.get_weight().value() < edge_b.get_weight().value();
			}
		}
		return *edge_a.src_ < *edge_b.src_;
	}

	template<typename N, typename E>
	[[nodiscard]] auto Edge<N, E>::operator==(Edge const& other) -> bool {
		if (*src_ != *other.src_)
			return false;
		if (*dst_ != *other.dst_)
			return false;
		if (!get_weight().has_value() && !other.get_weight().has_value())
			return true;
	}
} // namespace gdwg