#include "XML_Tree.hpp"
#include <iostream>
#include <iomanip> 
#include <map>
using namespace std;


XMLTree::XMLTree( ): rootNode( NULL ), nodeCount( 0 )
{

}


STATUS XMLTree::addTheRoot( Tree_node* Ptr_rootNode )
{
	if ( !isTreeEmpty( ) )
		return ERROR;

	incNodeCount( );
	rootNode = Ptr_rootNode;
	return OK;
}


std::size_t XMLTree::getNodeCount( ) const
{
	return nodeCount;
}


void XMLTree::incNodeCount( )
{
	++nodeCount;
}


XMLTree::Tree XMLTree::getRootNode( )const
{
	return rootNode;
}


STATUS XMLTree::addChild( Tree tree, Tree_node* childNode )
{
	if ( isTreeEmpty( tree ) )
		return ERROR;

	incNodeCount( );
	tree->addChildElement( childNode );
	return OK;

}


bool XMLTree::isTreeEmpty( )const
{
	return getRootNode( ) == NULL ? true : false;
}


bool XMLTree::isTreeEmpty( Tree tree )const
{
	return tree == NULL ? true : false;
}


STATUS XMLTree::preOrder_traverse( const XMLTree& treeStructure,
								   STATUS ( * proc )( const Tree, XMLTree_JSON** ) ,
								   Tree currNode )
{

	//traversing empty tree is a no op
	if ( treeStructure.isTreeEmpty( ) )
		return OK;


	STATUS st;
	//curNode is only NULL on first call where currNode has its default value
	//after this call the tree will be traversed
	if ( ! currNode ){

		preOrder_traverse( treeStructure, proc, treeStructure.getRootNode( ) );
	} else{

		if ( proc )
			proc( currNode , NULL );


		size_t childItr;


		for ( childItr = 0; childItr < currNode->getNumChildren( ); ++childItr ){

			st = preOrder_traverse( treeStructure, proc,
								 currNode->getChildPtr( childItr ) );
		}
	}
	return st;


}


void XMLTree::destroyTree( Tree tree )
{



	size_t childItr;

	for ( childItr = 0; childItr < tree->getNumChildren( ); ++childItr ){

		destroyTree( tree->getChildPtr( childItr ) );
	}

	//deleting tree: XMLelement* calls XMLelement dtor
	//ensuring XMLelement's resources are properly released
	delete tree;
	tree = NULL;


}


STATUS XMLTree::printTree( Tree tree )
{
	if ( !tree )
		return ERROR;

	size_t depth = tree->getElementDepth( );
	string output;

	while ( XMLelement::getStrings( tree, output ) ){
		cout << setfill( '*' ) << setw( depth + output.size( ) )
				<< right << output << endl;
	}
	cout << setfill( '*' ) << setw( depth + output.size( ) )
			<< right << output << endl;

	cout << endl;
	return OK;


}


XMLTree::~XMLTree( )
{

	destroyTree( getRootNode( ) );

}