#include "eadlib/cli/graphic/ProgressBar.h"

#include "io/FastaParser.h"
#include "io/DotExport.h"
#include "graph/GraphConstructor.h"
#include "algorithm/GraphCompressor.h"

void print( std::vector<char> &buffer ) {
    for( auto e : buffer ) {
        std::cout << e;
    }
    std::cout << std::endl;
}

int main() {
    std::string file_path = "../data/test01.fasta";
    auto reader = eadlib::io::FileReader( file_path );
    auto parser = sbp::io::FastaParser( reader );
    auto graph  = eadlib::WeightedGraph<std::string>();
    auto kmer   = 5;
    auto graph_constructor = sbp::graph::GraphConstructor( graph, kmer );
    auto writer = eadlib::io::FileWriter( "test01.dot" );
    auto dot_writer = sbp::io::DotExport( writer );

    std::cout << "-> Parsing file '" << reader.getFileName() << "' into graph." << std::endl;
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

    std::cout << "-> " << sequence_count << " reads parsed." << std::endl;
    std::cout << "-> " << graph_constructor.getKmerCount() << " k-mers of length " << kmer << " processed." << std::endl;
    std::cout << "-> " << graph.nodeCount() << " nodes in graph." << std::endl;
    std::cout << "-> " << graph.size() << " edges in graph." << std::endl;

    writer.open( true );
    dot_writer.exportToDot( "deBruijn", graph, false );

    std::cout << "-> Compressing graph..." << std::endl;
    auto compressor = sbp::algo::GraphCompressor( graph );
    compressor.compress();
    std::cout << "-> " << graph.nodeCount() << " nodes in graph." << std::endl;
    std::cout << "-> " << graph.size() << " edges in graph." << std::endl;


    std::cout << "-> Writing final dot file..." << std::endl;
    auto writer2 = eadlib::io::FileWriter( "test01c.dot" );
    writer2.open( true );
    auto dot_writer2 = sbp::io::DotExport( writer2 );
    dot_writer2.exportToDot( "compressed", graph, false );
    return 0;
}