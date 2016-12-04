#ifndef SUPERBUBBLE_PERFORMANCE_DOTEXPORT_H
#define SUPERBUBBLE_PERFORMANCE_DOTEXPORT_H

#include "eadlib/io/FileWriter.h"
#include "eadlib/logger/Logger.h"
#include "eadlib/datastructure/WeightedGraph.h"

namespace sbp {
    namespace io {
        class DotExport {
          public:
            enum class EdgeType { MULTI_EDGE, WEIGHT_LABEL};
            DotExport( eadlib::io::FileWriter &writer );
            ~DotExport();
            bool exportToDot( const std::string &graph_name,
                              const eadlib::WeightedGraph<std::string> &graph,
                              const bool &weight_label );
          private:
            eadlib::io::FileWriter &_writer;
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_DOTEXPORT_H
