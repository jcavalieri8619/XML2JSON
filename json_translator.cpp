/* 
 * File:   json_translator.cpp
 * Author: John Cavalieri
 * 
 * Created on November 7, 2013, 10:41 PM
 */

#include "json_translator.hpp"
#include <utility>
#include <vector>
#include <sstream>
using namespace std;





//index to state tuples
#define ENTITY 0
#define DEPTH 1
#define NAME 2
#define INDENTLEVEL 3

// index to node Queue
#define CURRENT 0
#define NEXT 1



static const std::string delimiters[] = { "{", "}", "[", "]", "," };


XMLTree_JSON::XMLTree_JSON( const std::string fileName ):
		indentSize( 0 ), fileOutput( fileName.data( ) ),
		THIS_Translator( this )
{
	if ( !fileOutput )
		throw "Error Opening output file";


}


XMLTree_JSON::~XMLTree_JSON( )
{
	delete THIS_Translator;
}


bool XMLTree_JSON::isClosing( const Tree_node* whatNode, bool prior )const
{
	return isClosingArray( whatNode, prior ) || isClosingObject( whatNode, prior );
}


STATUS XMLTree_JSON::initializeTranslator( Tree_node * tree,
										   XMLTree_JSON* * object )
{
	static XMLTree_JSON* self = new XMLTree_JSON;
	static bool initialized = false;

	//first call to initializeTranslator will construct XMLTree_JSON
	//object in the heap and pass back a pointer to it through
	//the *object parameter
	if ( ! initialized ){
		initialized = true;
		*object = self;
		return OK;

		//if initialized and called in the appropriate manner
		//then populate queue- passing to XMLtree traverse 
		//will always be called in appropriate manner.
	} else if ( tree && !object ){
		self->pushNode( tree );
		return OK;

		//if not called in the appropriate manner then ERROR
	} else return ERROR;


}


STATUS XMLTree_JSON::translator(  )
{


	//if XML tree is empty then translator is finished
	if ( NodeQueueEmpty( ) )
		return OK;



	bool rootProcessed = false;


	stringstream lineContainer( ios_base::out
							 | ios_base::ate );




	while ( !NodeQueueEmpty( ) ){

		//these variables need to be reinitialized
		//on every loop of translation process
		string	nodeName;

		size_t nodeDepth = 0;

		size_t currIndent = 0;

		vector<pair<string, string> > nodalKeyVals;







		if ( isClosing( getQueuedNode( CURRENT ) ) ){
			//loops begins by checking if the current node
			//closes the most nested entity (object or array);
			//if so then it is closed. If current
			//node closes multiple entities then each will be
			//closed before the node is further processed.

			generateOutput( lineContainer, Closing,
						 vector<pair<string, string> >( ) );

			popState( );

			continue;

		} else if ( isOpeningArray( ) &&
					( state_extractName( getCurrentState( ) ) !=
					 getQueuedNode( CURRENT )->getElementName( ) ) ){
			//determines in current node opens an array;
			//if so the appropriate output is generated.
			//the current node is not popped off front of queue
			//yet though because the node still has to be processed.
			//This is why gatherNodalData is not called within this
			//block

			gatherStateData( nodeDepth, nodeName, currIndent );

			pushState( ARRAY, nodeDepth, nodeName, currIndent );

			generateOutput( lineContainer, ArrayOpening,
						 vector<pair<string, string> >( ) );



			continue;

		} else if ( isOpeningObject( ) ){


			gatherStateData( nodeDepth, nodeName, currIndent );

			pushState( OBJECT, nodeDepth, nodeName, currIndent );

			gatherNodalData( nodalKeyVals );

			generateOutput( lineContainer,
						 ( rootProcessed ?
						  Opening : ( rootProcessed = true, RootOpening ) ),
						 nodalKeyVals );

			popFrontNode( );

			continue;

		} else{


			//must be a key value pair == complete element
			gatherNodalData(  nodalKeyVals );

			generateOutput( lineContainer, KeyVal, nodalKeyVals );

			popFrontNode( );

			continue;

		}
	}
	//unwind state stack by closing each remaining entity
	while ( !StateStackEmpty( ) ){

		generateOutput( lineContainer, Closing,
					 vector<pair<string, string> >( ) );

		popState( );
	}



	return OK;

}


