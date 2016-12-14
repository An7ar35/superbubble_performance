#include "PipelineRunner.h"
#include "cli/CliOptions.h"

int main( int argc, char *argv[] ) {
    std::string program_name { "###########################\n"
                               "# Superbubble Performance #\n"
                               "###########################" };
    auto parser = eadlib::cli::Parser( program_name );
    auto cli_options = sbp::cli::CliOptions( parser, argv );
    try {
        if( cli_options.parse( argc, argv ) ) {
            std::string graph_name          = "genome_02"; //"genome_01" //"test01" //"genome_02";
            std::string file_path           = "../data/" + graph_name + ".fasta";
            std::string dot_file            = graph_name + ".dot";
            std::string compressed_dot_file = graph_name + "_compressed.dot";
            std::string indexed_dot_file    = graph_name + "_indexed.dot";
            std::string check_dot_file      = graph_name + "_reconstructed.dot";
            std::string database_file       = "graphs.db";

            if( parser.optionUsed( "-l" ) ) {
                auto db = sbp::io::Database();
                db.open( database_file );
                db.listGraphs( std::cout );
            } else {

                size_t kmer_size = 8;
                auto runner = sbp::PipelineRunner();
                auto kmer_graph = new eadlib::WeightedGraph<std::string>( graph_name );
                //Stage 1 - Loading the sequencer reads
                runner.loadFASTA( file_path, kmer_size, *kmer_graph );
                runner.exportToDot( dot_file, *kmer_graph );
                //Stage 2 - Compressing the graph
                runner.compressGraph( *kmer_graph );
                runner.exportToDot( compressed_dot_file, *kmer_graph );
                //Stage 3 - Indexing and saving to database
                runner.exportToDB( database_file, *kmer_graph );
                delete kmer_graph;
                //Stage 4 - Retrieving indexed version of the graph from the database
                auto index_graph = new eadlib::WeightedGraph<size_t>( graph_name );
                runner.importFromDB( database_file, *index_graph );
                runner.exportToDot( indexed_dot_file, *index_graph );
                delete index_graph;
                //Stage 5 - Reconstructing the kmer graph
                auto reconstructed_kmer_graph = new eadlib::WeightedGraph<std::string>( graph_name );
                runner.importFromDB( database_file, *reconstructed_kmer_graph );
                runner.exportToDot( check_dot_file, *reconstructed_kmer_graph );
                delete ( reconstructed_kmer_graph );
            }
        } else {
            std::cerr << "Wrong arguments given to the program." << std::endl;
        }
    } catch( std::regex_error e ) {
        std::cerr << "Malformed regular expression detected in parser option." << std::endl;
    }
    return 0;
}