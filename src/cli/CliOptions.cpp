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
 * Parses the arguments and fills the option container
 * @param argc Number of arguments
 * @param argv Arguments
 * @param option_container Container
 * @return Success
 */
bool sbp::cli::CliOptions::parse( const int &argc,
                                  char **argv,
                                  OptionContainer &option_container ) {
    if( _parser.parse( argc, argv ) ) {
        auto converter = eadlib::tool::Convert();
        //Input options
        auto val = _parser.getValues( "-f" );
        std::cout << val.size() << ", " << val.front().first << ", " << val.front().second << std::endl;
        if( !val.empty() && val.front().first ) {
            option_container.fasta_file = val.front().second;
        }
        val = _parser.getValues( "-k" );
        if( !val.empty() && val.front().first ) {
            option_container.kmer_size = converter.string_to_type<size_t>( val.front().second );
        }
        //Dot format output options
        option_container.d  = _parser.optionUsed( "-d" );
        option_container.dk = _parser.optionUsed( "-dk" );
        option_container.di = _parser.optionUsed( "-di" );
        //Database options
        val = _parser.getValues( "-s" );
        if( !val.empty() && val.front().first ) {
            option_container.save_flag  = true;
            option_container.graph_name = val.front().second;
        }
        val = _parser.getValues( "-r" );
        if( !val.empty() && val.front().first ) {
            option_container.remove_flag = true;
            option_container.graph_name = val.front().second;
        }
        option_container.list_flag = _parser.optionUsed( "-l" );
        //Superbubble algorithm options
        option_container.compress_flag = _parser.optionUsed( "-c" );
        option_container.sb1 = _parser.optionUsed( "-sb1" );
        option_container.sb2 = _parser.optionUsed( "-sb2" );
        option_container.sb3 = _parser.optionUsed( "-sb3" );
        return true;
    }
    return false;
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
    _parser.option( "Input", "-f", "-fasta", "Load FASTA file", true,
                   { { std::regex( "^(.*/)?(?:$|(.+?)(?:(\\.[^.]*$)|$))+" ), "Invalid file name.", "" } } );
    _parser.option( "Input", "-k", "-kmer", "K-mer length to use for graph construction.", true,
                   { { std::regex( "[0-9]+" ), "Invalid K-mer length.", "" } } );
    //Dot format output options
    _parser.option( "Dot File", "-d", "", "Export graph to dot format on the fly after each graph stages passed.", false, {} );
    _parser.option( "Dot File", "-dk", "", "Export graph to dot format with K-mers as nodes.", false,
                   { { std::regex( "(_?[0-9a-zA-Z]+_?)+" ), "Graph does not exists in DB.", "" } } );
    _parser.option( "Dot File", "-di", "", "Export graph to dot format with K-mers as nodes.", false,
                    { { std::regex( "(_?[0-9a-zA-Z]+_?)+" ), "Graph does not exists in DB.", "" } } );
    //Database options
    _parser.option( "Database", "-s", "", "Save graph in the database as name given.", false,
                    { { std::regex( "(_?[0-9a-zA-Z]+_?)+" ), "Graph name invalid.", "" } } );
    _parser.option( "Database", "-r", "", "Deletes specified graph from the database.", false,
                    { { std::regex( "(_?[0-9a-zA-Z]+_?)+" ), "Graph name invalid.", "" } } );
    _parser.option( "Database", "-l", "", "Lists all the graphs in the database.", false, {} );
    //Superbubble algorithm options
    _parser.option( "Algorithms", "-c",   "", "Compresses the K-mer graph.", false, {} );
    _parser.option( "Algorithms", "-sb1", "", "Uses Quasi-Linear time superbubble algorithm.", false, {} );
    _parser.option( "Algorithms", "-sb2", "", "Uses N Log N time superbubble algorithm.", false, {} );
    _parser.option( "Algorithms", "-sb3", "", "Uses Quadratic time superbubble algorithm.", false, {} );

    _parser.addExampleLine( "To pass your user name to the program: " + std::string( argv[ 0 ] ) + " -n myUserName01" );
}

