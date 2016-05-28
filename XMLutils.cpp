/* 
 * File:   XMLscan.hpp
 * Author: John Cavalieri 
 *
 * Created on September 17, 2013, 9:51 PM
 */

#include "XMLutils.hpp"

#include <cctype>
#include <string>
#include <vector>



using namespace XMLutil;
using std::string;


//TESTING STUFF
#include <iostream>
using std::cout;
using std::endl;



//utility functions used by XMLscanner.  These functions are
//not meant to be exposed to the user therefore they are not
//declared in the .hpp file. Definitions shown below
bool scanComment( const string& input, TAGTYPE tagType );
bool scanDirective( const string& input, string& directive );
bool scanTag( const string& input, string& tagName,
		  TAGTYPE tagType, bool& selfCloser );

XMLutil::PARSER_STATE XMLutil::XMLscanner( const std::string& input,
							 std::string& output )
{



	static PARSER_STATE state = STARTING_FILE;
	static string lastOutput = "";
	bool isSelfClosing = false;
	output = "";

	//each line of input is fed into this if / else if chain
	//to determine state

	if ( STARTING_FILE == state ) {
		if ( scanDirective( input, output ) )
			return state = DIRECTIVE;


	} else if ( scanComment( input, OPEN ) ) {
		if ( scanComment( input, CLOSE ) )
			return state = ONELINE_COMMENT;

		return state = START_COMMENT;


	} else if ( scanComment( input, CLOSE ) ) {
		//if closing comment does not eventually 
		//follow an opening comment tag --> malformed. 
		if ( state == START_COMMENT || state == IN_COMMENT ) {

			return state = END_COMMENT;

		} else return ERROR;


	} else if ( START_COMMENT == state ) {
		return state = IN_COMMENT;


	} else if ( scanTag( input, output, OPEN, isSelfClosing ) ) {

		if ( isSelfClosing ) {

			return state = COMPLETE_ELEMENT;
		}

		string tagName;
		if ( scanTag( input, tagName, CLOSE, isSelfClosing ) ) {

			if ( tagName != output )
				return state = ERROR;

			return state = COMPLETE_ELEMENT;

		}
		return state = ELEMENT_OPENING_TAG;


	} else if ( scanTag( input, output, CLOSE, isSelfClosing ) ) {
		return ELEMENT_CLOSING_TAG;


	} else if ( ELEMENT_OPENING_TAG == state ) {
		output = lastOutput;
		return state = CONTENT;
	}

	lastOutput = output;
	return state;
}

bool XMLutil::get_commentContent( const std::string& input, std::string& content )
{
	std::size_t beginContent = 0;
	std::size_t endContent = 0;

	//one line comment handled here
	if ( ( ( beginContent = input.find( "<!--" ) ) != string::npos ) ) {
		if ( ( ( endContent = input.find( "-->" ) ) != string::npos ) ) {

			beginContent += 4;
			content = input.substr( beginContent, endContent - beginContent );
			return true;

		}
	}
	//in comment state handled here
	content = input;

	//returns boolean only to be consistent with other getter
	return true;

}

bool XMLutil::get_elementContent( const std::string& input, std::string& content )
{
	//if self closing then content is empty
	if ( input.find( "/>" ) != string::npos ) {
		content = "";
		return true;
	}

	std::size_t beginContent = 0;
	std::size_t endContent = 0;

	//checks if input actually represents complete element with content
	if ( ( ( beginContent = input.find( ">" ) ) == string::npos ) ||
	 ( input.find( "<" ) == string::npos ) ) {
		content = input;
		//handles content on its own line
		return true;
	}



	++beginContent;

	std::size_t charItr = 0;
	//handles complete element on single line
	for (; input.at( beginContent + charItr ) != '<'; ++charItr );
	endContent = charItr;

	content = input.substr( beginContent, endContent );
	return true;

}

bool XMLutil::get_elementAttributes( const std::string & input,
						 attributeType* keyValue_pair )
{
	std::vector<std::string> container;

	size_t currPosn = 0;
	size_t end = 0;
	size_t beg = 0;



	while ( ( beg = input.find( ' ', currPosn ) ) != string::npos ) {

		if ( ( end = input.find( ' ', beg + 1 ) ) == string::npos ) {

			end = input.find( '>' );
			end--;
			container.push_back( input.substr( beg + 1, end - beg ) );

			break;

		} else {

			container.push_back( input.substr( beg + 1, end - beg ) );

			currPosn = end;
		}

	}

	if ( container.size( ) == 0 ) {
		keyValue_pair->clear( );
		return true;
	}

	string key, value;
	size_t endPosn, begPosn;
	for ( const auto & stringVect: container ) {

		endPosn = stringVect.find( '=' );

		key = stringVect.substr( 0, endPosn );

		begPosn = endPosn + 1;

		value = stringVect.substr( begPosn, endPosn - begPosn );

		( *keyValue_pair )[key] = value;
	}
	return true;
}

/**takes XML line and scans it for patterns matching a XML directive.
 * If found, the directive contents are returned in 2nd parameter
 * @param input: XML line
 * @param directive: directive contents if return is true
 * @return: true if directive found; else false
 */
bool scanDirective( const std::string& input, std::string& directive )
{
	std::size_t beginDir = 0;
	std::size_t endDir = 0;

	const string DirPatterns[] = {"?>", "<?"};

	if ( ( ( beginDir = input.find( DirPatterns[OPEN] ) ) != string::npos ) ) {

		endDir = input.find( DirPatterns[CLOSE], beginDir += 2 );

		directive = input.substr( beginDir, endDir - beginDir );
		return true;
	} else
		return false;
}

/**takes XML line and whether to look for open/close pattern for
 * comment then returns true if particular syntax found i.e. <!--
 * @param input: XML line
 * @param tagType: open, close, self_close
 * @return : true if pattern found; else false
 */
bool scanComment( const std::string& input, TAGTYPE tagType )
{
	const string CommPatterns[] = {"-->", "<!--"};

	if ( input.find( CommPatterns[tagType] ) == string::npos )
		return false;

	return true;

}

/**takes XML line and whether to look for open/close pattern
 * for element tags. If found returns true and tag name in 2nd parameter
 * @param input: XML line
 * @param tagName: if return true, holds element tag name
 * @param tagType: open, close, self_close
 * @param selfCloser: if line contains element tag and its self closing
 *	then selfCloser is set to true
 * @return: true if line contains element tag matching tagType
 */
bool scanTag( const std::string& input, std::string& tagName,
		  TAGTYPE tagType, bool& selfCloser )
{
	std::size_t beginTag = 0;
	std::size_t endTag = 0;
	selfCloser = false;

	const string tagPatterns[] = {"</", "<", "/>"};

	bool tagFound = false;



	if ( ( beginTag = input.find( tagPatterns[tagType] ) ) == string::npos )
		return false;

	if ( tagType == CLOSE ) {
		beginTag += 2; //move begin tag to start at first char in name
		tagFound = true;

	} else if ( ( tagType == OPEN ) &&
			isalpha( input.at( beginTag += 1 ) ) ) {

		tagFound = true;

		if ( input.find( tagPatterns[SELF_CLOSE], beginTag ) != string::npos ) {
			selfCloser = true;
		}
	}

	if ( tagFound == false )
		return false;

	size_t charItr = 0;
	char endChar;
	for (; ( ( endChar = input.at( beginTag + charItr ) ) != ' ' )
	 && ( endChar != '>' ) && ( endChar != '/' );
	 ++charItr );

	endTag = charItr;
	tagName = input.substr( beginTag, endTag );
	return true;

}