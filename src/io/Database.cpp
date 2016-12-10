#include "Database.h"

/**
 * Constructor
 * @param writer File Writer
 */
sbp::io::Database::Database( const std::string &file_name ) {
    if( !_database.open( file_name ) ) {
        LOG_ERROR( "[sbp::io::KmerIndexExport::KmerIndexExport( ", file_name, " )] Could connect to database file." );
    }
    //TODO check table structure for indexing exists and if not then create it.
}

/**
 * Destructor
 */
sbp::io::Database::~Database() {

}

/**
 * Writes an index-kmer pair to file
 * @param index Index of the kmer
 * @param kmer  Graph kmer string
 * @return Success
 */
bool sbp::io::Database::writePair( const size_t &index, const std::string &kmer ) {
    //TODO send to db table
    return false;
}
