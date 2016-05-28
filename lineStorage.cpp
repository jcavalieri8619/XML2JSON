/* 
 * File:   lineStorage.cpp
 * Author: John Cavalieri 
 * 
 * Created on September 18, 2013, 10:52 AM
 */

#include "lineStorage.hpp"
#include <fstream>
#include <iostream>
using namespace std;



//constructor - takes the name of a file
//whose contents will be stored by individual lines
//@param fileName - name of file to read


lineStorage::lineStorage( const char * fileName ): lineCount( 0 )
{
	ifstream stream( fileName );

	if ( !stream ){
		cerr << "Error opening file" << endl;
		return;
	}


	/*loop declares lineHolder as a temporary container for strings
	extracted from file stream.  These strings are then inserted
	at the end of vector.  The conditional statement uses 
	getline()'s return value, which is the stream, to access
	member good() to check if EOF or BAD are true.  
	The update statement calls push_back() to store extracted line.*/
	string lineHolder;
	for ( ; getline( stream >> ws, lineHolder ).good( ); )
		lines.push_back( lineHolder );


	//stream.bad() true indicates stream I/O error; not EOF
	if ( stream.bad( ) ){
		stream.close( );
		cerr << "Error extracting data from file" << endl;
		return;
	}

	lineCount = lines.size( );

	stream.close( );

}



//getLine method - each call to getLine
//results in the next line copied into
//the argument line.
//@ param line - reference to string that will hold a line
//@ return - returns false is all lines have been read
//true otherwise


bool lineStorage::getLine( std::string& line ) const
{
	static size_t lineNum = 0;

	if ( lineNum == lineCount ){
		lineNum = 0;
		return false;
	}

	line = lines.at( lineNum++ );
	return true;
}

//overloaded method getLine - gets the lineNumber'th line
//and copies it to the argument line.  
//@ param lineNumber - indicates which line will be copied
//@ return - false if no line stored at lineNumber position


bool lineStorage::getLine( std::string& line, size_t lineNumber ) const
{
	if ( lineNumber >= 0 && lineNumber < lineCount ){
		line = lines.at( lineNumber );
		return true;
	}
	return false;
}


std::size_t lineStorage::getLineCount( ) const
{
	return lineCount;
}