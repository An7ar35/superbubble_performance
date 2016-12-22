#include "PipelineRunner.h"

/**
 * Loads a FASTA file and constructs a deBruijn graph from it
 * @param fasta_file_path Path and name of the FASTA file with the sequencer reads inside
 * @param kmer_size       Number of characters of a K-mer
 * @param graph           Graph instance to load into
 */
void sbp::PipelineRunner::loadFASTA( const std::string &fasta_file_path, const size_t &kmer_size, eadlib::WeightedGraph<std::string> &graph ) {
    auto reader = eadlib::io::FileReader( fasta_file_path );
    auto parser = sbp::io::FastaParser( reader );
    auto graph_constructor = sbp::graph::GraphConstructor( graph, kmer_size );
    std::cout << "-> Parsing file '" << reader.getFileName() << "' into K-mer graph." << std::endl;
    typedef sbp::io::FastaParserState ParseState_t;
    std::vector<char> buffer;
    size_t sequence_count { 0 };
    bool   parser_done { false };
    do {
        ParseState_t state = parser.parse( buffer );
        switch( state ) {
            case ParseState_t::PARSER_ERROR:
                std::cout << "Parser fault occurred." << std::endl;
                parser_done = true;
                break;
            case ParseState_t::FILE_ERROR:
                std::cout << "File error occurred." << std::endl;
                parser_done = true;
                break;
            case ParseState_t::DESC_PARSED:
                //print( buffer );
                break;
            case ParseState_t::READ_PARSED:
                sequence_count++;
                //std::cout << "Read=";
                //print( buffer );
                graph_constructor.addToGraph( buffer );
                break;
            case ParseState_t::EOF_REACHED:
                parser_done = true;
                break;
        }
    } while( !parser_done );

    std::cout << "-> Result: " << sequence_count << " reads parsed." << std::endl;
    std::cout << "           " << graph_constructor.getKmerCount() << " k-mers of length " << kmer_size << " processed." << std::endl;
    std::cout << "           " << graph.nodeCount() << " nodes in graph." << std::endl;
    std::cout << "           " << graph.size() << " edges in graph." << std::endl;
}

/**
 * Compresses the graph
 * @param graph Graph instance to compress
 */
void sbp::PipelineRunner::compressGraph( eadlib::WeightedGraph<std::string> &graph ) {
    std::cout << "-> Compressing graph..." << std::endl;
    auto compressor = sbp::algo::GraphCompressor( graph );
    compressor.compress();
    std::cout << "-> Result: " << graph.nodeCount() << " nodes in graph." << std::endl;
    std::cout << "           " << graph.size() << " edges in graph." << std::endl;
}

/**
 * Exports a graph to a Dot file format
 * @param file_name File name of the dot file
 * @param graph     Graph instance
 */
void sbp::PipelineRunner::exportToDot( const std::string &file_name, eadlib::WeightedGraph<std::string> &graph ) {
    std::cout << "-> Saving graph to Dot file format: " << file_name << std::endl;
    auto writer = eadlib::io::FileWriter( file_name );
    auto dot_writer = sbp::io::DotExport<std::string>( writer );
    writer.open( true );
    dot_writer.exportToDot( graph.getName(), graph, false );
}

/**
 * Exports a graph to a Dot file format
 * @param file_name File name of the dot file
 * @param graph     Graph instance
 */
void sbp::PipelineRunner::exportToDot( const std::string &file_name, eadlib::WeightedGraph<size_t> &graph ) {
    std::cout << "-> Saving graph to Dot file format: " << file_name << std::endl;
    auto writer = eadlib::io::FileWriter( file_name );
    auto dot_writer = sbp::io::DotExport<size_t>( writer );
    writer.open( true );
    dot_writer.exportToDot( graph.getName(), graph, false );
}

/**
 * Indexes and saves K-mer graph into Database
 * @param db_file_name Database file name
 * @param graph        Graph instance
 */
void sbp::PipelineRunner::exportToDB( const std::string &db_file_name, eadlib::WeightedGraph<std::string> &graph ) {
    auto db = sbp::io::Database();
    if( db.open( db_file_name ) ) {
        std::cout << "-> Storing into database..." << std::endl;
        auto graph_indexer = sbp::graph::GraphIndexer( db );
        graph_indexer.storeIntoDB( graph.getName(), graph );
        db.close();
    }
}

/**
 * Loads up an indexed version of the graph from the database
 * @param db_file_name Database file name
 * @param graph        Graph instance
 */
void sbp::PipelineRunner::importFromDB( const std::string &db_file_name, eadlib::WeightedGraph<size_t> &graph ) {
    auto db = sbp::io::Database();
    if( db.open( db_file_name ) ) {
        std::cout << "-> Loading indexed graph..." << std::endl;
        db.loadGraph( graph.getName(), graph );
        db.close();
        std::cout << "-> Result: " << graph.nodeCount() << " nodes in graph." << std::endl;
        std::cout << "           " << graph.size() << " edges in graph." << std::endl;
    }
}

/**
 * Loads up a kmer string version of the graph from the database
 * @param db_file_name Database file name
 * @param graph        Graph instance
 */
void sbp::PipelineRunner::importFromDB( const std::string &db_file_name, eadlib::WeightedGraph<std::string> &graph ) {
    auto db = sbp::io::Database();
    if( db.open( db_file_name ) ) {
        std::cout << "-> Loading kmer graph..." << std::endl;
        db.loadGraph( graph.getName(), graph );
        db.close();
        std::cout << "-> Result: " << graph.nodeCount() << " nodes in graph." << std::endl;
        std::cout << "           " << graph.size() << " edges in graph." << std::endl;
    }
}

/**
 * Runs the superbubble algorithms on the graph
 * @param graph Graph instance
 */
void sbp::PipelineRunner::runSuperbubble( const eadlib::WeightedGraph<size_t> &graph ) {
    auto writer = eadlib::io::FileWriter( "benchmarks.txt" );
    auto sb     = sbp::algo::SB_Driver( writer );
    auto result = std::list<sbp::algo::container::SuperBubble>();
    sb.runLinear( graph, result );
}
