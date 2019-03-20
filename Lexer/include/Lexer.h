#pragma once
#include "BaseRegexImpl.h"  
#include "LexerAST.h"
#include "LexerMatch.h"
#include <istream>

class Lexer : BaseRegexImpl
{
public:
	Lexer(const std::vector<std::string> &pTokens);
	~Lexer();

	void print(std::string &output);

	LexerMatch match(const std::string &input, std::size_t offset);

	LexerMatch match(std::istream &stream);

private:

	using BaseRegexImpl::createMatches;
	using BaseRegexImpl::processUsingInvertedDfa;

	bool processUsingRegularDfa(const std::string &input, std::size_t offset, std::size_t & lastInitialState, std::size_t & lastFinalState);

	bool processUsingRegularDfa(std::istream &stream, std::size_t & lastInitialState, std::size_t & lastFinalState);

	bool processUsingInvertedDfa(std::istream &input, std::size_t lastInitialState, std::size_t lastFinalState, std::vector<std::size_t> &invertedStates);

	void createMatches(std::istream &input, std::size_t lastInitialState, std::size_t lastFinalState, const std::vector<std::pair<std::size_t, std::size_t>>& submatches, std::vector<std::string>& matches);

	LexerAST ast;
	bool success;
};

