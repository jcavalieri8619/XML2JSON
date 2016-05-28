/* 
 * File:   XMLelements.cpp
 * Author: John Cavalieri 
 * 
 * Created on September 17, 2013, 4:01 PM
 */

#include "XMLelements.hpp"
#include <string>
#include <map>
#include <typeinfo>
#include <algorithm>

using namespace std;


XMLelement::XMLelement ( std::string elementName_,
						 std::size_t lineNum_,
						 std::size_t elementDepth_,
						 std::string elementContent_,
						 attributeType * Ptr_attribute ):
		elementContent ( elementContent_ ), elementName ( elementName_ ),
		lineNum ( lineNum_ ), elementDepth ( elementDepth_ ), attributes ( Ptr_attribute )
{

}


XMLelement::~XMLelement ( )
{
	delete attributes;


}


std::string XMLelement::searchHashMap ( std::string key )const
{
	if ( typeid (*getPtrAttributes ( ) ) != typeid (XMLutil::attributeType ) )
		throw "searchHashMap() requires map<string,string> attributeType";

	if ( !getPtrAttributes ( ) || getNumAttributes ( ) == 0 )
		return string ( " " );

	auto mapItr = getPtrAttributes ( )->find ( key );

	if ( mapItr == getPtrAttributes ( )->end ( ) )
		return string ( " " );

	return mapItr->second;
}


std::string XMLelement::getElementName ( )const
{
	return elementName;

}


std::size_t XMLelement::getElementDepth ( )const
{
	return elementDepth;
}


void XMLelement::setElementDepth ( std::size_t depth_ )
{
	elementDepth = depth_;
}


std::size_t XMLelement::getLineNum ( )const
{
	return lineNum;
}


std::size_t const XMLelement::getNumAttributes ( )const
{
	return (getPtrAttributes ( ) == NULL ) ? 0 : getPtrAttributes ( )->size ( );
}

//the pointer within class cannot be modified by this function
//but the hash map may be modified - 


attributeType * const XMLelement::getPtrAttributes ( )const
{
	return attributes;
}


std::string XMLelement::getElementContent ( )const
{
	return elementContent;
}


std::size_t XMLelement::getNumChildren ( )const
{
	return children.size ( );
}


void XMLelement::setElementName ( std::string elementName_ )
{
	elementName = elementName_;
}


void XMLelement::setElementContent ( std::string elementContent_ )
{
	elementContent += elementContent_;


	//efficient newline remove from string courtesy of
	//stackoverflow.com/questions/1488775/c-remove-new-line-from-multiline-string

	elementContent.erase( std::remove( elementContent.begin( ),
									 elementContent.end( ), '\n' ),
						 elementContent.end( ) );
}


void XMLelement::setLineNum ( std::size_t lineNum_ )
{
	lineNum = lineNum_;
}


void XMLelement::setPtrAttributes ( attributeType* Ptr_attribute )
{
	attributes = Ptr_attribute;

}


void XMLelement::addChildElement ( XMLelement * const Ptr_childElement )
{
	children.push_back ( Ptr_childElement );

}


XMLelement* XMLelement::getChildPtr ( size_t whatChild )const
{
	return whatChild < getNumChildren ( ) ? children[whatChild] : NULL;
}


bool XMLelement::getStrings ( XMLelement* PtrElement, std::string& output )
{
	static size_t infoItr = 0;

	switch ( infoItr % 4 )
	{

		case 0:
			output = "Name: " + PtrElement->getElementName ( );
			infoItr++;

			return true;
		case 1:
			output = "Content: " + PtrElement->getElementContent ( );
			infoItr++;

			return true;
		case 2:
			output = "LineNumber: " + to_string ( PtrElement->getLineNum ( ) );
			infoItr++;

			return true;
		case 3:

			output = "Attributes: ";

			if ( !PtrElement->getPtrAttributes ( ) ){
				infoItr++;
				return false;

			} else{

				for ( const auto & AttributeMap: *( PtrElement->getPtrAttributes ( ) ) ){

					output += AttributeMap.first + "=" + AttributeMap.second;

				}
				infoItr++;
				return false;
			}
	}
	return false;
}

