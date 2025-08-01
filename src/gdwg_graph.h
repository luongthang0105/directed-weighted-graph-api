#ifndef GDWG_GRAPH_H
#define GDWG_GRAPH_H

// TODO: Make both graph and edge generic
//       ... this won't just compile
//       straight away
namespace gdwg {
	template<typename N, typename E>
	class Edge {
	 public:
	 private:
		// You may need to add data members and member functions
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
		// Your member functions go here
	 private:
	};
} // namespace gdwg

#endif // GDWG_GRAPH_H
