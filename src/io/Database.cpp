#include "Database.h"

/**
 * Constructor
 * @param writer File Writer
 */
sbp::io::Database::Database() :
    _database( eadlib::wrapper::SQLite() ),
    _graph_name( "" )
{}

/**
 * Move-Constructor
 * @param db Database instance to move over
 */
sbp::io::Database::Database( sbp::io::Database &&db ) noexcept {
    _database = std::move( db._database );
    _graph_name = db._graph_name;
}

/**
 * Destructor
 */
sbp::io::Database::~Database() {}

/**
 * Move assignement operator
 * @param rhs Database instance to move over
 * @return Moved database instance
 */
sbp::io::Database & sbp::io::Database::operator =( sbp::io::Database &&rhs ) noexcept {
    _database = std::move( rhs._database );
    _graph_name = rhs._graph_name;
    return *this;
}

/**
 * Opens a connection to the database
 * @param file_name File name of database
 * @return Success
 */
bool sbp::io::Database::open( const std::string &file_name ) {
    if( !_database.open( file_name ) ) {
        LOG_ERROR( "[sbp::io::Database::open( ", file_name, " )] Could not open a connection to the database." );
        return false;
    } else {
        auto table = eadlib::TableDB();
        std::string check_graph_table_query = { "SELECT name FROM sqlite_master WHERE type='table' AND name='Graphs' COLLATE NOCASE" };
        if( _database.pull( check_graph_table_query, table ) == 0 ) {
            std::string create_table_query { "CREATE TABLE Graphs("
                                                 "GraphID INTEGER PRIMARY KEY, "
                                                 "Name TEXT NOT NULL, "
                                                 "CONSTRAINT unique_name UNIQUE( Name ));"
            };
            if( !_database.push( create_table_query ) ) {
                LOG_ERROR( "[sbp::io::Database::open( ", file_name, " )] Problem creating 'Graphs' table in database." );
                return false;
            } else {
                LOG_DEBUG( "[sbp::io::Database::open( ", file_name, " )] New DB detected. Created a 'Graphs' table." );
            }
        }
        return true;
    }
}

/**
 * Closes the current connection (if any) to the database
 * @return Success
 */
bool sbp::io::Database::close() {
    return _database.close();
}

/**
 * Checks if databse is connected
 * @return Connected state
 */
bool sbp::io::Database::isOpen() const {
    return _database.connected();
}

/**
 * Creates a table set for a new graph
 * @param graph_name Graph name
 * @return Success
 */
bool sbp::io::Database::create( const std::string &graph_name ) {
    if( exists( graph_name ) ) {
        auto graph_id = getGraphID( graph_name );
        auto table = eadlib::TableDB();
        std::string kmers_query { "SELECT COUNT(*) FROM kmers_" + std::to_string( graph_id ) };
        if( _database.pull( kmers_query, table ) > 0 ) {
            LOG_ERROR( "[sbp::io::Database::create( ", graph_name, " )] Kmers table already exists with data." );
            return false;
        }
        std::string edges_query { "SELECT COUNT(*) FROM edges_" + std::to_string( graph_id ) };
        if( _database.pull( edges_query, table ) > 0 ) {
            LOG_ERROR( "[sbp::io::Database::create( ", graph_name, " )] Edges table already exists with data." );
            return false;
        }
    } else {
        if( !_database.push( "INSERT INTO Graphs (Name) VALUES ('" + graph_name + "')" ) ) {
            LOG_ERROR( "[sbp::io::Database::create( ", graph_name, " )] Couldn't add graph to database." );
            return false;
        }
        auto graph_id = getGraphID( graph_name );
        if( graph_id < 0 ) {
            LOG_ERROR( "[sbp::io::Database::create( ", graph_name, " )] Couldn't get graph ID from database." );
            //remove( graph_name );
            return false;
        }
        std::string create_kmer_table_query { "CREATE TABLE kmers_" + std::to_string( graph_id ) + "( "
                                                                                                 "ID INTEGER NOT NULL, "
                                                                                                 "Kmer TEXT NOT NULL, "
                                                                                                 "CONSTRAINT unique_id UNIQUE( ID ), "
                                                                                                 "FOREIGN KEY(ID) REFERENCES Graphs(Graph_ID))"
        };
        if( !_database.push( create_kmer_table_query ) ) {
            LOG_ERROR( "[sbp::io::Database::create( ", graph_name, " )] Couldn't create Kmer table 'kmers_", graph_id, "'." );
            //remove( graph_name );
            return false;
        }
        std::string create_edge_table_query { "CREATE TABLE edges_" + std::to_string( graph_id ) + "( "
                                                                                                 "From_ID INTEGER NOT NULL, "
                                                                                                 "To_ID INTEGER NOT NULL, "
                                                                                                 "Weight INTEGER NOT NULL, "
                                                                                                 "FOREIGN KEY(From_ID) REFERENCES Graphs(Graph_ID), "
                                                                                                 "FOREIGN KEY(To_ID) REFERENCES Graphs(Graph_ID))"
        };
        if( !_database.push( create_edge_table_query ) ) {
            LOG_ERROR( "[sbp::io::Database::create( ", graph_name, " )] Couldn't create Edge table 'edges_", graph_id, "'." );
            //remove( graph_name );
            return false;
        }
        LOG_DEBUG( "[sbp::io::Database::create( ", graph_name, " )] Tables for graph (ID=", graph_id, ") created." );
    }
    return true;
}

