/**
    @class          eadlib::wrapper::SQLite
    @brief          SQLite database wrapper to simplify usage of sqlite3
    @dependencies   eadlib::TableDB, eadlib::TableDBCell, eadlib::TableDBRow,
                    eadlib::TableDBCol, eadlib::TableDBCursor, eadlib::logger::Logger,
                    sqlite3
    @author         E. A. Davison
    @copyright      E. A. Davison 2016
    @license        GNUv2 Public License
**/
#ifndef EADLIB_SQLITE_H
#define EADLIB_SQLITE_H

#include <string>
#include <memory>
#include <istream>
#include "../../logger/Logger.h"
#include "sqlite3/sqlite3.h"
#include "TableDB.h"

namespace eadlib {
    namespace wrapper {
        class SQLite {
          public:
            SQLite();
            SQLite( const SQLite &sqlite ) = delete;
            SQLite( SQLite &&sqlite ) noexcept;
            ~SQLite();
            SQLite & operator =( const SQLite &rhs ) = delete;
            SQLite & operator =( SQLite &&rhs ) noexcept;
            bool open( const std::string &file_name );
            std::string getFileName() const;
            bool close();
            bool connected() const;
            size_t pull( const std::string &query, TableDB &table );
            bool   push( const std::string &query );
            bool   pullMetaData( const std::string &table_name, TableDB &table );
          private:
            //Variables
            bool _connected_flag;
            sqlite3 * _database;
            std::string _file_name;
            //Methods
            bool connect( const std::string &file_name );
            bool disconnect();
            size_t queryPull( const std::string &query, TableDB &table );
            bool queryPush( const std::string &query );
            TableDBCell::DataType getItemType( int type ) const;
            //Error code handling
            bool sqlMsgCode( int code );
            bool sqlMsgCode( int code, const std::string &query );
        };

        //-----------------------------------------------------------------------------------------------------------------
        // SQLite class public method implementations
        //-----------------------------------------------------------------------------------------------------------------
        /**
         * Constructor
         */
        inline SQLite::SQLite() :
            _connected_flag( false ),
            _database( nullptr ),
            _file_name( "" )
        {}

        /**
         * Move-Constructor
         * @param sqlite SQLite wrapper instance to move over
         */
        inline SQLite::SQLite( SQLite &&sqlite ) noexcept :
            _connected_flag( sqlite._connected_flag ),
            _database( sqlite._database ),
            _file_name( sqlite._file_name )
        {}

        /**
         * Destructor
         */
        inline SQLite::~SQLite() {
            if( _connected_flag ) {
                disconnect();
            }
        }

        /**
         * Move assignment operator
         * @param rhs SQLite instance to move over
         * @return Moved instance
         */
        inline SQLite & SQLite::operator =( SQLite &&rhs ) noexcept {
            _connected_flag = rhs._connected_flag;
            _database = rhs._database;
            _file_name = rhs._file_name;
            return *this;
        }

        /**
         * Connects to database file
         * @param file_name File name
         * @return Success
         */
        inline bool SQLite::open( const std::string &file_name ) {
            if( _connected_flag ) {
                _connected_flag = disconnect();
            }
            _connected_flag = connect( file_name );
            if( connected() ) {
                _file_name = file_name;
            }
            return _connected_flag;
        }

        /**
         * Gets the file name of the currently opened database
         * @return File name
         */
        inline std::string SQLite::getFileName() const {
            return _file_name;
        }

        /**
         * Disconnects from the current database
         * @return Disconnected state
         */
        inline bool SQLite::close() {
            if( _connected_flag ) {
                _connected_flag  = !disconnect();
                if( !connected() ) {
                    _file_name = "";
                }
            }
            return _connected_flag;
        }

        /**
         * Checks if SQLite instance is connected to a database
         * @return Connected state
         */
        inline bool SQLite::connected() const {
            return _connected_flag;
        }

