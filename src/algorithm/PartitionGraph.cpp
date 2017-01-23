#include "PartitionGraph.h"

/**
 * Constructor
 */
sbp::algo::PartitionGraph::PartitionGraph() {}

/**
 * Destructor
 */
sbp::algo::PartitionGraph::~PartitionGraph() {}

/**
 * Partitions a graph into SubGraphs based on a list of SCCs
 * @param base_graph     Base graph (global) from which to partition off
 * @param scc_lists      Complete list if SCCs (where first list is a concatenate of all singleton SCCs found)
 * @param sb_name_prefix SubGraph name prefix
 * @return List of all SubGraphs created
 */
std::unique_ptr<std::list<sbp::graph::SubGraph>> sbp::algo::PartitionGraph::partitionSCCs( const eadlib::WeightedGraph<size_t> &base_graph,
                                                                                           const std::list<std::list<size_t>> &scc_lists,
                                                                                           const std::string &sb_name_prefix ) {
    _sub_graphs = std::make_unique<SubGraphList_t>();
    size_t sg_count { 0 };
    auto it = scc_lists.begin();
    if( it != scc_lists.end() && !it->empty() ) { //Singleton SCCs
        partitionSingletonSCCs( base_graph, *it, std::string( sb_name_prefix + std::to_string( sg_count ) ) );
        sg_count++;
    }
    for( it = std::next( it ); it != scc_lists.end(); ++it) { //All the other SCCs
        partitionSCC( base_graph, *it, std::string( sb_name_prefix + std::to_string( sg_count ) ) );
        sg_count++;
    }
    return std::move( _sub_graphs );
}

/**
 * Partitions a graph into a SubGraph based on a non-singleton SCC
 * @param base_graph    Base graph (global) from which to partition off
 * @param scc           List of nodes of an SCC
 * @param subGraph_name Name of the created SubGraph
 * @return Iterator to the created SubGraph
 */
void sbp::algo::PartitionGraph::partitionSCC( const eadlib::WeightedGraph<size_t> &base_graph,
                                              const std::list<size_t> &scc,
                                              const std::string &subGraph_name ) {
    auto sub_graph = _sub_graphs->emplace( _sub_graphs->end(), subGraph_name );
    auto entrance_id = sub_graph->getSourceID();
    auto exit_id     = sub_graph->getTerminalID();
    //add node
    for( auto v : scc ) {
        sub_graph->addNode( v );
    }
    //add edges
    for( auto it = sub_graph->begin(); it != sub_graph->end(); ++it ) {
        if( !( it->first == entrance_id || it->first == exit_id ) ) {
            auto v = sub_graph->getGlobalID( it->first );
            if( !base_graph.at( v ).childrenList.empty() ) {
                for( auto u : base_graph.at( v ).childrenList ) {
                    auto local_u_search = sub_graph->findGlobalID( u );
                    if( local_u_search != sub_graph->end() ) {
                        sub_graph->createDirectedEdge( it->first, local_u_search->first );
                    } else { //edge leaving this SCC sub-graph
                        sub_graph->createDirectedEdge( it->first, exit_id ); //creates v->r'
                    }
                }
            }
            if( !base_graph.at( v ).parentsList.empty() ) {
                for( auto u : base_graph.at( v ).parentsList ) {
                    if( sub_graph->findGlobalID( u ) == sub_graph->end() ) { //not in sub-graph
                        sub_graph->createDirectedEdge( entrance_id, it->first ); //creates r->v
                    }
                }
            }
        }
    }
}

/**
 * Partitions a graph into a SubGraph based on a list of singleton SCCs
 * @param base_graph    Base graph (global) from which to partition off
 * @param scc           List of nodes of an SCC
 * @param subGraph_name Name of the created SubGraph
 * @return Iterator to the created SubGraph
 */
void sbp::algo::PartitionGraph::partitionSingletonSCCs( const eadlib::WeightedGraph<size_t> &base_graph,
                                                        const std::list<size_t> &scc,
                                                        const std::string &subGraph_name ) {
    auto sub_graph = _sub_graphs->emplace( _sub_graphs->begin(), subGraph_name );
    auto entrance_id = sub_graph->getSourceID();
    auto exit_id     = sub_graph->getTerminalID();
    //add node
    for( auto v : scc ) {
        sub_graph->addNode( v );
    }
    //add edges
    for( auto it = sub_graph->begin(); it != sub_graph->end(); ++it ) {
        if( !( it->first == entrance_id || it->first == exit_id ) ) {
            auto v = sub_graph->getGlobalID( it->first );
            if( base_graph.at( v ).childrenList.empty() ) {
                sub_graph->createDirectedEdge( it->first, exit_id ); //creates v->r'
            } else {
                for( auto u : base_graph.at( v ).childrenList ) {
                    auto local_u_search = sub_graph->findGlobalID( u );
                    if( local_u_search != sub_graph->end() ) {
                        sub_graph->createDirectedEdge( it->first, local_u_search->first );
                    } else { //edge leaving this SCC sub-graph
                        sub_graph->createDirectedEdge( it->first, exit_id ); //creates v->r'
                    }
                }
            }
            if( base_graph.at( v ).parentsList.empty() ) {
                sub_graph->createDirectedEdge( entrance_id, it->first ); //creates r->v
            } else {
                for( auto u : base_graph.at( v ).parentsList ) {
                    if( sub_graph->findGlobalID( u ) == sub_graph->end() ) { //not in sub-graph
                        sub_graph->createDirectedEdge( entrance_id, it->first ); //creates r->v
                    }
                }
            }
        }
    }
}