/**
 * Removes a graph and all related tables from the database
 * @param graph_name Graph name
 * @return Deleted state
 */
bool sbp::io::Database::remove( const std::string &graph_name ) {
    if( exists( graph_name ) ) {
        bool error_flag { false };
        auto graph_id = getGraphID( graph_name );
        if( !_database.push( "DROP TABLE graphs.kmers_" + std::to_string( graph_id ) ) ) {
            LOG_ERROR( "sbp::io::Database::remove( ", graph_name, " )] Couldn't drop Kmer table 'kmers_", graph_id, "'." );
            error_flag = true;
        }
        if( !_database.push( "DROP TABLE graphs.edges_" + std::to_string( graph_id ) ) ) {
            LOG_ERROR( "sbp::io::Database::remove( ", graph_name, " )] Couldn't drop Edges table 'edges_", graph_id, "'." );
            error_flag = true;
        }
        if( !_database.push( "DELETE FROM Graphs WHERE Name='" + graph_name + "'" ) ) {
            LOG_ERROR( "sbp::io::Database::remove( ", graph_name, " )] Problem deleting entry in Graphs table." );
            error_flag = true;
        }
        return !error_flag;
    }
    return true;
}

/**
 * Checks if a graph exists in the database
 * @param graph_name Graph name
 * @return Existence state
 */
bool sbp::io::Database::exists( const std::string &graph_name ) {
    std::string graph_exists_query = { "SELECT * FROM Graphs WHERE Name='" + graph_name + "'" };
    auto table = eadlib::TableDB();
    return _database.pull( graph_exists_query, table ) > 0;
}

/**
 * Gets the ID associated with the graph
 * @param graph_name Graph name
 * @return Graph ID string ('-1' if not found)
 */
std::string sbp::io::Database::getID( const std::string &graph_name ) {
    return std::to_string( getGraphID( graph_name ) );
}

/**
 * Writes to the index table the node information
 * @param graph_id Graph ID
 * @param index    Node index id
 * @param kmer     Node content
 * @return Success
 */
bool sbp::io::Database::writeNode( const std::string &graph_id, const size_t &index, const std::string &kmer ) {
    return _database.push( "INSERT INTO kmers_"
                           + graph_id + " (ID, Kmer) VALUES ("
                           + std::to_string( index )
                           + ", '"
                           + kmer
                           + "');"
    );
}

/**
 * Writes to the edge table the edge
 * @param graph_id Graph ID
 * @param from     Origin node id
 * @param to       Destination node id
 * @return Success
 */
bool sbp::io::Database::writeEdge( const std::string &graph_id, const size_t &from, const size_t &to, const size_t &weight ) {
    return _database.push( "INSERT INTO edges_"
                           + graph_id + " (From_ID, To_ID, Weight) VALUES ("
                           + std::to_string( from )
                           + ", "
                           + std::to_string( to )
                           + ", "
                           + std::to_string( weight )
                           + ");"
    );
}

/**
 * Begin a transaction
 */
void sbp::io::Database::beginTransaction() {
    _database.push( "BEGIN;" );
}

/**
 * Commit transaction
 */
void sbp::io::Database::commitTransaction() {
    _database.push( "COMMIT;" );
}

