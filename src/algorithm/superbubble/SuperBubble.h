#ifndef SUPERBUBBLE_PERFORMANCE_SUPERBUBBLE_H
#define SUPERBUBBLE_PERFORMANCE_SUPERBUBBLE_H

#include <eadlib/io/FileWriter.h>
#include <eadlib/datastructure/WeightedGraph.h>

#include "SB_Linear.h"

namespace sbp {
    namespace algo {
        class SuperBubble {
          public:
            SuperBubble( eadlib::io::FileWriter &writer );
            ~SuperBubble();
            void runLinear( const eadlib::WeightedGraph<size_t> &graph );
          private:
            eadlib::io::FileWriter &_writer;
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_SUPERBUBBLE_H
