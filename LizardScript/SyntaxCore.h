/*!
\file SyntaxCore.h
\brief SyntaxCore class.
\author killer-nyasha
\version 0.2
\date 17.01.2020
*/
#pragma once
#include "interpreter_util.h"
#include "Tokens.h"

namespace LizardScript
{
	//!Contains basic inforamtion about all keywords and special chars.
	class SyntaxCore
	{
	public:
		//!Non-text chars which compiler reads as they were letters
		std::vector<TCHAR> textChars;

		//!Symbols which always split text into different tokens.
		//!for example, `((` - two tokens `(` and `(`, unlike `++` - one token.
		std::vector<TCHAR> breakChars;


		//!all keywords
		std::vector<KeywordToken*> keywords;

		//!simple keywords, unary operators and left brackets. auto-generated by function confirmChanges()
		std::vector<KeywordToken*> keywords_listA;

		//!binary operators and brackets. auto-generated by function confirmChanges()
		std::vector<KeywordToken*> keywords_listB;

		SyntaxCore() 
		{ 
			
		}

		//!deletes all keywords
		~SyntaxCore()
		{
			keywords_listA.clear();
			keywords_listB.clear();
			for (auto& k : keywords)
				delete k;
		}

		//!you have to call this function after any changes in this SyntaxCore before using it in compiler
		void confirmChanges();
	};

	extern SyntaxCore defaultSyntaxCore;
}