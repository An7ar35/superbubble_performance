/**
    @class          sbp::algo::GraphToDAG
    @brief          Graph to DAG algorithm

    Implementation of the 'GraphToDAG(G)' algorithm found in the Quasi-Linear SuperBubble algorithm paper
    See the README.md

    @dependencies   sbp::graph::SubGraph, sbp::graph::DAG
**/
#ifndef SUPERBUBBLE_PERFORMANCE_GRAPHTODAG_H
#define SUPERBUBBLE_PERFORMANCE_GRAPHTODAG_H

#include <list>
#include "../graph/SubGraph.h"
#include "../graph/DAG.h"

namespace sbp {
    namespace algo {
        class GraphToDAG {
          public:
            typedef std::list<graph::DAG> DAG_List_t;
            GraphToDAG();
            ~GraphToDAG();
            //Converters
            std::unique_ptr<DAG_List_t> convertToDAG( const std::list<graph::SubGraph> &sg_list,
                                                      const std::string &dag_name_prefix );
          private:
            void convertToDAG( const graph::SubGraph &sub_graph,
                               const std::string &dag_name );
            std::unique_ptr<DAG_List_t> _dag;
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_GRAPHTODAG_H
