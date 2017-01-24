#include "GraphToDAG.h"

/**
 * Constructor
 */
sbp::algo::GraphToDAG::GraphToDAG() {}

/**
 * Destructor
 */
sbp::algo::GraphToDAG::~GraphToDAG() {}

/**
 * Converts a set of SubGraphs into DAGs
 * @param sg_list SubGraph list
 * @return list of DAGs in the same order as the SubGraph list
 */
std::unique_ptr<sbp::algo::GraphToDAG::DAG_List_t> sbp::algo::GraphToDAG::convertToDAG( const std::list<graph::SubGraph> &sg_list,
                                                                 const std::string &dag_name_prefix ) {
    _dag = std::make_unique<std::list<graph::DAG>>();
    size_t sg_count { 0 };
    for( auto it = sg_list.begin(); it != sg_list.end(); ++it ) {
        convertToDAG( *it, std::string( dag_name_prefix + std::to_string( sg_count ) ) );
        sg_count++;
    }
    return std::move( _dag );
}

/**
 * Converts a SubGraph into a DAG
 * @param sub_graph SubGraph to convert
 */
void sbp::algo::GraphToDAG::convertToDAG( const graph::SubGraph &sub_graph,
                                          const std::string &dag_name ) {
    auto dag = _dag->emplace( _dag->end(), graph::DAG( dag_name ) );
    dag->addNodes( sub_graph );

    //TODO
}