#include "SB_Linear.h"

/**
 * Constructor
 * @param graph Graph on which to detect superbubbles
 */
sbp::algo::SB_Linear::SB_Linear( const eadlib::WeightedGraph<size_t> &graph ) :
    _graph( graph )
{}

/**
 * Destructor
 */
sbp::algo::SB_Linear::~SB_Linear() {}

/**
 * Run the Linear SuperBubble identification algorithm
 * @param superbubble_list SuperBubble list container to fill
 * @return Success
 */
bool sbp::algo::SB_Linear::run( std::list<container::SuperBubble> &superbubble_list ) {
    //Find SCCs
    auto found_SCCs = Tarjan( _graph ).findSCCs();

    //Partition graph into sub-graphs
    auto sub_graphs = PartitionGraph().partitionSCCs( _graph, *found_SCCs, "SubGraph" );
    found_SCCs.reset(); //no longer needed so early destruction to free up memory

    //Convert SubGraphs into DAG
    auto dag_packages = sbp::algo::GraphToDAG().convertToDAG( *sub_graphs, "DAG" );

    for( auto it = dag_packages->begin(); it != dag_packages->end(); ++it ) {
        std::vector<size_t> invOrd;
        std::vector<size_t> ordD( it->_dag.nodeCount() );
        fillTopologicalOrder( it->_dag, invOrd, ordD );

        std::cout << "invOrd: ";
        for( auto e : invOrd ) {
            std::cout << e << " ";
        }
        std::cout << std::endl;

        std::cout << "ordD: ";
        for( auto e : ordD ) {
            std::cout << e << " ";
        }
        std::cout << std::endl;

        auto candidate_list = std::list<std::shared_ptr<sbp::algo::SB_Linear::Candidate>>();
        auto pvsEntrance    = std::vector<std::shared_ptr<sbp::algo::SB_Linear::Candidate>>( it->_dag.nodeCount() );
        generateCandidateList( it->_dag, invOrd, candidate_list, pvsEntrance );
    }

    //TODO
    return 0;
}

/**
 * Fills a vector with the node IDs of a DAG in topological order
 * @param dag    DAG
 * @param invOrd Container for node IDs in topological order
 * @param ordD //TODO
 */
void sbp::algo::SB_Linear::fillTopologicalOrder( const sbp::graph::DAG &dag,
                                                 std::vector<size_t> &invOrd,
                                                 std::vector<size_t> &ordD ) {
    auto order_stack = std::stack<size_t>();
    auto visited     = std::vector<bool>( dag.nodeCount(), false );

    topologicalSort( dag, dag.getSourceID(), visited, order_stack );

    size_t order { 0 };
    while( !order_stack.empty() ) {
        invOrd.emplace_back( order_stack.top() );
        ordD.at( order_stack.top() ) = order++;
        order_stack.pop();
    }
}

/**
 * Sorts nodes in a DAG topologically
 * @param dag         DAG
 * @param v           Node (vertex)
 * @param visited     Visitation flags of nodes
 * @param order_stack Topological order stack
 */
void sbp::algo::SB_Linear::topologicalSort( const sbp::graph::DAG &dag,
                                            const size_t &v,
                                            std::vector<bool> &visited,
                                            std::stack<size_t> &order_stack ) {
    visited.at( v ) = true;
    auto node = dag.at( v );

    for( auto child : node.childrenList ) {
        if( !visited.at( child ) ) {
            topologicalSort( dag, child, visited, order_stack );
        }
    }

    order_stack.push( v );
}

/**
 * Generates candidate list
 * @param dag            DAG
 * @param invOrd         Container for node IDs in topological order
 * @param candidate_list Candidate list container
 * @param pvsEntrance    Previous entrances for each node ID
 */
void sbp::algo::SB_Linear::generateCandidateList( const sbp::graph::DAG &dag,
                                                  const std::vector<size_t> &invOrd,
                                                  std::list<std::shared_ptr<sbp::algo::SB_Linear::Candidate>> &candidate_list,
                                                  std::vector<std::shared_ptr<sbp::algo::SB_Linear::Candidate>> &pvsEntrance ) {

    std::shared_ptr<Candidate> pvs_entrance_ptr;
    for( auto order : invOrd ) {
        bool exit_flag { false };
        bool entrance_flag { false };
        auto node = dag.at( order );

        //exit candidate
        for( auto it = node.parentsList.begin(); it != node.parentsList.end() && !exit_flag; ++it ) {
            if( dag.getOutDegree( *it ) == 1 ) {
                candidate_list.emplace_back( std::make_shared<Candidate>( order, false, pvs_entrance_ptr ) );
                exit_flag = true;
            }
        }
        //entrance candidate
        for( auto it = node.childrenList.begin(); it != node.childrenList.end() && !entrance_flag; ++it ) {
            if( dag.getInDegree( *it ) == 1 ) {
                pvs_entrance_ptr = std::make_shared<Candidate>( order, true );
                candidate_list.emplace_back( pvs_entrance_ptr );
                entrance_flag = true;
            }
        }

        pvsEntrance.at( order ) = pvs_entrance_ptr;
    }
}

