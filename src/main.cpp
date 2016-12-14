#include "PipelineRunner.h"

int main() {
    std::string graph_name          = "test01"; //"genome_01" //"test01" //"genome_02";
    std::string file_path           = "../data/" + graph_name + ".fasta";
    std::string dot_file            = graph_name + ".dot";
    std::string compressed_dot_file = graph_name + "_compressed.dot";
    std::string indexed_dot_file    = graph_name + "_indexed.dot";
    std::string check_dot_file      = graph_name + "_reconstructed.dot";
    std::string database_file       = "graphs.db";
    size_t      kmer_size           = 5;
    auto        runner              = sbp::PipelineRunner();
    auto        kmer_graph          = new eadlib::WeightedGraph<std::string>( graph_name );
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
    delete( reconstructed_kmer_graph );
    return 0;
}