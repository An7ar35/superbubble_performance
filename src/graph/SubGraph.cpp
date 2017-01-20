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
    _name( subGraph_name )
{
    //add nodes to sub-graph and maps
    _sub_graph.addNode( ( _entrance_node = 0 ) ); // r
    size_t local_id { 1 };
    for( auto v : scc ) {
        _sub_graph.addNode( local_id );
        std::cout << "Global: " << v << ", Local: " << local_id << std::endl;
        _local2global_map.emplace( std::make_pair( local_id, v ) );
        _global2local_map.emplace( std::make_pair( v, local_id ) );
        local_id++;
    }
    _sub_graph.addNode( ( _exit_node = local_id ) ); // r'
    //add edges
    for( auto it = _sub_graph.begin(); it != _sub_graph.end(); ++it ) {
        if( !( it->first == _entrance_node || it->first == _exit_node ) ) {
            auto v = _local2global_map.at( it->first );
            if( base_graph.at( v ).childrenList.empty() ) {
                _sub_graph.createDirectedEdge( it->first, _exit_node ); //creates v->r'
            } else {
                for( auto u : base_graph.at( v ).childrenList ) {
                    auto local_u_search = _global2local_map.find( u );
                    if( local_u_search != _global2local_map.end() ) {
                        _sub_graph.createDirectedEdge( it->first, local_u_search->second );
                    } else { //edge leaving this SCC sub-graph
                        _sub_graph.createDirectedEdge( it->first, _exit_node ); //creates v->r'
                    }
                }
            }
            if( base_graph.at( v ).parentsList.empty() ) {
                _sub_graph.createDirectedEdge( _entrance_node, it->first ); //creates r->v
            } else {
                for( auto u : base_graph.at( v ).parentsList ) {
                    if( _global2local_map.find( u ) == _global2local_map.end() ) { //not in sub-graph
                        _sub_graph.createDirectedEdge( _entrance_node, it->first ); //creates r->v
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
 * const iterator begin()
 * @return begin() iterator to the SubGraph's adjacency list
 */
sbp::graph::SubGraph::const_iterator sbp::graph::SubGraph::begin() const {
    return _sub_graph.begin();
}

/**
 * const iterator cend()
 * @return end() iterator to the SubGraph's adjacency list
 */
sbp::graph::SubGraph::const_iterator sbp::graph::SubGraph::end() const {
    return _sub_graph.end();
}

/**
 * Finds node in SubGraph
 * @param node Node to look up in the SubGraph
 * @return Const iterator to the Adjacency lists of the node
 */
sbp::graph::SubGraph::const_iterator sbp::graph::SubGraph::find( const size_t &node ) const {
    return _sub_graph.find( node );
}

/**
 * Gets the Node in the SubGraph (read-only)
 * @param node Node to look up in the SubGraph
 * @return Adjacency lists for the node
 * throws std::out_of_range when node specified is not in the SubGraph
 */
const eadlib::Graph<size_t>::NodeAdjacency &sbp::graph::SubGraph::at( const size_t &node ) const {
    try {
        return _sub_graph.at( node );
    } catch( std::out_of_range e ) {
        throw e;
    }
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
 * Gets the size of the sub-graph
 * @return Size of subgraph (number of edges)
 */
size_t sbp::graph::SubGraph::size() const {
    return _sub_graph.size();
}

/**
 * Gets the number of nodes in the sub-graph
 * @return Number of nodes
 */
size_t sbp::graph::SubGraph::nodeCount() const {
    return _sub_graph.nodeCount();
}

/**
 * Gets the name of the SubGraph
 * @return SubGraph name
 */
std::string sbp::graph::SubGraph::getName() const {
    return _name;
}

/**
 * Prints the SubGraph with local IDs
 * @param out Output stream
 * @return Output stream
 */
std::ostream &sbp::graph::SubGraph::printLocal( std::ostream &out ) const {
    return _sub_graph.printAdjacencyList( out );
}

/**
 * Prints the SubGraph with global IDs
 * @param out Output stream
 * @return Output stream
 */
std::ostream &sbp::graph::SubGraph::printGlobal( std::ostream &out ) const {
    for( auto it = _sub_graph.begin(); it != _sub_graph.end(); ++it ) {
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
        if( it != _sub_graph.end() ) out << "\n";
    }
    return out;
}
