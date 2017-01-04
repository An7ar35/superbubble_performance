/**
    @class          sbp::algo::SB_Driver
    @brief          Driver for running superbubble algorithms
    @dependencies   eadlib::WeightedGraph<T>, eadlib::io::FileWriter, sbp::algo::SB_Linear
    @author         E. A. Davison
    @copyright      E. A. Davison 2016
    @license        GNUv2 Public License
**/
#ifndef SUPERBUBBLE_PERFORMANCE_SB_DRIVER_H
#define SUPERBUBBLE_PERFORMANCE_SB_DRIVER_H

#include <eadlib/io/FileWriter.h>
#include <eadlib/datastructure/WeightedGraph.h>

#include "SB_Linear.h"
#include "SB_QLinear.h"

namespace sbp {
    namespace algo {
        class SB_Driver {
          public:
            SB_Driver( eadlib::io::FileWriter &writer );
            ~SB_Driver();
            void runLinear( const eadlib::WeightedGraph<size_t> &graph, std::list<container::SuperBubble> &sb_list );
            void runQLinear( const eadlib::WeightedGraph<size_t> &graph, std::list<container::SuperBubble> &sb_list );
          private:
            eadlib::io::FileWriter &_writer;
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_SB_DRIVER_H
