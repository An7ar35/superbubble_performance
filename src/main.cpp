#include "io/FastaParser.h"
#include "graph/GraphConstructor.h"
#include "io/DotExport.h"

void print( std::vector<char> &buffer ) {
    for( auto e : buffer ) {
        std::cout << e;
    }
    std::cout << std::endl;
}

int main() {
    std::string file_path = "../data/genome_01.fasta";
    auto reader = eadlib::io::FileReader( file_path );
    auto parser = sbp::io::FastaParser( reader );
    auto graph  = eadlib::WeightedGraph<std::string>();
    auto graph_constructor = sbp::graph::GraphConstructor( graph, 3 );
    auto writer = eadlib::io::FileWriter( "graph01.dot" );
    auto dot_writer = sbp::io::DotExport( writer );

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
                print( buffer );
                break;
            case ParseState_t::READ_PARSED:
                sequence_count++;
                std::cout << "Read=";
                print( buffer );
                graph_constructor.addToGraph( buffer );
                break;
            case ParseState_t::EOF_REACHED:
                parser_done = true;
                break;
        }
    } while( !parser_done );

    std::cout << "Sequences parsed: " << sequence_count << std::endl;
    std::cout << "Read processed  : " << graph_constructor.getReadCount() << std::endl;
    std::cout << "K-mers processed: " << graph_constructor.getKmerCount() << std::endl;
    graph.printStats( std::cout );
    writer.open( true );
    dot_writer.exportToDot( graph );
    //graph.printGraphNodes( std::cout );
    return 0;
}