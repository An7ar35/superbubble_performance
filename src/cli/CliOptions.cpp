#include "CliOptions.h"

/**
 * Constructor
 * @param parser Parser instance
 */
sbp::cli::CliOptions::CliOptions( eadlib::cli::Parser &parser, char **argv ) :
    _parser( parser )
{
    init( argv );
}

/**
 *
 * @param argc
 * @param argv
 * @return
 */
bool sbp::cli::CliOptions::parse( const int &argc, char **argv ) {
    return _parser.parse( argc, argv );
}

/**
 * Sets up the cli options available
 * @param argv
 */
void sbp::cli::CliOptions::init( char **argv ) {
    //TODO sort out regex for options
    _parser.addDescriptionLine( "Simplified deBruijn graph construction and\n"
                               "superbubble identification pipeline for benchmarking." );
    //Input options
    _parser.option( "Input", "-f", "-fasta", "Load FASTA file", false,
                   { { std::regex( "^(.*/)?(?:$|(.+?)(?:(\\.[^.]*$)|$))+" ), "Invalid file name.", "" } } );
    _parser.option( "Input", "-k", "-kmer", "K-mer length to use for graph construction.", false,
                   { { std::regex( "[0-9]+" ), "Invalid K-mer length.", "" } } );
    //Dot format output options
    _parser.option( "Dot File", "-d1", "", "Export 1st stage graph (fasta) to dot format.", false, {} );
    _parser.option( "Dot File", "-d2", "", "Export 2nd stage graph (compressed) to dot format.", false, {} );
    _parser.option( "Dot File", "-d3", "", "Export 3rd stage graph (indexed) to dot format.", false, {} );
    _parser.option( "Dot File", "-dk", "", "Export a stored database graph to dot format with Kmers as nodes.", false,
                   { { std::regex( "[0-9a-zA-Z]+" ), "Graph does not exists in DB.", "" } } );
    _parser.option( "Dot File", "-di", "", "Export a stored database graph to dot format with Indices as nodes.", false,
                   { { std::regex( "[0-9a-zA-Z]+" ), "Graph does not exists in DB.", "" } } );
    //Database options
    _parser.option( "Database", "-s", "", "Save graph in the database.", false, {} );
    _parser.option( "Database", "-d", "", "Database file name.", false,
                   { { std::regex( "^(.*/)?(?:$|(.+?)(?:(\\.[^.]*$)|$))+" ), "Invalid file name.", "graphs.db" } } );
    _parser.option( "Database", "-l", "", "Lists all the graphs in the database.", false, {} );
    //Superbubble algorithm options
    _parser.option( "Algorithms", "-c",   "", "Compresses the K-mer graph.", false, {} );
    _parser.option( "Algorithms", "-sb1", "", "Uses Quasi-Linear time superbubble algorithm.", false, {} );
    _parser.option( "Algorithms", "-sb2", "", "Uses N Log N time superbubble algorithm.", false, {} );
    _parser.option( "Algorithms", "-sb3", "", "Uses Quadratic time superbubble algorithm.", false, {} );


    _parser.addExampleLine( "To pass your user name to the program: " + std::string( argv[ 0 ] ) + " -n myUserName01" );
}


