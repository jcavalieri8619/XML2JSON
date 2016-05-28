/* 
 * File:   lineStorage.hpp
 * Author: John Cavalieri 
 *
 * Created on September 17, 2013, 10:52 AM
 */

#ifndef LINESTORAGE_HPP
#define	LINESTORAGE_HPP

#include <vector>
#include <string>

//lineStorage class takes an ifstream and stores the entire file
//as individual string lines. Each line can be accessed incrementally
//or by referencing a specific line number.

/**
 * structure designed to hold individual
 * lines of a file within strings for processing
 */
class lineStorage
{
public:

	/**
	 * constructor that reads each line from file
	 * into an internal vector of strings
	 * @param fileName: name of file to read
	 */
	lineStorage( const char * fileName );

	/**
	 * sequentially gets each line within vector
	 * 
	 * @param line: output - holds current line
	 * @return: true until all lines have been got 
	 */
	bool getLine( std::string& line ) const;

	/**
	 * gets line number identified by user
	 * @param line: output - holds line
	 * @param lineNumber: line number to get
	 * @return: false if line number out of bounds; else true 
	 */
	bool getLine( std::string& line, size_t lineNumber ) const;

	/**
	 * gets the amount of lines read and stored into internal vector
	 * @return 
	 */
	std::size_t getLineCount( ) const;

private:
	/**
	 * internal vector used to hold each line as string
	 */
	std::vector< std::string > lines;

	/**
	 * unsigned number - number lines stored
	 */
	std::size_t lineCount;

};

#endif	/* LINESTORAGE_HPP */