        /**
         * Processes a query and stores the result in a table
         * @param query SQLite query
         * @param table TableDB to store result of query into
         * @return Number of rows fetched by the query
         */
        inline size_t SQLite::pull( const std::string &query, TableDB &table ) {
            if( !connected() ) {
                LOG_ERROR( "([eadlib::wrapper::SQLite::pull( '", query, "', ..)] Database file not opened." );
                return 0;
            } else {
                return queryPull( query, table );
            }
        }

        /**
         * Processes a query with no return data
         * @param query SQLite query
         * @return Success
         */
        inline bool SQLite::push( const std::string &query ) {
            if( !connected() ) {
                LOG_ERROR( "[eadlib::wrapper::SQLite::push( '", query, "' )] Database file not opened." );
                return false;
            } else {
                return queryPush( query );
            }
        }

        /**
         * Pulls the metadata of a table from the SQLite database
         * @param table_name Name of the table to get the metadata for
         * @param table TableDB to store the metadata into
         * @return Success
         */
        inline bool SQLite::pullMetaData( const std::string &table_name, TableDB &table ) {
            if( !connected() ) {
                LOG_ERROR( "[eadlib::wrapper::SQLite::pullMetaData( '", table_name, "', <TableDB> )] Database file not opened." );
                return false;
            } else {
                std::stringstream ss;
                ss << "PRAGMA table_info('" << table_name << "');";
                return ( pull( ss.str(), table ) ) > 0;
            }
        }

        //-----------------------------------------------------------------------------------------------------------------
        // SQLite class private method implementations
        //-----------------------------------------------------------------------------------------------------------------
        /**
         * Opens a connection to a SQLite database file
         * @param file_name File name
         * @return Success
         */
        inline bool SQLite::connect( const std::string &file_name ) {
            return !( sqlMsgCode( sqlite3_open( file_name.c_str(), &_database ) ) );
        }

        /**
         * Closes the current connection
         * @return Success
         */
        inline bool SQLite::disconnect() {
            return !( sqlMsgCode( sqlite3_close( _database ) ) );
        }

        /**
         * Processes and stores the result of given query into a table
         * ->Currently supports types: SQLITE_TEXT, SQLITE_INTEGER, SQLITE_FLOAT, SQLITE_NULL
         * @param query SQLite query string
         * @param table TableDB container for the returned data
         * @return Number of rows returned by the query
         * @throws std::invalid_argument when an unsupported type (Blob) from the database is encountered
         * @throws std::runtime_error when there is a problem adding data to the TableDB
         */
        inline size_t SQLite::queryPull( const std::string &query, TableDB &table ) {
            size_t row_counter { 0 };
            sqlite3_stmt *statement { };
            sqlMsgCode( sqlite3_prepare( _database, query.c_str(), -1, &statement, NULL ), query );
            int cols = sqlite3_column_count( statement ); //get number of columns from statement
            sqlMsgCode( sqlite3_step( statement ), query ); //execute statement

            table.reset(); //Making sure container is empty

            //Getting the headings & types
            for( int i = 0; i < cols; i++ ) {
                int type = sqlite3_column_type( statement, i );
                table.createColumn( std::string( sqlite3_column_name( statement, i ) ), getItemType( type ) );
            }
            //Columns set; Ready to add
            table.lockStructure();
            //Retrieving the data
            while( sqlite3_column_text( statement, 0 ) ) { //reading the 1st column of result
                for( int i = 0; i < cols; i++ ) {
                    bool success { false };
                    if( sqlite3_column_type( statement, i ) == SQLITE_TEXT ) {
                        if( sqlite3_column_text( statement, i ) == nullptr ) {
                            success = table.add( "" );
                        } else {
                            success = table.add(
                                std::string( (char*) ( sqlite3_column_text( statement, i ) ) ) );
                        }
                    } else if( sqlite3_column_type( statement, i ) == SQLITE_INTEGER ) {
                        success = table.add( static_cast<signed long long>( sqlite3_column_int( statement, i ) ) );
                    } else if( sqlite3_column_type( statement, i ) == SQLITE_FLOAT ) {
                        success = table.add( sqlite3_column_double( statement, i ) );
                    } else if( sqlite3_column_type( statement, i ) == SQLITE_NULL ) {
                        success = table.add();
                    } else {
                        sqlMsgCode( sqlite3_finalize( statement ), query );
                        //TODO look into adding BLOB support if at all possible
                        LOG_ERROR( "[eadlib::wrapper::SQLite::queryPull( '", query, "', <TableDB> )] Type unsupported (SQLITE_BLOB)." );
                        throw std::invalid_argument( "[eadlib::wrapper::SQLite::queryPull(..)] Type unsupported (SQLITE_BLOB)" );
                    }
                    if( !success ) {
                        sqlMsgCode( sqlite3_finalize( statement ), query );
                        LOG_ERROR( "[eadlib::wrapper::SQLite::queryPull( '", query, "', <TableDB> )] Problem adding data to table. Process stalled." );
                        throw std::runtime_error( "eadlib::wrapper::SQLite::queryPull(..)] Problem adding data to table. Process stalled." );
                    }
                }
                row_counter++;
                sqlMsgCode( sqlite3_step( statement ), query );
            }
            sqlMsgCode( sqlite3_finalize( statement ), query );
            return row_counter;
        }

