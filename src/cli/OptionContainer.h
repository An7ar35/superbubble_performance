#ifndef SUPERBUBBLE_PERFORMANCE_OPTIONCONTAINER_H
#define SUPERBUBBLE_PERFORMANCE_OPTIONCONTAINER_H

#include <string>
#include <iostream>

namespace sbp {
    namespace cli {
        struct OptionContainer {
            //Input options
            std::string fasta_file { "" }; //Fasta file to import (-f)
            size_t      kmer_size  { 0 };  //Kmer string size on nodes (-k)
            //Dot format output options
            bool d  { false }; //On the fly at each stages passed
            bool dk { false }; //From DB as kmer string nodes
            bool di { false }; //From DB as indexed nodes
            //Database options
            bool        save_flag   { false };       //Save graph to DB (-s)
            bool        remove_flag { false };       //Deletes graph from DB (-r)
            std::string graph_name  { "" };          //Name of the graph
            std::string db_name     { "graphs.db" }; //Name of the DB file
            bool        list_flag   { false };       //List DB graphs names
            //Superbubble algorithm options
            bool compress_flag  { false };
            bool sb1            { false };
            bool sb2            { false };
            bool sb3            { false };
        };
    }
}

#endif //SUPERBUBBLE_PERFORMANCE_OPTIONCONTAINER_H