/**
 * Rollback transaction
 */
void sbp::io::Database::rollbackTransaction() {
    _database.push( "ROLLBACK;" );
}

/**
 * Loads an index graph from the database
 * @param graph_name Graph name
 * @param graph      Index graph instance to load into
 * @return Success
 */
bool sbp::io::Database::loadGraph( const std::string &graph_name, eadlib::WeightedGraph<size_t> &graph ) {
    auto graph_id = getGraphID( graph_name );
    //Error control
    if( graph_id < 0 ) {
        LOG_ERROR( "sbp::io::Database::loadIndexGraph( ", graph_name, ", .. )] Graph not found in DB." );
        return false;
    }
    if( !graph.isEmpty() ) {
        LOG_ERROR( "sbp::io::Database::loadIndexGraph( ", graph_name, ", .. )] WeightedGraph<size_t> instance not empty." );
        return false;
    }
    //Size check
    auto table = eadlib::TableDB();
    std::string size_query { "SELECT COUNT(*) FROM edges_" + std::to_string( graph_id ) };
    if( _database.pull( size_query, table ) < 1 || table.at( 0, 0 ).getInt() < 1 ) {
        LOG_ERROR( "sbp::io::Database::loadIndexGraph( ", graph_name, ", .. )] Problem getting size of 'edges_", graph_id, "' table." );
        return false;
    }
    long long int total_rows { table.at( 0, 0 ).getInt() };
    //Filling graph
    if( total_rows < 1000 ) { //pull everything
        std::string all_data_query { "SELECT * FROM edges_" + std::to_string( graph_id ) };
        if( _database.pull( all_data_query, table ) < 1 ) {
            LOG_ERROR( "sbp::io::Database::loadIndexGraph( ", graph_name, ", .. )] Problem pulling all data from 'edges_", graph_id, "' table." );
            return false;
        } else {
            for( auto it = table.begin(); it != table.end(); ++it ) { //Iterate the rows
                graph.createDirectedEdge_fast( static_cast<size_t>( it->at( 0 ).getInt() ),   //Origin node ID
                                               static_cast<size_t>( it->at( 1 ).getInt() ),   //Destination node ID
                                               static_cast<size_t>( it->at( 2 ).getInt() ) ); //Edge weight
            }
        }
    } else { //pull per 1000 size chunks
        size_t chunks { 0 };
        size_t offset { 0 };
        std::string chunk_query = { "SELECT * FROM edges_" + std::to_string( graph_id ) + " LIMIT 1000 OFFSET " + std::to_string( offset ) };
        while( _database.pull( chunk_query, table ) ) {
            chunks++;
            for( auto it = table.begin(); it != table.end(); ++it ) { //Iterate the rows
                graph.createDirectedEdge_fast( static_cast<size_t>( it->at( 0 ).getInt() ),   //Origin node ID
                                               static_cast<size_t>( it->at( 1 ).getInt() ),   //Destination node ID
                                               static_cast<size_t>( it->at( 2 ).getInt() ) ); //Edge weight
            }
            offset += 1000;
            chunk_query = { "SELECT * FROM edges_" + std::to_string( graph_id ) + " LIMIT 1000 OFFSET " + std::to_string( offset ) };
        }
        LOG_DEBUG( "[sbp::io::Database::loadIndexGraph( ", graph_name, ", .. )] Processed ", chunks, " chunks from 'edges_", graph_id, "' table." );
    }
    return true;
}

/**
 * Loads a Kmer string graph from the database
 * @param graph_name Graph name
 * @param graph      Kmer string graph instance to load into
 * @return Success
 */
bool sbp::io::Database::loadGraph( const std::string &graph_name, eadlib::WeightedGraph<std::string> &graph ) {
    //TODO
    return false;
}

/**
 * Gets the id of the graph
 * @param graph_name Graph name
 * @return ID number of the graph (-1 if not found)
 */
signed long long sbp::io::Database::getGraphID( const std::string &graph_name ) {
    auto table = eadlib::TableDB();
    std::string query { "SELECT GraphID FROM Graphs WHERE Name='" + graph_name + "'" };
    if( _database.pull( query, table ) == 0 ) {
        LOG_ERROR( "[sbp::io::Database::getGraphID( ", graph_name, " )] Graph ID not found." );
        return -1;
    } else {
        return table.at( 0, 0 ).getInt();
    }
}
