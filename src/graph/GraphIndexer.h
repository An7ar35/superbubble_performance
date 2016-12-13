#ifndef SUPERBUBBLE_PERFORMANCE_GRAPHINDEXER_H
#define SUPERBUBBLE_PERFORMANCE_GRAPHINDEXER_H

#include "eadlib/datastructure/WeightedGraph.h"
#include "eadlib/cli/graphic/ProgressBar.h"
#include "../io/Database.h"

namespace sbp {
    namespace graph {
        class GraphIndexer {
          public:
            GraphIndexer( sbp::io::Database &db );
            ~GraphIndexer();
            bool storeIntoDB( const std::string &graph_name, const eadlib::WeightedGraph<std::string> &graph );
          private:
            sbp::io::Database &_db;
        };
    }
}


#endif //SUPERBUBBLE_PERFORMANCE_GRAPHINDEXER_H
