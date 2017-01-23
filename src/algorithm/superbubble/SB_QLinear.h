/**
    @class          sbp::algo::SB_QLinear
    @brief          Implementation of the O(m log m) superbubble algorithm

                    Based on: An O(_m_ log _m_)-Time Algorithm for Detecting Superbubbles
                    by Wing-Kin Sung, Kunihiko Sadakane, Tetsuo Shibuya, Abha Belorkar, and Iana Pyrogova,
                    IEEE/ACM Transactions on Computational Biology and Bioinformatics, Vol. 12, No. 4, July/August 2015

    @dependencies   eadlib::WeightedGraph<T>, eadlib::Graph<T>,
                    sbp::algo::container::SuperBubble, sbp::algo::Tarjan

    @author         E. A. Davison
    @copyright      E. A. Davison 2016
    @license        GNUv2 Public License
**/

#ifndef SUPERBUBBLE_PERFORMANCE_SB_QLINEAR_H
#define SUPERBUBBLE_PERFORMANCE_SB_QLINEAR_H

#include <vector>
#include <memory>

#include <eadlib/datastructure/Graph.h>
#include <eadlib/datastructure/WeightedGraph.h>

#include "container/SuperBubble.h"
#include "../Tarjan.h"
#include "../PartitionGraph.h"

namespace sbp {
    namespace algo {
        class SB_QLinear {
          public:
            SB_QLinear( const eadlib::WeightedGraph<size_t> &graph );
            ~SB_QLinear();
            bool run( std::list<container::SuperBubble> &superbubble_list );
          private:
            const eadlib::WeightedGraph<size_t> _graph;
        };
    }
}


#endif //SUPERBUBBLE_PERFORMANCE_SB_QLINEAR_H
