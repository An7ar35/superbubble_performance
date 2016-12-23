/**
    @class          sbp::algo::SB_Linear
    @brief          Implementation of the O(n + m) superbubble algorithm

                    //TODO cite here!!

    @dependencies   eadlib::WeightedGraph<T>, eadlib::Graph<T>,
                    sbp::algo::container::SuperBubble, sbp::algo::Tarjan

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

namespace sbp {
    namespace algo {
        class SB_Linear {
          public:
            SB_Linear( const eadlib::WeightedGraph<size_t> &graph );
            ~SB_Linear();
            bool run( std::list<container::SuperBubble> &superbubble_list );
          private:
            size_t concatenateSingletonSCCs( std::list<std::list<size_t>> &scc_list );
            const eadlib::WeightedGraph<size_t> _graph;
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_SB_LINEAR_H
