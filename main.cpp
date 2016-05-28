/* 
 * File:   main.cpp
 * Author: John Cavalieri
 * John_Cavalieri@student.uml.edu
 * 
 */
#include "XML_Tree.hpp"
#include "XMLelements.hpp"
#include "lineStorage.hpp"
#include "json_translator.hpp"
#include "XMLutils.hpp"
#include <cstdlib>
#include <string>
#include <iostream>
#include <vector>
//each XMLelement allocated is used by stack
//and parse tree
#define STACK 0
#define TREE 1

using namespace std;

/*
 * 
 */


/**
 * destroyStack() is called on runtime parsing error
 * to free any resources tied up in the stack.  
 * @param Stack: stack of pointers that need to be freed
 */
static void destroyStack ( vector<XMLelement*>& Stack )
{
	for ( const auto& Itr: Stack )
		delete Itr;
}


void buildXMLTree ( XMLelement* tree, XMLTree& XMLTree,
					const char*const fileName )
{
	using namespace XMLutil;


	static vector<XMLelement*> elementStack;
	static lineStorage container( fileName );
	static size_t line_number = 0;
	static size_t treeDepth = 0;
	PARSER_STATE state;

	string currLine; // <--> XML_LINE
	string output;
	string content;


	while ( container.getLine( currLine ) ){
		line_number++;


		switch ( state = XMLscanner( currLine, output ) )
		{

			case DIRECTIVE:
			case START_COMMENT:
			case IN_COMMENT:
			case ONELINE_COMMENT:
			case END_COMMENT:
				break;


			case ELEMENT_OPENING_TAG:
			{

				//allocate XMLelement for both stack and tree
				//which will both release their own copy
				XMLelement *
						node[] = {
								new XMLelement( output, line_number, treeDepth ),
								new XMLelement( output, line_number, treeDepth )
				};

				elementStack.push_back( node[STACK] );

				attributeType* record = new attributeType;

				if ( XMLTree.isTreeEmpty( ) ){
					get_elementAttributes( currLine, record );
					node[TREE]->setPtrAttributes( record );
					XMLTree.addTheRoot( node[TREE] );


				} else{

					get_elementAttributes( currLine, record );
					node[TREE]->setPtrAttributes( record );
					XMLTree.addChild( tree, node[TREE] );



				}

				++treeDepth;
				buildXMLTree( node[TREE], XMLTree, fileName );
				break;
			}




			case ELEMENT_CLOSING_TAG:
			{
				XMLelement* PtrElement = elementStack.back( );
				if ( PtrElement->getElementName( ) != output ){
					destroyStack( elementStack );
					throw "Mismatched Element Tags";

				}

				delete PtrElement;
				elementStack.pop_back( );

				--treeDepth;
				return;
			}

			case CONTENT:
			case COMPLETE_ELEMENT:

				if ( get_elementContent( currLine, content ) == false ){
					destroyStack( elementStack );
					throw "Could not extract element content";
				}


				if ( CONTENT == state ){

					//content on its own line implies it belongs
					//to last node created pointed to by tree argument
					tree->setElementContent( content );

				} else{


					//complete element on a single line

					XMLelement*	node =
							new XMLelement( output, line_number, treeDepth, content );

					XMLTree.addChild( tree, node );


				}

				break;


			default:
			case XMLutil::ERROR:

				destroyStack( elementStack );
				throw "Error Processing XML File";


		}

	}


}


int main ( int argc, char** argv )
{


	XMLTree XMLTree;

	const string fileName = "BeatlesXML.xml";


	try{

		buildXMLTree( XMLTree.getRootNode( ), XMLTree, fileName.data( ) );

		XMLTree_JSON* xmlJson;

		XMLTree_JSON::initializeTranslator( NULL, &xmlJson );

		XMLTree::preOrder_traverse( XMLTree, XMLTree_JSON::initializeTranslator );

		xmlJson->translator( );


	}	catch ( const char * error ){
		cerr << "Exception Occurred: " << error << endl;
	}


	return 0;




}

