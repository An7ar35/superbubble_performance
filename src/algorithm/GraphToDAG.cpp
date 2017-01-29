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
    _dag_package_list = std::make_unique<std::list<sbp::algo::GraphToDAG::DAG_Package>>();
    size_t sg_count { 0 };
    for( auto it = sg_list.begin(); it != sg_list.end(); ++it ) {
        convertToDAG( *it, std::string( dag_name_prefix + std::to_string( sg_count ) ) );
        sg_count++;
    }
    return std::move( _dag_package_list );
}

/**
 * DFS visit of nodes in SubGraph to create edges
 * @param sub_graph SubGraph to visit in
 * @param u         Node to visit
 * @param colour    Node colours for the SubGraph
 * @param time      Reach time
 * @param dag_pack  Package holding the DAG, discovery times and finish times
 */
void sbp::algo::GraphToDAG::visitUsingDFS( const sbp::graph::SubGraph &sub_graph,
                                           const size_t &u,
                                           std::vector<sbp::algo::GraphToDAG::DFSColours> &colour,
                                           size_t time,
                                           DAG_Package &dag_pack ) {

    /**
     * [Lambda] Check that u/v are source or terminal nodes
     */
    auto notSourceOrTerminal = [&]( const size_t &u, const size_t &v ) {
        return ( u != sub_graph.getSourceID() && u != sub_graph.getTerminalID()
                 && v != sub_graph.getSourceID() && v != sub_graph.getTerminalID() );
    };

    auto dag       = &dag_pack._dag;
    auto discovery = &dag_pack._discovery_times;
    auto finish    = &dag_pack._finish_times;

    colour.at( u ) = DFSColours::GREY;
    discovery->at( u ) = ++time;
    if( !sub_graph.at( u ).childrenList.empty() ) {
        auto node = sub_graph.at( u );
        for( auto v : node.childrenList ) {
            switch( colour.at( v ) ) {
                case DFSColours::WHITE: //u->v is a tree edge
                    if( notSourceOrTerminal( u, v ) ) {
                        auto u_duplicate = u + dag->getUniqueNodeCount() - 2;
                        auto v_duplicate = v + dag->getUniqueNodeCount() - 2;
                        dag->createDirectedEdge( u, v );
                        dag->createDirectedEdge( u_duplicate, v_duplicate );
                    }
                    visitUsingDFS( sub_graph, v, colour, time, dag_pack );
                    break;
                case DFSColours::GREY: //u->v is a back edge
                    if( notSourceOrTerminal( u, v ) ) {
                        auto v_duplicate = dag->findLocalID( v + dag->getUniqueNodeCount() - 2 )->first;
                        dag->createDirectedEdge( u, v_duplicate );
                    }
                    break;
                case DFSColours::BLACK: //u->v is either a forward or cross edge
                    if( notSourceOrTerminal( u, v ) ) {
                        auto u_duplicate = dag->findLocalID( u + dag->getUniqueNodeCount() - 2 )->first;
                        auto v_duplicate = dag->findLocalID( v + dag->getUniqueNodeCount() - 2 )->first;
                        dag->createDirectedEdge( u, v );
                        dag->createDirectedEdge( u_duplicate, v_duplicate );
                    }
                    break;
            }
        }
    }
    colour.at( u ) = DFSColours::BLACK;
    finish->at( u ) = ++time;
}

/**
 * Converts a SubGraph into a DAG
 * @param sub_graph SubGraph to convert
 */
void sbp::algo::GraphToDAG::convertToDAG( const graph::SubGraph &sub_graph,
                                          const std::string &dag_name ) {
    auto dag_pack = _dag_package_list->emplace( _dag_package_list->end(), DAG_Package( dag_name, sub_graph.nodeCount() ) );
    auto dag = &dag_pack->_dag;
    dag->addNodes( sub_graph );
    //create r->v in DAG
    auto sg_source   = sub_graph.at( sub_graph.getSourceID() );
    for( auto child : sg_source.childrenList ) { //create dag_source->v
        if( child != sub_graph.getTerminalID() ) {
            dag->createDirectedEdge( dag->getSourceID(), child );
        }
    }

    //create v'<-r' in DAG
    auto sg_terminal = sub_graph.at( sub_graph.getTerminalID() );
    for( auto parent : sg_terminal.parentsList ) { //create v<-dag_terminal
        if( parent != sub_graph.getSourceID() ) {
            auto dag_duplicate = dag->findLocalID( parent + dag->getUniqueNodeCount() - 2 );
            if( dag_duplicate != dag->end() ) {
                dag->createDirectedEdge( dag_duplicate->first, dag->getTerminalID() );
            } else {
                LOG_ERROR( "[sbp::algo::GraphToDAG::convertToDAG( <graph::SubGraph>, ", dag_name, ")] "
                    "Problem finding the duplicate ID [", dag_duplicate->first ,"] of local node [", parent, "] in the DAG." );
            }
        }
    }

    //if no out-degree from r, choose random v as source/root (not really random but does the trick..)
    auto root = sg_source.childrenList.empty() ? 3 : sub_graph.getSourceID();
    auto sg_colours = std::vector<DFSColours>( sub_graph.size(), DFSColours::WHITE );
    visitUsingDFS( sub_graph, root, sg_colours, 0, *dag_pack );

    //adjust source and terminal vertices
    if( sg_source.childrenList.empty() ) { //G does not contain r
        for( auto it = std::next( dag->begin(), 2 ); it != dag->end(); ++it ) {
            if( it->second.parentsList.empty() ) { //in-degree == 0
                //for every u ∈ V (G ) such that u has no incoming edge in G' create an edge (r, u)
                dag->createDirectedEdge( dag->getSourceID(), it->first );
            }
        }
    }
    if( sg_terminal.parentsList.empty() ) { //G does not contain r'
        for( auto it = std::next( dag->begin(), 2 ); it != dag->end(); ++it ) {
            if( it->second.childrenList.empty() ) { //out-degree == 0
                //for every u ∈ V (G ) such that u has no outgoing edge in G' create an edge (u, r')
                dag->createDirectedEdge( it->first, dag->getTerminalID() );
            }
        }
    }
}
