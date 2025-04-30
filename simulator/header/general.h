#pragma once

template<typename ... Args>
std::string string_format( const std::string & format , Args ... args )
{
    int size_s = std::snprintf( nullptr , 0 , format.c_str() , args ... ) + 1; // Extra space for '\0'
    if ( size_s <= 0 )
    {
        throw std::runtime_error( "Error during formatting." );
    }
    auto size = static_cast< size_t >( size_s );
    std::unique_ptr<char[]> buf( new char[ size ] );
    std::snprintf( buf.get() , size , format.c_str() , args ... );
    return std::string( buf.get() , buf.get() + size - 1 ); // We don't want the '\0' inside
}

void ltrim( std::string & s );
void rtrim( std::string & s );
void trim( std::string & s );
std::string ltrim_copy( std::string s );
std::string rtrim_copy( std::string s );
std::string trim_copy( std::string s );
bool is_number( const std::string & s );
std::string ReplaceAll( std::string & str , const std::string & from , const std::string & to );
bool try_read_token( stringstream & line_token , const char * tried_token , string & readed_token , char escapechar = ']' , bool bassert = true );

void StrTokenizer( string & source , const char * delimiter , vector<string> & Tokens );
string trim_floatnumber( double fn );

double nextTime( double rateParameter );

double nonzero_in_range( double value , double zeroranje );

#define SIZEOF( ar ) ( sizeof( ar ) / sizeof( ar[ 0 ] ) )

string get_time( const char * style = "%Y-%m-%d %H:%M:%S" );

