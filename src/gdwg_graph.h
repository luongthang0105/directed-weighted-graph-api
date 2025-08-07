#ifndef GDWG_GRAPH_H
#define GDWG_GRAPH_H

#include <map>
#include <memory>
#include <optional>
#include <set>

namespace gdwg {
	template<typename N, typename E>
	class Edge {
	 public:
		Edge(N const& src, N const& dst)
		: src_{src}
		, dst_{dst} {}

		virtual auto print_edge() -> std::string = 0;
		virtual auto is_weighted() -> bool = 0;
		virtual auto get_weight() -> std::optional<E> = 0;
		auto get_nodes() -> std::pair<N, N>;

		virtual ~Edge() = default;

		[[nodiscard]] auto operator==(Edge const& other) -> bool;

		struct UniquePtrEdgeComparator {
			auto operator()(const std::unique_ptr<Edge<N, E>>& a, const std::unique_ptr<Edge<N, E>>& b) const -> bool;
		};

	 private:
		template<typename G_N, typename G_E>
		friend class Graph;

	 protected:
		N* src_;
		N* dst_;
	};

	template<typename N, typename E>
	class WeightedEdge : public Edge<N, E> {
	 public:
		WeightedEdge(N const& src, N const& dst, E const& weight);

		auto print_edge() -> std::string override;
		auto is_weighted() -> bool override;
		auto get_weight() -> std::optional<E> override;

	 private:
		std::unique_ptr<E> weight;
	};

	template<typename N, typename E>
	class UnweightedEdge : public Edge<N, E> {
	 public:
		UnweightedEdge(N const& src, N const& dst);

		auto print_edge() -> std::string override;
		auto is_weighted() -> bool override;
		auto get_weight() -> std::optional<E> override;

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

		// =================ACCESSORS===================
		[[nodiscard]] auto is_node(N const& value) -> bool;
		[[nodiscard]] auto empty() -> bool;

		// =================COMPARISONS===================
		[[nodiscard]] auto operator==(Graph const& other) -> bool;

	 private:
		struct UniquePtrValueComparator {
			auto operator()(const std::unique_ptr<N>& a, const std::unique_ptr<N>& b) const -> bool;
		};

		struct RawPtrValueComparator {
			auto operator()(const N*& a, const N*& b) const -> bool;
		};

		auto swap(Graph& other) noexcept -> void;

		std::set<std::unique_ptr<N>, UniquePtrValueComparator> nodes_;
		std::map<N*, std::set<std::unique_ptr<Edge<N, E>>, typename Edge<N, E>::UniquePtrEdgeComparator>, RawPtrValueComparator>
		    adjacency_list_;
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

#endif // GDWG_GRAPH_H
