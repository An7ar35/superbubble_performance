#ifndef SUPERBUBBLE_PERFORMANCE_SB_LINEAR_H
#define SUPERBUBBLE_PERFORMANCE_SB_LINEAR_H

#include <eadlib/datastructure/WeightedGraph.h>

#include "../Tarjan.h"

namespace sbp {
    namespace algo {
        class SB_Linear {
          public:
            SB_Linear( const eadlib::WeightedGraph<size_t> &graph );
            ~SB_Linear();
            size_t run();
          private:
            const eadlib::WeightedGraph<size_t> _graph;
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_SB_LINEAR_H
