#include "PipelineRunner.h"
#include "cli/CliOptions.h"

namespace sbp {
    std::string fileNameExtractor( const std::string &file_path );
}

int main( int argc, char *argv[] ) {
    std::string program_name { "###########################\n"
                               "# Superbubble Performance #\n"
                               "###########################" };
    auto parser      = eadlib::cli::Parser( program_name );
    auto cli_options = sbp::cli::CliOptions( parser, argv );
    auto options     = sbp::cli::OptionContainer();
    try {
        if( cli_options.parse( argc, argv, options ) ) {
            //Option: List graphs in DB
            if( options.list_flag ) {
                auto db = sbp::io::Database();
                db.open( options.db_name );
                db.listGraphs( std::cout );
                return 0;
            }
            //Option: Delete graph from DB
            if( options.remove_flag ) {
                if( options.save_flag ) {
                    std::cerr << "Error: Cannot both save and remove a graph. Please do one at a time." << std::endl;
                    return -1;
                }
                auto db = sbp::io::Database();
                db.open( options.db_name );
                if( !db.remove( options.graph_name ) ) {
                    std::cerr << "Error: Could not delete '" << options.graph_name << "' graph from DB." << std::endl;
                    return -1;
                }
                return 0;
            }
            //Are all required flag are used?
            if( !parser.requirementFulfilled() ) {
                std::cerr << "Error: required option argument flags not set." << std::endl;
                return -1;
            }

            std::string graph_name = sbp::fileNameExtractor( options.fasta_file ); //"genome_01" //"test01" //"genome_02"
            std::cout << "File path: " << options.fasta_file << std::endl;
            std::cout << "Graph name: " << graph_name << std::endl;
            std::string dot_file = graph_name + ".dot";
            std::string compressed_dot_file = graph_name + "_compressed.dot";
            std::string indexed_dot_file = graph_name + "_indexed.dot";
            std::string check_dot_file = graph_name + "_reconstructed.dot";

            auto runner = sbp::PipelineRunner();
            auto kmer_graph = new eadlib::WeightedGraph<std::string>( graph_name );
            //Stage 1 - Loading the sequencer reads
            runner.loadFASTA( options.fasta_file, options.kmer_size, *kmer_graph );
            runner.exportToDot( dot_file, *kmer_graph );
            //Stage 2 - Compressing the graph
            runner.compressGraph( *kmer_graph );
            runner.exportToDot( compressed_dot_file, *kmer_graph );
            //Stage 3 - Indexing and saving to database
            runner.exportToDB( options.db_name, *kmer_graph );
            delete kmer_graph;
            //Stage 4 - Retrieving indexed version of the graph from the database
            auto index_graph = new eadlib::WeightedGraph<size_t>( graph_name );
            runner.importFromDB( options.db_name, *index_graph );
            runner.exportToDot( indexed_dot_file, *index_graph );
            runner.runTarjan( *index_graph );
            delete index_graph;
            //Stage 5 - Reconstructing the kmer graph
            auto reconstructed_kmer_graph = new eadlib::WeightedGraph<std::string>( graph_name );
            runner.importFromDB( options.db_name, *reconstructed_kmer_graph );
            runner.exportToDot( check_dot_file, *reconstructed_kmer_graph );
            delete ( reconstructed_kmer_graph );
        } else {
            std::cerr << "Wrong arguments given to the program." << std::endl;
        }
    } catch( std::regex_error e ) {
        std::cerr << "Malformed regular expression detected in parser option." << std::endl;
    }
    return 0;
}

/**
 *
 * @param file_path
 * @return
 */
std::string sbp::fileNameExtractor( const std::string &file_path ) {
    std::string name = file_path;
    size_t start  = name.find_last_of("\\/");
    size_t finish = name.find_last_of( "." );
    std::cout << start << ":" << finish << std::endl;
    if( start != std::string::npos ) {
        size_t dot = finish - start;
        name = name.substr( start + 1, dot - 1 );
    }
    return name;
}