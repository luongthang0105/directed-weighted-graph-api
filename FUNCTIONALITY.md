# Functionality Reference

This document captures the detailed behavior contract for the generic directed weighted graph API.
It is intentionally spec-style: what each operation does, its pre/postconditions, and target complexity.

## Model and Invariants

- The graph is directed.
- Nodes are unique by value (`operator==` equivalence).
- Edges may be unweighted or weighted.
- For a given `(src, dst)`:
  - at most one unweighted edge exists;
  - weighted edges are unique by weight.
- Edge ordering: source, then destination, then weight; unweighted comes before weighted for equal source/destination.
- Reflexive edges (`src == dst`) are valid.

## Complexity Symbols

- `n`: number of stored nodes.
- `e`: context-dependent edge count (as defined per function).
- `d`: distance between iterators in a range erase.

## Constructor and Assignment

### `Graph()`
- **Functionality:** value-initializes all members.
- **Preconditions:** none.
- **Postconditions:** graph is valid and empty.

### `Graph(std::initializer_list<N> il)`
- **Functionality:** constructs nodes from the initializer list.
- **Preconditions:** `N` is copyable/comparable.
- **Postconditions:** all unique values from `il` exist as nodes.

### `template<typename InputIt> Graph(InputIt first, InputIt last)`
- **Functionality:** constructs nodes from `[first, last)`.
- **Preconditions:** `InputIt` models input iterator and dereferences to `N`.
- **Postconditions:** all unique values in range exist as nodes.

### `Graph(Graph&& other) noexcept`
- **Functionality:** move-constructs from `other`.
- **Preconditions:** none.
- **Postconditions:** 
  - `*this` equals old `other`; 
  - `other.empty() == true`.
  - All iterators pointing to elements owned by `*this` prior to this operator’s invocation are invalidated.
  - All iterators pointing to elements owned by `other` prior to this operator’s invocation remain valid, but now point to the elements owned by `*this`.

### `auto operator=(Graph&& other) noexcept -> Graph&`
- **Functionality:** move-assigns graph state.
- **Preconditions:** none.
- **Postconditions:** 
  - `*this` equals old `other`; 
  - `other.empty() == true`.
  - All iterators pointing to elements owned by `*this` prior to this operator’s invocation are invalidated.
  - All iterators pointing to elements owned by `other` prior to this operator’s invocation remain valid, but now point to the elements owned by `*this`.


### `Graph(Graph const& other)`
- **Functionality:** deep-copy constructs.
- **Preconditions:** none.
- **Postconditions:** `*this == other`.

### `auto operator=(Graph const& other) -> Graph&`
- **Functionality:** deep-copy assignment.
- **Preconditions:** none.
- **Postconditions:** 
  - `*this == other`; 
  - All iterators pointing to elements owned by `*this` prior to this operator’s invocation are invalidated; 
  - Returns `*this`.

## Edge Hierarchy

## `Edge<N, E>` (abstract base)

### `auto print_edge() -> std::string`
- **Functionality:** returns formatted edge text.
- **Preconditions:** edge is valid.
- **Postconditions:** no state mutation.
- **Complexity:** `O(1)` excluding stream formatting cost.

### `auto is_weighted() -> bool`
- **Functionality:** reports whether edge has weight.
- **Preconditions:** edge is valid.
- **Postconditions:** no state mutation.
- **Complexity:** `O(1)`.

### `auto get_weight() -> std::optional<E>`
- **Functionality:** returns weight or `std::nullopt`.
- **Preconditions:** edge is valid.
- **Postconditions:** no state mutation.
- **Complexity:** `O(1)`.

### `auto get_nodes() -> std::pair<N, N>`
- **Functionality:** returns `(src, dst)`.
- **Preconditions:** edge is valid.
- **Postconditions:** no state mutation.
- **Complexity:** `O(1)`.

### `auto operator==(Edge const& other) -> bool`
- **Functionality:** value-comparison by src, dst, and weight.
- **Preconditions:** edges are comparable.
- **Postconditions:** no state mutation.
- **Complexity:** `O(1)`.

## `WeightedEdge<N, E>`

### `WeightedEdge(N const& src, N const& dst, E const& weight)`
- **Functionality:** creates weighted edge.
- **Preconditions:** node and weight values are valid/copyable.
- **Postconditions:** `is_weighted() == true`.

## `UnweightedEdge<N, E>`

### `UnweightedEdge(N const& src, N const& dst)`
- **Functionality:** creates unweighted edge.
- **Preconditions:** node values are valid/copyable.
- **Postconditions:** `is_weighted() == false`.

## Modifiers

### `auto insert_node(N const& value) -> bool`
- **Functionality:** inserts node if not already present.
- **Preconditions:** `N` is comparable.
- **Postconditions:** graph contains `value` iff return is `true`.
- **Complexity:** `O(log n)`.

### `auto insert_edge(N const& src, N const& dst, std::optional<E> weight = std::nullopt) -> bool`
- **Functionality:** inserts weighted/unweighted edge if unique.
- **Preconditions:** both `src` and `dst` nodes exist.
- **Postconditions:** edge exists iff return is `true`.
- **Throws:** `std::runtime_error` if `src` or `dst` missing.
- **Complexity:** `O(log n + log e_src)` typical set/map implementation.

### `auto replace_node(N const& old_data, N const& new_data) -> bool`
- **Functionality:** renames a node if target name does not already exist.
- **Preconditions:** `old_data` exists.
- **Postconditions:** if return is `true`, old node replaced with new and incident edges remapped.
- **Throws:** `std::runtime_error` if `old_data` missing.
- **Complexity:** up to `O(e log n)` due to remapping.

