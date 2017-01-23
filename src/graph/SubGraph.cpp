#include "SubGraph.h"

/**
 * Constructor
 * @param base_graph    Master graph from which the subGraph is derived
 * @param scc           Strongly connected component set to build the sub graph with
 * @param subGraph_name Name of the sub-graph
 */
sbp::graph::SubGraph::SubGraph( const eadlib::WeightedGraph<size_t> &base_graph,
                                const std::list<size_t> &scc,
                                const std::string &subGraph_name  ) :
    eadlib::Graph<size_t>( subGraph_name )
{
    //add nodes to sub-graph and maps
    addNode( ( _entrance_node = 0 ) ); // r
    size_t local_id { 1 };
    for( auto v : scc ) {
        addNode( local_id );
        _local2global_map.emplace( std::make_pair( local_id, v ) );
        _global2local_map.emplace( std::make_pair( v, local_id ) );
        local_id++;
    }
    addNode( ( _exit_node = local_id ) ); // r'
    //add edges
    for( auto it = begin(); it != end(); ++it ) {
        if( !( it->first == _entrance_node || it->first == _exit_node ) ) {
            auto v = _local2global_map.at( it->first );
            if( base_graph.at( v ).childrenList.empty() ) {
                createDirectedEdge( it->first, _exit_node ); //creates v->r'
            } else {
                for( auto u : base_graph.at( v ).childrenList ) {
                    auto local_u_search = _global2local_map.find( u );
                    if( local_u_search != _global2local_map.end() ) {
                        createDirectedEdge( it->first, local_u_search->second );
                    } else { //edge leaving this SCC sub-graph
                        createDirectedEdge( it->first, _exit_node ); //creates v->r'
                    }
                }
            }
            if( base_graph.at( v ).parentsList.empty() ) {
                createDirectedEdge( _entrance_node, it->first ); //creates r->v
            } else {
                for( auto u : base_graph.at( v ).parentsList ) {
                    if( _global2local_map.find( u ) == _global2local_map.end() ) { //not in sub-graph
                        createDirectedEdge( _entrance_node, it->first ); //creates r->v
                    }
                }
            }
        }
    }
}

/**
 * Destructor
 */
sbp::graph::SubGraph::~SubGraph() {}

/**
 * Gets the local source node ID of the subgraph (r)
 * @return Source ID r
 */
size_t sbp::graph::SubGraph::getSourceID() const {
    return _entrance_node;
}

/**
 * Gets the local terminal node ID of the subgraph (r')
 * @return Terminal ID r'
 */
size_t sbp::graph::SubGraph::getTerminalID() const {
    return _exit_node;
}

/**
 * Gets the global ID for the local one
 * @param local Local ID
 * @return Global ID
 * @throws std::out_of_range when local id passed is not in global graph (r/r'/invalid node)
 */
size_t sbp::graph::SubGraph::getGlobalID( const size_t local ) const {
    try {
        return _local2global_map.at( local );
    } catch( std::out_of_range e ) {
        throw e;
    }
}

/**
 * Prints the SubGraph with local IDs
 * @param out Output stream
 * @return Output stream
 */
std::ostream &sbp::graph::SubGraph::printLocal( std::ostream &out ) const {
    return printAdjacencyList( out );
}

/**
 * Prints the SubGraph with global IDs
 * @param out Output stream
 * @return Output stream
 */
std::ostream &sbp::graph::SubGraph::printGlobal( std::ostream &out ) const {
    for( auto it = begin(); it != end(); ++it ) {
        if( it->first == _entrance_node ) {
            out << "[r] -> ";
        } else if( it->first == _exit_node ) {
            out << "[r'] -> ";
        } else {
            out << "[" << _local2global_map.at( it->first ) << "] -> ";
        }
        for( auto child : it->second.childrenList ) {
            if( child == _entrance_node ) {
                out << "[r] ";
            } else if( child == _exit_node ) {
                out << "[r'] ";
            } else {
                out << "[" << _local2global_map.at( child ) << "] ";
            }
        }
        if( it != end() ) out << "\n";
    }
    return out;
}
