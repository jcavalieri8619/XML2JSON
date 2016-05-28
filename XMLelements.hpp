/* 
 * File:   XMLelements.hpp
 * Author: John Cavalieri
 * John_Cavalieri@student.uml.edu
 * Created on September 17, 2013, 4:01 PM
 */

#ifndef XMLELEMENTS_HPP
#define	XMLELEMENTS_HPP

#include "XMLutils.hpp"
#include <string>
#include <vector>
#include <utility>


/**
 * representing attributeType as hash map
 * with string key and string value - User
 * can choose other container representation
 * and set typedef accordingly.
 */
typedef XMLutil::attributeType attributeType;


/**
 * class representing an XML element
 * and contains functionality that allows
 * use as a tree node
 */
class XMLelement
{
	/**
	 * content of an XML element
	 */
	std::string elementContent;

	/**
	 * name of an XML element
	 */
	std::string elementName;

	/**
	 * document line number on which XML element occurred
	 */
	std::size_t lineNum;

	/**
	 * depth of element within XML parse tree
	 */
	std::size_t elementDepth;

	/**
	 * pointer to container holding XML element's attribute/s
	 */
	attributeType* attributes;

	/**
	 * container holding pointers to element's children, if any
	 */
	std::vector<XMLelement *> children;




public:

	/**
	 * XMLelement constructor that initializes
	 *  the data members associated with its parameters
	 * @param elementName_ string representing element name
	 * @param elementContent_ string representing element content
	 * @param lineNum_ unsigned number representing line number
	 * @param Ptr_attribute pointer to attribute container
	 */
	XMLelement( std::string elementName_ = "",
			 std::size_t lineNum_ = 0,
			 std::size_t elementDepth_ = 0,
			 std::string elementContent_ = "",
			 attributeType* Ptr_attribute = NULL );


	/**
	 * frees memory associated with attributes and
	 * tree functionality
	 */
	~XMLelement( );



	/**
	 * searches hash map of element attributes
	 * for key parameter 
	 * @param key string - key to search hash map for
	 * @return string - value associated with key else
	 * empty string 
	 */
	std::string searchHashMap( std::string key )const;


	/**
	 * gets depth of XML element within an XML parse tree
	 * @return unsigned number - depth 
	 */
	std::size_t getElementDepth( )const;


	/**
	 * sets the depth of XML element within XML parse tree
	 * @param depth_ unsigned number - depth
	 */
	void setElementDepth( std::size_t depth_ );



	/**
	 * gets the name of the XML element
	 * @return string- element name
	 */
	std::string getElementName( )const;


	/**
	 * gets line number on which XMl element occurred
	 * @return unsigned number - line number 
	 */
	std::size_t getLineNum( )const;


	/**
	 * gets the number of children of XML element
	 * @return unsigned number- number children
	 */
	std::size_t getNumChildren( )const;


	/**
	 * gets the content of XML element
	 * @return string - element content 
	 */
	std::string getElementContent( )const;


	/**
	 * gets reference to hash map holding attributes;
	 * hash map is just one representation of attribute container.
	 * User chooses representation by typedef'ing attributeType
	 * and using method setPtrAttribute() to
	 * @return pointer - attribute hash map
	 */
	attributeType * const getPtrAttributes( )const;




	/**
	 * gets a pointer to the indicated child of
	 * an XMLelement object
	 * @param whatChild unsigned number - what child
	 * @return pointer - whatChild'th child 
	 */
	XMLelement* getChildPtr( size_t whatChild )const;

	/**
	 * sets XML content
	 * @param elementContent_ string - content
	 */
	void setElementContent( std::string elementContent_ );


	/**
	 * sets XML name
	 * @param elementName_ string - name
	 */
	void setElementName( std::string elementName_ );



	/**
	 * sets line number of element
	 * @param lineNum_ unsigned - line number
	 */
	void setLineNum( std::size_t lineNum_ );



	/**
	 * gets the number of attributes contained
	 * within hash map
	 * @return unsigned number - number of attributes
	 */
	std::size_t const getNumAttributes( )const;



	/**
	 * sets pointer to attribute data structure
	 * allowing user to specify custom attribute container.
	 * Default container is hash map.
	 * @param Ptr_attribute pointer - attribute container 
	 * 
	 */
	void setPtrAttributes( attributeType* Ptr_attribute );



	/**
	 * adds a pointer to XMLelement object in children vector
	 * @param Ptr_childElement pointer - XMLelement child
	 */
	void addChildElement( XMLelement * const Ptr_childElement );


	/**
	 * static method that sequentially gets each string describing
	 * the XML element that PtrElement points to.  
	 * @param PtrElement pointer - element whose strings to get
	 * @param output string - representing element information
	 * like name, content, etc...
	 * @return true until all strings have been output for the particular
	 * element 
	 */
	static bool getStrings( XMLelement* PtrElement, std::string& output );


} ;

#endif	/* XMLELEMENTS_HPP */

