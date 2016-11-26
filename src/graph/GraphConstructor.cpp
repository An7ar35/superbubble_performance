#include "GraphConstructor.h"

/**
 * Constructor
 * @param graph       Weighted Digraph
 * @param kmer_length Length of the k-mers
 */
sbp::graph::GraphConstructor::GraphConstructor( eadlib::WeightedGraph<std::string> &graph, const size_t &kmer_length ) :
    _graph( graph ),
    _kmer_length( kmer_length ),
    _kmer_processed( 0 ),
    _read_processed( 0 )
{}

/**
 * Destructor
 */
sbp::graph::GraphConstructor::~GraphConstructor() {}

/**
 * Processes a read into the graph
 * @param read Sequencer read
 * @return Success
 */
bool sbp::graph::GraphConstructor::addToGraph( std::vector<char> &read ) {
    if( _kmer_length < 2 ) {
        LOG_ERROR( "[sbp::graph::GraphConstructor::addToGraph(..)] k-mer length too small (", _kmer_length, ")." );
        return false;
    }
    if( read.size() < 3 ) {
        LOG_ERROR( "[sbp::graph::GraphConstructor::addToGraph(..)] Read size too small (", read.size(),")." );
        return false;
    }
    if( _kmer_length >= read.size() ) {
        LOG_ERROR( "[sbp::graph::GraphConstructor::addToGraph(..)] k-mer length (", _kmer_length, ") too big for read (", read.size(),")." );
        return false;
    }
    //Breaking down read into n k-mers
    std::vector<std::string> kmer_store;
    size_t last_kmer_index = read.size() - _kmer_length;
    size_t index { 0 };
    do {
        kmer_store.emplace_back( std::string ( &read[index], _kmer_length ) );
        index++;
    } while( index <= last_kmer_index );
    _read_processed++;

    //Adding k-mers to graph
    try {
        for( auto it = kmer_store.begin(); it != kmer_store.end(); ++it ) {
            auto next = std::next( it, 1 );
            if( next != kmer_store.end() ) {
                if( !_graph.createDirectedEdge_fast( *it, *next ) ) {
                    LOG_ERROR( "[sbp::graph::GraphConstructor::addToGraph(..)] Problem adding edge '", *it, "'->'", *next, "'." );
                    return false;
                }
                _kmer_processed++;
            }
        }
        return true;
    } catch( std::overflow_error ) {
        LOG_FATAL( "[sbp::graph::GraphConstructor::addToGraph(..)] Graph has hit size limit (", _graph.size(), ")." );
        return false;
    }
}

/**
 * Gets the number of k-mers processed
 * @return Total k-mers processed
 */
uint64_t sbp::graph::GraphConstructor::getKmerCount() {
    return _kmer_processed;
}

/**
 * Gets the number of reads processed
 * @return Total reads processed
 */
uint64_t sbp::graph::GraphConstructor::getReadCount() {
    return _read_processed;
}