std::size_t XMLTree_JSON::StateStackSize( )const
{
	return StateStack.size( );
}


bool XMLTree_JSON::StateStackEmpty( )const
{
	return StateStack.empty( );
}


std::size_t XMLTree_JSON::NodeQueueSize( )const
{
	return NodeQueue.size( );
}


bool XMLTree_JSON::NodeQueueEmpty( )const
{
	return NodeQueue.empty( );
}


void XMLTree_JSON::pushNode( const Tree_node * node )
{
	NodeQueue.push_back( node );

}


void XMLTree_JSON::popFrontNode( )
{
	NodeQueue.pop_front( );
}


void XMLTree_JSON::gatherStateData( std::size_t& depth, std::string& name,
									std::size_t & indentLevel )const
{

	depth = getQueuedNode( CURRENT )->getElementDepth( );
	name = getQueuedNode( CURRENT )->getElementName( );
	indentLevel = getIndentSize( );
}


bool XMLTree_JSON::isOpeningObject(   )const
{


	return countObjectChildren( ) > 0;

}


bool XMLTree_JSON::isOpeningArray(   )const
{

	size_t depth_ = getQueuedNode( CURRENT )->getElementDepth( );
	string name_ = getQueuedNode( CURRENT )->getElementName( );


	for ( const auto & nodeItr : NodeQueue ){

		if ( nodeItr != getQueuedNode( CURRENT ) ){

			if ( nodeItr->getElementDepth( ) < depth_ )
				return false;

			if ( ( nodeItr->getElementDepth( ) == depth_ )
				 && ( nodeItr->getElementName( ) == name_ ) )
				return true;
		}
	}

	return false;
}


std::tuple<ValueEntity, std::size_t,
std::string, std::size_t> XMLTree_JSON::getCurrentState( )const
{
	return StateStack.back( );
}


std::tuple<ValueEntity, std::size_t,
std::string, std::size_t> XMLTree_JSON::getPriorState( )const
{
	auto rItr = StateStack.rbegin( );
	return *( ++rItr );
}


ValueEntity XMLTree_JSON::state_extractEntity(  std::tuple<ValueEntity,
											   std::size_t, std::string,
											   std::size_t> state )const
{
	return get<ENTITY>( state );

}


std::size_t XMLTree_JSON::state_extractDepth(  std::tuple<ValueEntity,
											  std::size_t, std::string,
											  std::size_t> state )const
{
	return get<DEPTH>( state );

}


std::size_t XMLTree_JSON::state_extractIndentLevel( std::tuple<ValueEntity,
													std::size_t, std::string,
													std::size_t> state )const
{
	return get<INDENTLEVEL>( state );
}


std::string XMLTree_JSON::state_extractName(  std::tuple<ValueEntity,
											 std::size_t, std::string,
											 std::size_t> state )const
{
	return get<NAME>( state );

}


void XMLTree_JSON::pushState(  ValueEntity value, std::size_t depth,
							  std::string name, std::size_t indentLevel )
{

	StateStack.push_back( make_tuple( value, depth, name, indentLevel ) );
}


void XMLTree_JSON::popState( )
{


	StateStack.pop_back( );
}


bool XMLTree_JSON::isKeyValuePair(  )const
{
	return countObjectChildren( ) == 0;
}


