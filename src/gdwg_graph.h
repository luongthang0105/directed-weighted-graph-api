#ifndef GDWG_GRAPH_H
#define GDWG_GRAPH_H

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <sstream>
#include <utility>
#include <vector>

namespace gdwg {
	template<typename N, typename E>
	class Edge {
	 public:
		Edge(N const& src, N const& dst);
		Edge(Edge const& other);

		virtual auto print_edge() -> std::string = 0;
		virtual auto is_weighted() -> bool = 0;
		virtual auto get_weight() const -> std::optional<E> = 0;
		auto get_nodes() const -> std::pair<N, N>;

		virtual ~Edge() = default;

		[[nodiscard]] auto operator==(Edge const& other) -> bool;

		struct UniquePtrEdgeComparator {
			auto operator()(const std::unique_ptr<Edge<N, E>>& a, const std::unique_ptr<Edge<N, E>>& b) const -> bool;
		};

	 private:
		template<typename G_N, typename G_E>
		friend class Graph;

	 protected:
		N const* src_;
		N const* dst_;
	};

	template<typename N, typename E>
	class WeightedEdge : public Edge<N, E> {
	 public:
		WeightedEdge(N const& src, N const& dst, E const& weight);
		explicit WeightedEdge(WeightedEdge const& other);

		auto print_edge() -> std::string override;
		auto is_weighted() -> bool override;
		auto get_weight() const -> std::optional<E> override;

	 private:
		std::unique_ptr<E> weight_;
	};

	template<typename N, typename E>
	class UnweightedEdge : public Edge<N, E> {
	 public:
		UnweightedEdge(N const& src, N const& dst);
		explicit UnweightedEdge(UnweightedEdge const& other);

		auto print_edge() -> std::string override;
		auto is_weighted() -> bool override;
		auto get_weight() const -> std::optional<E> override;

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

		~Graph() = default;

		// =================MODIFIERS===================
		auto insert_node(N const& value) -> bool;
		auto insert_edge(N const& src, N const& dst, std::optional<E> weight = std::nullopt) -> bool;

		// =================ACCESSORS===================
		[[nodiscard]] auto is_node(N const& value) -> bool;
		[[nodiscard]] auto empty() -> bool;
		[[nodiscard]] auto is_connected(N const& src, N const& dst) -> bool;
		[[nodiscard]] auto nodes() -> std::vector<N>;
		[[nodiscard]] auto edges(N const& src, N const& dst) -> std::vector<std::unique_ptr<Edge<N, E>>>;
		// [[nodiscard]] auto find(N const& src, N const& dst, std::optional<E> weight = std::nullopt) -> iterator;
		[[nodiscard]] auto connections(N const& src) -> std::vector<N>;

		// =================COMPARISONS===================
		[[nodiscard]] auto operator==(Graph const& other) const -> bool;

	 private:
		struct UniquePtrValueComparator {
			auto operator()(const std::unique_ptr<N>& a, const std::unique_ptr<N>& b) const -> bool;
		};

		struct RawPtrValueComparator {
			auto operator()(const N* a, const N* b) const -> bool;
		};

		auto swap(Graph& other) noexcept -> void;

		/**
		 * Precondition: `is_node(value) == true`
		 */
		auto find_node_by_value(N const& value) -> const std::unique_ptr<N>&;

		using edge_set = std::set<std::unique_ptr<Edge<N, E>>, typename Edge<N, E>::UniquePtrEdgeComparator>;

