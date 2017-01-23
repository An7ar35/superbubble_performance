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
            ~PartitionGraph();
            //Type definition
            typedef std::list<graph::SubGraph> SubGraphList_t;
            //Partitioning method
            std::unique_ptr<SubGraphList_t> partitionSCCs( const eadlib::WeightedGraph<size_t> &base_graph,
                                                           const std::list<std::list<size_t>> &scc_lists,
                                                           const std::string &sb_name_prefix );

          private:
            void partitionSCC( const eadlib::WeightedGraph<size_t> &base_graph,
                               const std::list<size_t> &scc,
                               const std::string &subGraph_name );
            void partitionSingletonSCCs( const eadlib::WeightedGraph<size_t> &base_graph,
                                         const std::list<size_t> &scc,
                                         const std::string &subGraph_name );
            std::unique_ptr<SubGraphList_t> _sub_graphs;
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_GRAPHPARTITIONER_H