### `auto merge_replace_node(N const& old_data, N const& new_data) -> void`
- **Functionality:** merges `old_data` into `new_data`; rewires incoming/outgoing edges; removes duplicates.
- **Preconditions:** both nodes exist.
- **Postconditions:** `old_data` removed; equivalent connectivity preserved via `new_data`.
- **Throws:** `std::runtime_error` if either node missing.
- **Complexity:** up to `O(e log n)`.

### `auto erase_node(N const& value) -> bool`
- **Functionality:** erases node and all incident edges.
- **Preconditions:** none.
- **Postconditions:** node absent; no edge references to node.
- **Complexity:** `O(log n + e)` (node lookup + incident/global cleanup).

### `auto erase_edge(N const& src, N const& dst, std::optional<E> weight = std::nullopt) -> bool`
- **Functionality:** erases matching edge.
- **Preconditions:** `src` and `dst` nodes exist.
- **Postconditions:** matching edge absent if it existed.
- **Throws:** `std::runtime_error` if `src` or `dst` missing.
- **Complexity:** target requirement `O(log n + e)`.

### `auto erase_edge(iterator i) -> iterator`
- **Functionality:** erases edge at iterator and returns next iterator.
- **Preconditions:** `i` is valid and dereferenceable.
- **Postconditions:** pointed edge removed; iterators considered invalidated by contract.
- **Complexity:** target requirement `O(log n + e)`.

### `auto erase_edge(iterator i, iterator s) -> iterator`
- **Functionality:** erases edge range `[i, s)`.
- **Preconditions:** valid range in the same graph, `s` strictly after `i`.
- **Postconditions:** range removed; iterators considered invalidated by contract.
- **Complexity:** target requirement `O(d * (log n + e))`.

### `auto clear() noexcept -> void`
- **Functionality:** clears all nodes and edges.
- **Preconditions:** none.
- **Postconditions:** `empty() == true`.
- **Complexity:** `O(n + e)`.

## Accessors

### `[[nodiscard]] auto is_node(N const& value) -> bool`
- **Functionality:** checks node existence.
- **Preconditions:** none.
- **Postconditions:** no mutation.
- **Complexity:** `O(log n)`.

### `[[nodiscard]] auto empty() -> bool`
- **Functionality:** checks if graph has no nodes.
- **Preconditions:** none.
- **Postconditions:** no mutation.
- **Complexity:** `O(1)`.

### `[[nodiscard]] auto is_connected(N const& src, N const& dst) -> bool`
- **Functionality:** checks if any edge from `src` to `dst` exists.
- **Preconditions:** `src` and `dst` nodes exist.
- **Postconditions:** no mutation.
- **Throws:** `std::runtime_error` if either node missing.
- **Complexity:** `O(log n + e_src)`.

### `[[nodiscard]] auto nodes() -> std::vector<N>`
- **Functionality:** returns sorted node values.
- **Preconditions:** none.
- **Postconditions:** returned values are copies.
- **Complexity:** `O(n)`.

### `[[nodiscard]] auto edges(N const& src, N const& dst) -> std::vector<std::unique_ptr<Edge<N,E>>>`
- **Functionality:** returns edges from `src` to `dst`, with unweighted first, then weighted ascending by weight.
- **Preconditions:** `src` and `dst` nodes exist.
- **Postconditions:** returned edges are owning copies.
- **Throws:** `std::runtime_error` if either node missing.
- **Complexity:** target requirement `O(log n + e)`.

### `[[nodiscard]] auto find(N const& src, N const& dst, std::optional<E> weight = std::nullopt) -> iterator`
- **Functionality:** locates exact edge and returns iterator (or `end()`).
- **Preconditions:** none in logical contract.
- **Postconditions:** no mutation.
- **Complexity:** target requirement `O(log n + e)`.

### `[[nodiscard]] auto connections(N const& src) -> std::vector<N>`
- **Functionality:** returns sorted unique destination nodes directly reachable from `src`.
- **Preconditions:** `src` exists.
- **Postconditions:** returned values are copies.
- **Throws:** `std::runtime_error` if `src` missing.
- **Complexity:** `O(log n + e_out)`.

## Iterator Access

### `[[nodiscard]] auto begin() const -> iterator`
- **Functionality:** returns iterator to first traversable edge.
- **Preconditions:** none.
- **Postconditions:** valid begin/end pair semantics.
- **Complexity:** `O(1)` expected.

### `[[nodiscard]] auto end() const -> iterator`
- **Functionality:** returns sentinel iterator.
- **Preconditions:** none.
- **Postconditions:** defines `[begin(), end())` traversal range.
- **Complexity:** `O(1)`.

## Comparisons

### `[[nodiscard]] auto operator==(Graph const& other) -> bool`
- **Functionality:** compares graph value equality (same nodes and edges).
- **Preconditions:** none.
- **Postconditions:** no mutation.
- **Complexity:** `O(n + e)`.

## Stream Output

### `friend auto operator<<(std::ostream& os, Graph const& g) -> std::ostream&`
- **Functionality:** prints nodes and outgoing edges in sorted deterministic format.
- **Preconditions:** stream is writable.
- **Postconditions:** stream updated with graph text.
- **Complexity:** `O(n + e)` excluding stream I/O overhead.

## Iterator Type

Iterator value type:

```cpp
struct {
  N from;
  N to;
  std::optional<E> weight;
}
```

- Nodes with zero outgoing edges are skipped during traversal.
- Supports bidirectional traversal intent (`++`, `--`, postfix variants).
- `operator*` returns current edge triple.
- `operator==` compares position/state.