		std::set<std::unique_ptr<N>, UniquePtrValueComparator> nodes_;
		std::map<N*, edge_set, RawPtrValueComparator> adjacency_list_;
	};
} // namespace gdwg

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
		for (const std::unique_ptr<N>& node : other.nodes_) {
			auto val = *node;
			insert_node(val);
		}

		for (auto& [node_ptr, edges] : other.adjacency_list_) {
			for (const std::unique_ptr<Edge<N, E>>& edge : edges) {
				auto [src, dst] = (*edge).get_nodes();

				// SAFETY COMMENT: find_node_by_value() should always has value, because we already inserted all nodes
				// into the graph, in the above for loop.
				auto& src_ptr = find_node_by_value(src);
				auto& dst_ptr = find_node_by_value(dst);
				auto weight = (*edge).get_weight();
				insert_edge(*src_ptr, *dst_ptr, weight);
			}
		}
	}

	template<typename N, typename E>
	auto Graph<N, E>::operator=(Graph const& other) -> Graph& {
		if (this == &other) {
			return *this;
		}

		this->swap(Graph(other));
		return *this;
	}

	template<typename N, typename E>
	auto Graph<N, E>::swap(Graph& other) noexcept -> void {
		std::swap(nodes_, other.nodes_);
		std::swap(adjacency_list_, other.adjacency_list_);
	}

	template<typename N, typename E>
	auto Graph<N, E>::find_node_by_value(N const& value) -> const std::unique_ptr<N>& {
		auto it = nodes_.find(std::make_unique<N>(value));
		return *it;
	}

	template<typename N, typename E>
	auto Graph<N, E>::UniquePtrValueComparator::operator()(const std::unique_ptr<N>& a, const std::unique_ptr<N>& b) const
	    -> bool {
		return *a < *b;
	}

	template<typename N, typename E>
	auto Graph<N, E>::RawPtrValueComparator::operator()(const N* a, const N* b) const -> bool {
		return *a < *b;
	}

	// =================MODIFIERS===================
	template<typename N, typename E>
	auto Graph<N, E>::insert_node(N const& value) -> bool {
		auto new_unique_ptr = std::make_unique<N>(value);
		if (nodes_.contains(new_unique_ptr)) {
			return false;
		}
		auto [it_new_elem, _] = nodes_.insert(std::move(new_unique_ptr));
		adjacency_list_.insert(std::make_pair((*it_new_elem).get(), edge_set{}));
		return true;
	}

	template<typename N, typename E>
	auto Graph<N, E>::insert_edge(N const& src, N const& dst, std::optional<E> weight) -> bool {
		if (!is_node(src) || !is_node(dst)) {
			throw std::runtime_error("Cannot call gdwg::Graph<N, E>::insert_edge when either src or dst node does not "
			                         "exist");
		}

		auto& src_ptr = find_node_by_value(src);
		auto& dst_ptr = find_node_by_value(dst);

		std::unique_ptr<Edge<N, E>> new_edge;
		if (weight.has_value()) {
			new_edge = std::make_unique<WeightedEdge<N, E>>(*src_ptr, *dst_ptr, weight.value());
		}
		else {
			new_edge = std::make_unique<UnweightedEdge<N, E>>(*src_ptr, *dst_ptr);
		}

		auto& edges_from_src = adjacency_list_.at(src_ptr.get());
		if (edges_from_src.contains(new_edge)) {
			return false;
		}
		edges_from_src.insert(std::move(new_edge));
		return true;
	}

	// =================ACCESSORS===================
	template<typename N, typename E>
	[[nodiscard]] auto Graph<N, E>::is_node(N const& value) -> bool {
		return nodes_.contains(std::make_unique<N>(value)); // O(log(n)) because nodes_ is a set.
	}

	template<typename N, typename E>
	[[nodiscard]] auto Graph<N, E>::empty() -> bool {
		return nodes_.size() == 0 && adjacency_list_.size() == 0;
	}

	template<typename N, typename E>
	auto Graph<N, E>::is_connected(N const& src, N const& dst) -> bool {
		if (!is_node(src) || !is_node(dst)) {
			throw std::runtime_error("Cannot call gdwg::Graph<N, E>::is_connected if src or dst node don't exist in "
			                         "the graph");
		}

		auto& src_ptr = find_node_by_value(src);
		auto& edges_from_src = adjacency_list_.at(src_ptr.get());

		return std::find_if(edges_from_src.begin(),
		                    edges_from_src.end(),
		                    [&dst](auto const& edge) {
			                    N edge_dst = (*edge).get_nodes().second;
			                    return dst == edge_dst;
		                    })
		       != edges_from_src.end();
	}

	template<typename N, typename E>
	auto Graph<N, E>::nodes() -> std::vector<N> {
		auto returned_nodes = std::vector<N>{};

		for (auto& node : nodes_) {
			returned_nodes.push_back(*node);
		}
		return returned_nodes;
	}

	template<typename N, typename E>
	auto Graph<N, E>::edges(N const& src, N const& dst) -> std::vector<std::unique_ptr<Edge<N, E>>> {
		if (!is_node(src) || !is_node(dst)) {
			throw std::runtime_error("Cannot call gdwg::Graph<N, E>::edges if src or dst node don't exist in the "
			                         "graph");
		}

		auto& src_ptr = find_node_by_value(src); // O(log(n))
		auto& edges_from_src = adjacency_list_.at(src_ptr.get()); // O(log(n))

		auto is_matched_dst = [&dst](auto const& edge) {
			N edge_dst = (*edge).get_nodes().second;
			return dst == edge_dst;
		};

		auto first_occurrence_it = std::find_if(edges_from_src.begin(), edges_from_src.end(), is_matched_dst); // O(e)

		auto returned_edges = std::vector<std::unique_ptr<Edge<N, E>>>{};
		for (auto& it = first_occurrence_it; it != edges_from_src.end(); it++) { // O(e)
			std::unique_ptr<Edge<N, E>> const& edge_ptr = *it;
			if (is_matched_dst(edge_ptr)) {
				if (edge_ptr->is_weighted()) {
					auto const& temp = static_cast<WeightedEdge<N, E>&>(*edge_ptr);
					returned_edges.push_back(std::make_unique<WeightedEdge<N, E>>(temp));
				}
				else {
					auto const& temp = static_cast<UnweightedEdge<N, E>&>(*edge_ptr);
					returned_edges.push_back(std::make_unique<UnweightedEdge<N, E>>(temp));
				}
			}
			else {
				break;
			}
		}
		return returned_edges;
	}

	template<typename N, typename E>
	auto Graph<N, E>::connections(N const& src) -> std::vector<N> {
		if (!is_node(src)) {
			throw std::runtime_error("Cannot call gdwg::Graph<N, E>::connections if src doesn't exist in the "
			                         "graph");
		}
		auto& src_ptr = find_node_by_value(src); // O(log(n))
		auto& edges_from_src = adjacency_list_.at(src_ptr.get()); // O(log(n))

		auto returned_nodes = std::vector<N>{};
		for (auto& edge_ptr : edges_from_src) { // O(e)
			auto const dst = edge_ptr->get_nodes().second;
			if (returned_nodes.empty() || returned_nodes.back() != dst) {
				returned_nodes.push_back(dst);
			}
		}
		return returned_nodes;
	}

	// =================COMPARISONS===================
	template<typename N, typename E>
	[[nodiscard]] auto Graph<N, E>::operator==(Graph<N, E> const& other) const -> bool {
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

		// comparing each edge
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
			if (*edge_a.dst_ == *edge_b.dst_) {
				// 2 unweighted edges are equivalent, hence must return false, because set uses equivalence relation to
				// determine if 2 things are the same,  i.e. if !comp(a, b) && !comp(b, a) then a == b
				if (!edge_a.get_weight().has_value() && !edge_b.get_weight().has_value())
					return false;

				if (!edge_a.get_weight().has_value())
					return true;

				if (!edge_b.get_weight().has_value())
					return false;

				return (edge_a).get_weight().value() < (edge_b).get_weight().value();
			}
			return *edge_a.dst_ < *edge_b.dst_;
		}
		return *edge_a.src_ < *edge_b.src_;
	}

	template<typename N, typename E>
	Edge<N, E>::Edge(N const& src, N const& dst)
	: src_{&src}
	, dst_{&dst} {}

	template<typename N, typename E>
	Edge<N, E>::Edge(Edge const& other)
	: src_{other.src_}
	, dst_{other.dst_} {}

	template<typename N, typename E>
	auto Edge<N, E>::get_nodes() const -> std::pair<N, N> {
		return std::make_pair(*src_, *dst_);
	}

	template<typename N, typename E>
	[[nodiscard]] auto Edge<N, E>::operator==(Edge const& other) -> bool {
		if (*src_ != *other.src_)
			return false;
		if (*dst_ != *other.dst_)
			return false;

		return get_weight() == other.get_weight();
	}

	template<typename N, typename E>
	WeightedEdge<N, E>::WeightedEdge(N const& src, N const& dst, E const& weight)
	: Edge<N, E>(src, dst)
	, weight_{std::make_unique<E>(weight)} {}

	template<typename N, typename E>
	WeightedEdge<N, E>::WeightedEdge(WeightedEdge const& other)
	: Edge<N, E>(other)
	, weight_{std::make_unique<E>(*other.weight_)} {}

	template<typename N, typename E>
	auto WeightedEdge<N, E>::print_edge() -> std::string {
		auto ss = std::stringstream{};
		ss << *(this->src_) << " -> ";
		ss << *(this->dst_) << " | W | ";
		ss << *weight_;
		return ss.str();
	}

	template<typename N, typename E>
	auto WeightedEdge<N, E>::is_weighted() -> bool {
		return true;
	}

	template<typename N, typename E>
	auto WeightedEdge<N, E>::get_weight() const -> std::optional<E> {
		return *weight_;
	}

	template<typename N, typename E>
	UnweightedEdge<N, E>::UnweightedEdge(N const& src, N const& dst)
	: Edge<N, E>(src, dst) {}

	template<typename N, typename E>
	UnweightedEdge<N, E>::UnweightedEdge(UnweightedEdge const& other)
	: Edge<N, E>(other) {}

	template<typename N, typename E>
	auto UnweightedEdge<N, E>::print_edge() -> std::string {
		auto ss = std::stringstream{};
		ss << *(this->src_) << " -> ";
		ss << *(this->dst_) << " | U";
		return ss.str();
	}

	template<typename N, typename E>
	auto UnweightedEdge<N, E>::is_weighted() -> bool {
		return false;
	}

	template<typename N, typename E>
	auto UnweightedEdge<N, E>::get_weight() const -> std::optional<E> {
		return std::nullopt;
	}
} // namespace gdwg

#endif // GDWG_GRAPH_H
