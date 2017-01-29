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
#include <eadlib/logger/Logger.h>
#include "../graph/SubGraph.h"
#include "../graph/DAG.h"

namespace sbp {
    namespace algo {
        class GraphToDAG {
          public:
            struct DAG_Package {
                DAG_Package( const std::string &dag_name, const size_t &sg_node_count ) :
                    _dag( graph::DAG( dag_name ) ),
                    _discovery_times( sg_node_count ),
                    _finish_times( sg_node_count )
                {};
                graph::DAG _dag;
                std::vector<size_t> _discovery_times;
                std::vector<size_t> _finish_times;
            };
            typedef std::list<DAG_Package> DAG_List_t;
            GraphToDAG();
            ~GraphToDAG();
            //Converters
            std::unique_ptr<DAG_List_t> convertToDAG( const std::list<graph::SubGraph> &sg_list,
                                                      const std::string &dag_name_prefix );
          private:
            enum class DFSColours {
                WHITE,
                GREY,
                BLACK
            };
            void visitUsingDFS( const sbp::graph::SubGraph &sub_graph,
                                const size_t &u,
                                std::vector<sbp::algo::GraphToDAG::DFSColours> &colour,
                                size_t time,
                                DAG_Package &dag_pack );
            void convertToDAG( const graph::SubGraph &sub_graph,
                               const std::string &dag_name );
            std::unique_ptr<DAG_List_t> _dag_package_list;
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_GRAPHTODAG_H