bool XMLTree_JSON::isClosingArray(  const Tree_node * whatNode,
								   bool prior )const
{

	if ( !StateStackEmpty( ) ){

		if ( state_extractEntity( prior ?
								 getPriorState( ) :
								 getCurrentState( ) ) == ARRAY  ){


			if (  whatNode->getElementDepth( ) <

				 ( state_extractDepth( prior ?
									  getPriorState( ) :
									  getCurrentState( ) ) )   ){

				return true;




			} else if ( ( state_extractDepth( ( prior ?
												getPriorState( ) :
												getCurrentState( ) ) )  ==

						  whatNode->getElementDepth( ) ) ){


				if ( state_extractName( ( prior ?
										 getPriorState( ) :
										 getCurrentState( ) ) )   !=
					 whatNode->getElementName( ) ){


					return true;
				}
			}
		}
	}

	return false;
}


bool XMLTree_JSON::isClosingObject(  const Tree_node * whatNode,
									bool prior  )const
{


	if ( !StateStackEmpty( ) ){

		if ( state_extractEntity( ( prior ?
								 getPriorState( ) : getCurrentState( ) ) )
			 == OBJECT  ){


			if ( ( whatNode->getElementDepth( ) )  <=

				 state_extractDepth( ( prior ?
									  getPriorState( ) : getCurrentState( ) ) ) )

				return true;


		}

	}

	return false;


}


bool XMLTree_JSON::isClosingCommaNeeded( )const
{
	if ( NodeQueueEmpty( ) )
		return false;

		//if isClosing( CurrNode, priorState) --> current delimiter NO COMMA

	else return !isClosing( getQueuedNode( CURRENT ), true );

}


bool XMLTree_JSON::isCommaNeeded( )const
{
	//if isClosing(next) then last nodal pair of current node-->NO COMMA
	if ( NodeQueueSize( ) == 1 )
		return false;


	return !isClosing( getQueuedNode( NEXT ) );

}


void XMLTree_JSON::insertDelimiter( std::stringstream& formJSONLine,
									delimiterType delimiter )const
{


	formJSONLine << delimiters[delimiter];

}


void XMLTree_JSON::insertComma( std::stringstream & formJSONLine )const
{


	formJSONLine << delimiters[COMMA];
}

const static string QUOTE( "\"" );


void XMLTree_JSON::insertKeyValuePair( std::stringstream& formJSONLine,
									   std::string key,
									   std::string val )const
{
	formJSONLine << QUOTE + key + QUOTE << " : ";

	if ( ( val.size( ) == 0 ) || ( val.find( QUOTE ) != string::npos ) )
		formJSONLine << val;

	else formJSONLine << QUOTE + val + QUOTE;


}


std::size_t XMLTree_JSON::getCurrentNodeDepth( )const
{


	return getQueuedNode( CURRENT )->getElementDepth( );
}


std::size_t XMLTree_JSON::getIndentSize( )const
{


	return indentSize;
}


void XMLTree_JSON::setIndentSize( std::size_t size )
{


	indentSize = size;
}


STATUS XMLTree_JSON::writeJSONLine(  std::stringstream & JSONLine )
{

	string currString = JSONLine.str( );
	size_t length = currString.length( );


	//setfill used to prepend indent onto output string
	//where indent size is equal to size of the string plus
	//the indent size --> this ensures setfill will kick
	//in because the min width will always be greater than
	//string length
	fileOutput << setfill( ' ' ) <<
			setw( length + getIndentSize( ) ) <<
			right <<
			currString << endl;

	if ( ! fileOutput )
		return ERROR;



	//resets string stream for next output
	JSONLine.clear( );
	JSONLine.str( string( "" ) );
	return OK;
}


const Tree_node * XMLTree_JSON::getQueuedNode( std::size_t whatNodeIndex )const
{


	return NodeQueue[whatNodeIndex];
}


std::size_t XMLTree_JSON::countObjectChildren( )const
{


	return getQueuedNode( CURRENT )->getNumAttributes( ) +
			getQueuedNode( CURRENT )->getNumChildren( ) ;

}



//indent changes size by 2 to emphasize hierarchy


void XMLTree_JSON::incrementIndent( )
{


	indentSize += 2;
}






//after gathering nodal data, build line looping through
//nodal pairs vector.  build line will build line using
//key and value parameters and type.  If opening type
//then no need to check for comma. If closing type
//then key and value will be empty strings. If opening
//then value will be empty.  After building call insert indent
//so that line size can be used to determine indent 


