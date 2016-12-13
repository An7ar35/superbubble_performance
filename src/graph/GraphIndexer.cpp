#include "GraphIndexer.h"

/**
 * Constructor
 * @param db Database access instance
 */
sbp::graph::GraphIndexer::GraphIndexer( sbp::io::Database &db ) :
    _db( db )
{}

/**
 * Destructor
 */
sbp::graph::GraphIndexer::~GraphIndexer() {}

/**
 * Indexes and stores kmer string graph in database
 * @param graph_name Name of graph
 * @param graph      Kmer deBruijn Graph
 * @return Success
 */
bool sbp::graph::GraphIndexer::storeIntoDB( const std::string &graph_name, const eadlib::WeightedGraph<std::string> &graph ) {
    //Error control
    if( !_db.isOpen() ) {
        LOG_ERROR( "[sbp::graph::GraphIndexer::storeIntoDB( ", graph_name, ", <eadlib::WeightedGraph> )] Database not open." );
        return false;
    }
    if( _db.exists( graph_name ) ) {
        LOG_ERROR( "[sbp::graph::GraphIndexer::storeIntoDB( ", graph_name, ", <eadlib::WeightedGraph> )] Graph already exists." );
        return false;
    }
    if( !_db.create( graph_name ) ) {
        LOG_ERROR( "[sbp::graph::GraphIndexer::storeIntoDB( ", graph_name, ", <eadlib::WeightedGraph> )] Problem creating graph in DB." );
        return false;
    }
    std::string graph_ID = _db.getID( graph_name );
    if( graph_ID == "-1" ) {
        LOG_ERROR( "[sbp::graph::GraphIndexer::storeIntoDB( ", graph_name, ", <eadlib::WeightedGraph> )] Problem getting graph ID." );
        return false;
    }
    //Indexer
    std::cout << "-> DB: writing kmer indices." << std::endl;
    std::unordered_map<std::string, size_t> kmer_index;
    size_t i { 0 };
    auto index_progress = eadlib::cli::ProgressBar( graph.nodeCount(), 70 );
    _db.beginTransaction();
    for( auto node : graph ) {
        kmer_index.insert( typename std::unordered_map<std::string, size_t>::value_type( node.first, i ) );
        _db.writeNode( graph_ID, i, node.first );
        i++;
        ( index_progress++ ).printPercentBar( std::cout, 2 );
    }
    _db.commitTransaction();
    std::cout << index_progress.complete() << std::endl;
    //Indexed graph
    std::cout << "-> DB: writing graph edges." << std::endl;
    auto edge_progress = eadlib::cli::ProgressBar( graph.size(), 70 );
    _db.beginTransaction();
    for( auto node : graph ) {
        edge_progress.printPercentBar( std::cout, 2 );
        size_t origin_node = kmer_index.at( node.first );
        if( !node.second.childrenList.empty() ) {
            for( auto dest : node.second.childrenList ) {
                size_t destination_node = kmer_index.at( dest );
                _db.writeEdge( graph_ID, origin_node, destination_node, node.second.weight.at( dest ) );
                edge_progress += node.second.weight.at( dest );
                edge_progress.printPercentBar( std::cout, 2 );
            }
        }
    }
    _db.commitTransaction();
    std::cout << edge_progress.complete() << std::endl;
    return true;
}