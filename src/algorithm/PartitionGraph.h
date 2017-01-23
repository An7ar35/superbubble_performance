/**
    @class          sbp::algo::GraphPartitioner
    @brief          Graph partitioning algorithm

    Implementation of the 'PartitionGraph(H)' algorithm found in the Quasi-Linear SuperBubble algorithm paper
    See the README.md
**/
#ifndef SUPERBUBBLE_PERFORMANCE_GRAPHPARTITIONER_H
#define SUPERBUBBLE_PERFORMANCE_GRAPHPARTITIONER_H

#include <list>
#include <eadlib/datastructure/WeightedGraph.h>
#include "../graph/SubGraph.h"

namespace sbp {
    namespace algo {
        class PartitionGraph {
          public:
            PartitionGraph();
            PartitionGraph( const PartitionGraph& ) = delete;
            PartitionGraph( PartitionGraph &&p );
            ~PartitionGraph();
            //Operator
            PartitionGraph & operator =( const PartitionGraph & ) = delete;
            PartitionGraph & operator =( PartitionGraph &&p );
            //Iterators
            typedef std::list<graph::SubGraph>::iterator iterator;
            typedef std::list<graph::SubGraph>::const_iterator const_iterator;
            iterator begin();
            iterator end();
            const_iterator cbegin();
            const_iterator cend();
            //Partition algo functions
            iterator partitionSCC( const eadlib::WeightedGraph<size_t> &base_graph,
                                   const std::list<size_t> &scc,
                                   const std::string &subGraph_name );
            iterator partitionSingletonSCCs( const eadlib::WeightedGraph<size_t> &base_graph,
                                             const std::list<size_t> &scc,
                                             const std::string &subGraph_name );
            //State
            bool empty() const;
            size_t size() const;

          private:
            std::unique_ptr<std::list<graph::SubGraph>> _sub_graphs;
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_GRAPHPARTITIONER_H
