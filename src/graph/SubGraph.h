#ifndef SUPERBUBBLE_PERFORMANCE_SUBGRAPH_H
#define SUPERBUBBLE_PERFORMANCE_SUBGRAPH_H

#include <vector>
#include <eadlib/datastructure/Graph.h>
#include <eadlib/datastructure/WeightedGraph.h>

namespace sbp {
    namespace graph {
        class SubGraph {
          public:
            SubGraph( const eadlib::WeightedGraph<size_t> &base_graph,
                      const std::list<size_t> &scc,
                      const std::string &subGraph_name = "subgraph" );
            ~SubGraph();
            //Iterator
            typedef eadlib::Graph<size_t>::Graph_t::const_iterator const_iterator;
            const_iterator begin() const;
            const_iterator end() const;
            //Graph access
            const_iterator find( const size_t &node ) const;
            const eadlib::Graph<size_t>::NodeAdjacency & at( const size_t &node ) const;
            size_t getSourceID() const;
            size_t getTerminalID() const;
            //Translation
            size_t getGlobalID( const size_t local ) const;
            //State
            size_t size() const;
            size_t nodeCount() const;


          private:
            eadlib::Graph<size_t> _sub_graph;
            std::vector<size_t> _node_map;
            size_t _edge_count;
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_SUBGRAPH_H