void XMLTree_JSON::generateOutput( std::stringstream& formJSONLine, lineType type,
								   const std::vector<std::pair<std::string, std::string> > & nodalPairs )
{

	const Tree_node * CurrNode = getQueuedNode( CURRENT );


	switch ( type )
	{
		case Opening:
		{

			//print "name : " if state top is not ARRAY
			//if ARRAY then just print opening delimiter
			//after printing each "{ " opening delimiter
			//increment indentSize member. 

			if ( StateStackSize( ) > 2 &&
				 state_extractEntity( getPriorState(  ) ) == ARRAY ){

				insertDelimiter( formJSONLine, OpenObject );

				writeJSONLine( formJSONLine );

			} else{

				insertKeyValuePair( formJSONLine, CurrNode->getElementName( ) );

				writeJSONLine( formJSONLine );

				insertDelimiter( formJSONLine, OpenObject );

				writeJSONLine( formJSONLine );



			}

			incrementIndent( );

			//prints any content and attributes associated with the node

			for ( auto keyValItr = nodalPairs.begin( );
				 keyValItr  != nodalPairs.end( ); ++keyValItr ){

				insertKeyValuePair( formJSONLine,
								 keyValItr->first,
								 keyValItr->second );


				if ( ( keyValItr + 1 == nodalPairs.end( ) ) ){

					if ( ! isCommaNeeded( ) ){
						writeJSONLine( formJSONLine );
						break;

					}
				}

				insertComma( formJSONLine );
				writeJSONLine( formJSONLine );

			}

			break;

		}

		case KeyVal:
		{

			insertKeyValuePair( formJSONLine,
							 nodalPairs.front( ).first,
							 nodalPairs.front( ).second );

			if ( isCommaNeeded( ) )
				insertComma( formJSONLine );


			writeJSONLine( formJSONLine );
			break;


		}

		case Closing:
			//each closing "} " delimiter is printed at
			//state_extractIndent(getCurrState()) then
			//indentSize is set equal extracted indent
			//to determine if comma needed, check if 
			//next node is NOT closing - if true add comma
			//else no comma needed

			setIndentSize( state_extractIndentLevel( getCurrentState( ) ) );

			insertDelimiter( formJSONLine,
							 ( state_extractEntity( getCurrentState( ) ) == ARRAY ?
							  CloseArray : CloseObject ) );



			//top state is about to be popped so comma needed
			//if the current node is going to close the next
			//highest state on state stack
			if ( isClosingCommaNeeded( ) )
				insertComma( formJSONLine );

			writeJSONLine( formJSONLine );

			break;

		case ArrayOpening:

			//print opening "name : " and "[ "

			insertKeyValuePair( formJSONLine, CurrNode->getElementName( ) );

			writeJSONLine( formJSONLine );

			insertDelimiter( formJSONLine, OpenArray );

			writeJSONLine( formJSONLine );

			incrementIndent( );

			break;

		case RootOpening:
			// only print "{ " at indentSize

			insertDelimiter( formJSONLine, OpenObject );

			writeJSONLine( formJSONLine );

			incrementIndent( );

			break;
		default:
			throw "generate output default case tripped" ;

	}


}


void XMLTree_JSON::gatherNodalData( std::vector<std::pair<std::string,
									std::string> >&  nodalPairs )
{

	const Tree_node * currNode = getQueuedNode( CURRENT );
	string content;



	content = currNode->getElementContent( ) ;

	if ( countObjectChildren( ) == 0 ){


		nodalPairs.push_back( make_pair( currNode->getElementName( ), content ) );



	} else{

		if ( content.size( ) > 0 )
			nodalPairs.push_back( make_pair( "content", content ) );





		for ( const auto & Attributes : *( currNode->getPtrAttributes( ) ) ){

			nodalPairs.push_back( make_pair( Attributes.first,
											 Attributes.second ) );
		}
	}



}