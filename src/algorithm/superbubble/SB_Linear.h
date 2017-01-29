/**
    @class          sbp::algo::SB_Linear
    @brief          Implementation of the O(n + m) superbubble algorithm

                    Based on: "Linear-Time Superbubble Identification Algorithm for Genome Assembly"
                    by L. Brankovic, C. S. Iliopoulos, R. Kundu, M. Mohamed, S. P. Pissis, F. Vayani,
                    Theoretical Computer Science, 2015.

    @dependencies   eadlib::WeightedGraph<T>, eadlib::Graph<T>,
                    sbp::algo::container::SuperBubble, sbp::algo::Tarjan, sbp::algo::GraphToDAG

    @author         E. A. Davison
    @copyright      E. A. Davison 2016
    @license        GNUv2 Public License
**/
#ifndef SUPERBUBBLE_PERFORMANCE_SB_LINEAR_H
#define SUPERBUBBLE_PERFORMANCE_SB_LINEAR_H

#include <vector>
#include <memory>

#include <eadlib/datastructure/Graph.h>
#include <eadlib/datastructure/WeightedGraph.h>

#include "container/SuperBubble.h"
#include "../Tarjan.h"
#include "../PartitionGraph.h"
#include "../GraphToDAG.h"

namespace sbp {
    namespace algo {
        class SB_Linear {
          public:
            SB_Linear( const eadlib::WeightedGraph<size_t> &graph );
            ~SB_Linear();
            bool run( std::list<container::SuperBubble> &superbubble_list );
          private:
            const eadlib::WeightedGraph<size_t> _graph;
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_SB_LINEAR_H
