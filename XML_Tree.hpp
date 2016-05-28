/* 
 * File:   XML_Tree.hpp
 * Author: jcavalie
 *
 * Created on October 19, 2013, 8:43 AM
 */

#ifndef XML_TREE_HPP
#define	XML_TREE_HPP

#include "XMLelements.hpp"
#include "XMLutils.hpp"

#include "globals.hpp"



class XMLTree_JSON;


/**
 *XMLTree class builds and manages
 * tree structure where each node is
 * an XMLelement. Tree structure mimics
 * hierarchical structure of the XML document
 * used to generate each XMLelement 
 */
class XMLTree
{
public:
	typedef XMLelement Tree_node, * Tree;


	/**
	 * tree constructor that initializes 
	 * tree to empty
	 */
	XMLTree( );



	/**
	 * adds a child node/element to tree pointed to by
	 * first argument
	 * @param tree: pointer to node whose child is being added
	 * @param childNode: node being added as child 
	 */
	STATUS addChild( Tree tree, Tree_node* childNode );



	/**
	 * adds root node to an empty tree
	 * @param Ptr_rootNode: root of the tree
	 */
	STATUS addTheRoot( Tree_node* Ptr_rootNode );



	/**
	 * gets a pointer to the root of the tree
	 * @return: pointer to root node
	 */
	Tree getRootNode( )const;



	/**
	 * determines whether tree is empty or not
	 * by testing root for NULL
	 * @return: true if empty; else false 
	 */
	bool isTreeEmpty( )const;



	/**
	 * overload - determines whether tree pointed
	 * to by tree parameter is empty
	 * @param tree: pointer - tree 
	 * @return: true if tree is empty; else false 
	 */
	bool isTreeEmpty( Tree tree )const;


	/**
	 * traverses tree pointed to by the first parameter
	 * in a pre-order fashion. The procedure is applied
	 * to each node in the tree
	 * @param tree: pointer - tree to be traversed
	 * @param proc: procedure object applied to each
	 * node within tree
	 */

	static STATUS preOrder_traverse( const XMLTree& treeStructure,
									 STATUS ( * proc )( const Tree, XMLTree_JSON** ),
									 Tree currNode = NULL );


	/**
	 * prints tree node pointed to by tree parameter using toString method
	 * of the tree node.  Each node is indented to match its depth
	 * within parse tree. Passing this function to preOrder_traverse
	 * prints entire tree, not just the node pointed to by tree parameter
	 * @param tree: pointer - node to be printed via node's toString method
	 */
	static STATUS printTree( Tree tree );



	/**
	 * destructor releases resources used
	 * in building parse tree; calls utility
	 * function destroyTree 
	 */
	~XMLTree( );



	/**
	 * gets the number of nodes within
	 * the tree at any given time. 
	 */
	std::size_t getNodeCount( ) const;








private:

	/**
	 * Increments the current node count
	 * for each call to addRoot() and addChild()
	 * methods
	 */
	void incNodeCount( );


	/**
	 * pointer to the root of the entire tree
	 */
	Tree rootNode;


	/**
	 * The number of nodes within the tree
	 * at any given time.  
	 */
	std::size_t nodeCount;




	/**
	 * recursively deletes each node within tree
	 * @param tree: pointer to tree to be destroyed
	 */
	void destroyTree( Tree tree );



} ;



#endif	/* XML_TREE_HPP */