        /**
         * Forward a query to the database with the expectation that no data will be returned
         * @param query SQL query string
         * @return Success
         */
        inline bool SQLite::queryPush( const std::string &query ) {
            sqlite3_stmt *statement { };
            //prepare statement
            bool step1 = sqlMsgCode( sqlite3_prepare_v2( _database, query.c_str(), -1, &statement, nullptr ), query );
            //execute statement
            bool step2 = sqlMsgCode( sqlite3_step( statement ), query );
            bool step3 = sqlMsgCode( sqlite3_finalize( statement ), query );
            return !( step1 && step2 && step3 );
        }

        /**
         * Converts SQLite data type code into TableDBCell::DataType
         * @param type SQLite data type code
         * @return TableDBCell::DataType translation
         */
        inline TableDBCell::DataType SQLite::getItemType( int type ) const {
            switch( type ) {
                case 1: //SQLITE_INTEGER
                    return TableDBCell::DataType::INT;
                case 2: //SQLITE_FLOAT
                    return TableDBCell::DataType::DOUBLE;
                case 3: //SQLITE_TEXT
                    return TableDBCell::DataType::STRING;
                case 4: //SQLITE_BLOB
                    LOG_ERROR( "[eadlib::sqlite::Db::getItemType( ", type, " )] "
                        "SQLite BLOB type is not supported by Item container! Using 'DataType::NONE'" );
                    return TableDBCell::DataType::NONE;
                case 5: //SQLITE_NULL
                    return TableDBCell::DataType::NONE;
                case 1000: //Non-SQLite type; Boolean
                    return TableDBCell::DataType::BOOL;
                default:
                    LOG_WARNING( "[eadlib::wrapper::SQLite::getItemType( ", type, " )] "
                        "SQLite type is not supported by Item container. Using 'DataType::STRING' as default." );
                    return TableDBCell::DataType::STRING;
            }
        }

        //-----------------------------------------------------------------------------------------------------------------
        // SQLite static error code method implementations
        //-----------------------------------------------------------------------------------------------------------------
        /**
         * SQLite3 Error code handling
         * @param code Error code
         * @return Problem state (0 = no problems)
         */
        inline bool SQLite::sqlMsgCode( int code ) {
            return sqlMsgCode( code, "" );
        }

