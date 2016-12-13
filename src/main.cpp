#include "eadlib/cli/graphic/ProgressBar.h"

#include "io/Database.h"
#include "io/FastaParser.h"
#include "io/DotExport.h"
#include "graph/GraphConstructor.h"
#include "graph/GraphIndexer.h"
#include "algorithm/GraphCompressor.h"

void print( std::vector<char> &buffer ) {
    for( auto e : buffer ) {
        std::cout << e;
    }
    std::cout << std::endl;
}

int main() {
    std::string graph_name = "test01"; //"genome_01"; //"test01";
    std::string file_path = "../data/" + graph_name + ".fasta";
    std::string dot_file = graph_name + ".dot";
    std::string compressed_dot_file = graph_name + "c.dot";
    std::string indexed_dot_file = graph_name + "i.dot";


    auto reader = eadlib::io::FileReader( file_path );
    auto parser = sbp::io::FastaParser( reader );
    auto graph  = eadlib::WeightedGraph<std::string>();
    auto kmer   = 5; //K-mer string size
    auto graph_constructor = sbp::graph::GraphConstructor( graph, kmer );
    auto writer = eadlib::io::FileWriter( dot_file );
    auto dot_writer = sbp::io::DotExport( writer );

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
    std::cout << "           " << graph_constructor.getKmerCount() << " k-mers of length " << kmer << " processed." << std::endl;
    std::cout << "           " << graph.nodeCount() << " nodes in graph." << std::endl;
    std::cout << "           " << graph.size() << " edges in graph." << std::endl;

    auto db = sbp::io::Database();
    if( db.open( "graphs.db" ) ) {
        writer.open( true );
        dot_writer.exportToDot<std::string>( "deBruijn", graph, false );

        std::cout << "-> Compressing graph..." << std::endl;
        auto compressor = sbp::algo::GraphCompressor( graph );
        compressor.compress();
        std::cout << "-> Result: " << graph.nodeCount() << " nodes in graph." << std::endl;
        std::cout << "           " << graph.size() << " edges in graph." << std::endl;

        std::cout << "-> Storing into database..." << std::endl;
        auto graph_indexer = sbp::graph::GraphIndexer( db );
        graph_indexer.storeIntoDB( graph_name, graph );

        std::cout << "-> Writing kmer dot file..." << std::endl;
        auto writer2 = eadlib::io::FileWriter( compressed_dot_file );
        writer2.open( true );
        auto dot_writer2 = sbp::io::DotExport( writer2 );
        dot_writer2.exportToDot<std::string>( "compressed", graph, false );

        std::cout << "-> Loading indexed graph..." << std::endl;
        auto indexed_graph = eadlib::WeightedGraph<size_t>();
        db.loadGraph( graph_name, indexed_graph );

        std::cout << "-> Writing indexed dot file..." << std::endl;
        auto writer3 = eadlib::io::FileWriter( indexed_dot_file );
        writer3.open( true );
        auto dot_writer3 = sbp::io::DotExport( writer3 );
        dot_writer3.exportToDot<size_t>( "indexed", indexed_graph, false );

        db.close();
    }
    return 0;
}