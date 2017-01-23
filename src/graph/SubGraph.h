#ifndef SUPERBUBBLE_PERFORMANCE_SUBGRAPH_H
#define SUPERBUBBLE_PERFORMANCE_SUBGRAPH_H

#include <vector>
#include <iterator>
#include <eadlib/datastructure/Graph.h>
#include <eadlib/datastructure/WeightedGraph.h>

namespace sbp {
    namespace graph {
        class SubGraph : public eadlib::Graph<size_t> {
          public:
            SubGraph( const std::string &name );
            ~SubGraph();
            //Manipulation
            bool addNode( const size_t &node ) override;
            //Access
            const_iterator findLocalID( const size_t &node ) const;
            const_iterator findGlobalID( const size_t &node ) const;
            size_t getSourceID() const;
            size_t getTerminalID() const;
            //Translation
            size_t getGlobalID( const size_t local ) const;
            size_t getLocalID( const size_t global ) const;
            //Print
            std::ostream & printLocal( std::ostream &out ) const;

            std::ostream & printGlobal( std::ostream &out ) const;

          private:
            std::unordered_map<size_t, size_t> _local2global_map; //local to global ID lookup
            std::unordered_map<size_t, size_t> _global2local_map; //reverse ID lookup
            size_t                             _entrance_node;    // r
            size_t                             _exit_node;        // r'
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_SUBGRAPH_H