        /**
         * SQLite3 Error code handling
         * (All codes are from the official manual)
         * SQLite message codes are forwarded here for logging purposes.
         * If the code matches an error then the function returns 'true' on the existence of a problem.
         * Source: http://www.sqlite.org/c3ref/c_abort.html
         * @param code Error code produced
         * @param query Query used if any
         * @return Problem state (0 = no problems)
         */
        inline bool SQLite::sqlMsgCode( int code, const std::string &query ) {
            switch( code ) {
                case SQLITE_OK: /* Successful result */
                    LOG_TRACE( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] SQLITE OK: ", query );
                    return 0;
                case SQLITE_ERROR: /* SQL error or missing database */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] SQL error or missing database: ", query );
                    return 1;
                case SQLITE_INTERNAL: /* Internal logic error in SQLite */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Internal logic error in SQLite: ", query );
                    return 1;
                case SQLITE_PERM: /* Access permission denied */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Access permission denied: ", query );
                    return 1;
                case SQLITE_ABORT: /* Callback routine requested an abort */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Callback routine requested an abort: ", query );
                    return 1;
                case SQLITE_BUSY: /* The database file is locked */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] The database file is locked: ", query );
                    return 1;
                case SQLITE_LOCKED: /* A table in the database is locked */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] A table in the database is locked: ", query );
                    return 1;
                case SQLITE_NOMEM: /* A malloc() failed */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] A malloc() failed: ", query );
                    return 1;
                case SQLITE_READONLY: /* Attempt to write a readonly database */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Attempt to write a readonly database: ", query );
                    return 1;
                case SQLITE_INTERRUPT: /* Operation terminated by sqlite3_interrupt()*/
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Operation terminated by sqlite3_interrupt(): ", query );
                    return 1;
                case SQLITE_IOERR: /* Some kind of disk I/O error occurred */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Some kind of disk I/O error occurred: ", query );
                    return 1;
                case SQLITE_CORRUPT: /* The database disk image is malformed */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] The database disk image is malformed: ", query );
                    return 1;
                case SQLITE_NOTFOUND: /* Unknown opcode in sqlite3_file_control() */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Unknown opcode in sqlite3_file_control(): ", query );
                    return 1;
                case SQLITE_FULL: /* Insertion failed because database is full */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Insertion failed because database is full: ", query );
                    return 1;
                case SQLITE_CANTOPEN: /* Unable to open the database file */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Unable to open the database file: ", query );
                    return 1;
                case SQLITE_PROTOCOL: /* Database lock protocol error */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Database lock protocol error: ", query );
                    return 1;
                case SQLITE_EMPTY: /* Database is empty */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Database is empty: ", query );
                    return 1;
                case SQLITE_SCHEMA: /* The database schema changed */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] The database schema changed: ", query );
                    return 1;
                case SQLITE_TOOBIG: /* String or BLOB exceeds size limit */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] String or BLOB exceeds size limit: ", query );
                    return 1;
                case SQLITE_CONSTRAINT: /* Abort due to constraint violation */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Abort due to constraint violation: ", query );
                    return 1;
                case SQLITE_MISMATCH: /* Data type mismatch */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Data type mismatch: ", query );
                    return 1;
                case SQLITE_MISUSE: /* Library used incorrectly */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Library used incorrectly: ", query );
                    return 1;
                case SQLITE_NOLFS: /* Uses OS features not supported on host */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Uses OS features not supported on host: ", query );
                    return 1;
                case SQLITE_AUTH: /* Authorisation denied */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Authorisation denied: ", query );
                    return 1;
                case SQLITE_FORMAT: /* Auxiliary database format error */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Auxiliary database format error: ", query );
                    return 1;
                case SQLITE_RANGE: /* 2nd parameter to sqlite3_bind out of range */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] 2nd parameter to sqlite3_bind out of range: ", query );
                    return 1;
                case SQLITE_NOTADB: /* File opened that is not a database file */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] File opened that is not a database file: ", query );
                    return 1;
                case SQLITE_NOTICE: /* Notifications from sqlite3_log() */
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Notifications from sqlite3_log(): ", query );
                    return 1;
                case SQLITE_WARNING: /* Warnings from sqlite3_log() */
                    LOG_DEBUG( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Warnings from sqlite3_log(): ", query );
                    return 1;
                case SQLITE_ROW: /* sqlite3_step() has another row ready */
                    LOG_DEBUG( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] sqlite3_step() has another row ready." );
                    return 0;
                case SQLITE_DONE: /* sqlite3_step() has finished executing */
                    LOG_DEBUG( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] sqlite3_step() has finished executing: ", query );
                    return 0;
                default:
                    LOG_ERROR( "[eadlib::wrapper::SQLite::sqlMsgCode( ", code, " )] Unrecognised SQlite3 error code." );
                    return 1;
            }
        }
    }
}

#endif //EADLIB_SQLITE_H
