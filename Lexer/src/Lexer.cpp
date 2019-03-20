#include "Lexer.h"



Lexer::Lexer(const std::vector<std::string> &pTokens) : success(true)
{
	for (auto &token : pTokens)
	{
		success = ast.addToken(token);
		if (success == false)
		{
			break;
		}
	}

	if (success)
	{
		success = ast.createAutomatas(initAutomata, nonDeterministicAutomata, automata, invertedAutomata, groupAutomata);
	}
}


Lexer::~Lexer()
{
}

void Lexer::print(std::string & output)
{
	ast.print(output);
	initAutomata.print(output);
	nonDeterministicAutomata.print(output);
	automata.print(output);
	invertedAutomata.print(output);
	groupAutomata.print(output);
}

LexerMatch Lexer::match(const std::string & input, std::size_t offset)
{
	std::size_t lastFinalState = 0;
	std::size_t lastInitialState = 0;
	std::vector<std::size_t> invertedStates;
	std::vector<std::vector<std::size_t>> groups;
	std::vector<std::pair < std::size_t, std::size_t>> submatches;
	std::vector<std::string> matches;

	bool success = processUsingRegularDfa(input, offset, lastInitialState, lastFinalState) &&
		processUsingInvertedDfa(input, offset, lastInitialState, lastFinalState, invertedStates) &&
		processUsingGroupDfa(invertedStates, groups);

	if (success)
	{
		createSubmatches(groups, submatches);
		createMatches(input, offset, lastInitialState, lastFinalState, submatches, matches);
		for (auto it = matches.begin() + 1; it < matches.end(); it++)
		{
			if (it->empty() == false)
			{
				unsigned int groupIndex = unsigned int(it - matches.begin());
				std::vector<unsigned int> groupIndexMap;
				ast.getTokenIndexToGroupIndexMap(groupIndexMap);
				auto tokenGroup = std::lower_bound(
					groupIndexMap.begin(),
					groupIndexMap.end(),
					groupIndex);
				unsigned int tokenIndex = unsigned int(tokenGroup - groupIndexMap.begin());
				if (tokenGroup != groupIndexMap.end() && groupIndexMap[tokenIndex] == groupIndex)
				{
					return LexerMatch(tokenIndex, *it);
				}
			}
		}
	}
	return LexerMatch();
}

LexerMatch Lexer::match(std::istream &input)
{
	std::size_t lastFinalState = 0;
	std::size_t lastInitialState = 0;
	std::vector<std::size_t> invertedStates;
	std::vector<std::vector<std::size_t>> groups;
	std::vector<std::pair < std::size_t, std::size_t>> submatches;
	std::vector<std::string> matches;

	bool success = processUsingRegularDfa(input, lastInitialState, lastFinalState) &&
		processUsingInvertedDfa(input, lastInitialState, lastFinalState, invertedStates) &&
		processUsingGroupDfa(invertedStates, groups);

	if (success)
	{
		createSubmatches(groups, submatches);
		createMatches(input, lastInitialState, lastFinalState, submatches, matches);
		for (auto it = matches.begin() + 1; it < matches.end(); it++)
		{
			if (it->empty() == false)
			{
				unsigned int groupIndex = unsigned int(it - matches.begin());
				std::vector<unsigned int> groupIndexMap;
				ast.getTokenIndexToGroupIndexMap(groupIndexMap);
				auto tokenGroup = std::lower_bound(
					groupIndexMap.begin(),
					groupIndexMap.end(),
					groupIndex);
				unsigned int tokenIndex = unsigned int(tokenGroup - groupIndexMap.begin());
				if (tokenGroup != groupIndexMap.end() && groupIndexMap[tokenIndex] == groupIndex)
				{
					return LexerMatch(tokenIndex, *it);
				}
			}
		}
	}
	return LexerMatch();
}
bool Lexer::processUsingRegularDfa(
	const std::string &input,
	std::size_t offset,
	std::size_t &lastInitialState,
	std::size_t &lastFinalState)
{
	std::size_t startState = automata.getStartState();
	auto finalStates = automata.getFinalStates();
	bool wasFinalState = false;
	automata.returnToStartPosition();
	std::size_t actualState = automata.getActualState();
	lastInitialState = offset;
	lastFinalState = offset;
	for (std::size_t j = offset; j < input.size(); j++)
	{
		if (!automata.returnNextState(input[j], actualState))
		{
			break;
		}
		if (finalStates.find(actualState) != finalStates.end())
		{
			lastFinalState = j;
			wasFinalState = true;
		}
	}
	return wasFinalState;
}


bool Lexer::processUsingRegularDfa(
	std::istream &input,
	std::size_t &lastInitialState,
	std::size_t &lastFinalState)
{
	std::size_t startState = automata.getStartState();
	auto finalStates = automata.getFinalStates();
	bool wasFinalState = false;
	automata.returnToStartPosition();
	std::size_t actualState = automata.getActualState();
	std::size_t offset = input.tellg();
	lastInitialState = offset;
	lastFinalState = offset;
	char character;
	std::size_t currOffset = offset;
	while ((character = (char)input.get()) > '\0')
	{
		if (!automata.returnNextState(character, actualState))
		{
			break;
		}
		if (finalStates.find(actualState) != finalStates.end())
		{
			lastFinalState = currOffset;
			wasFinalState = true;
		}
		currOffset++;
	}
	input.clear();
	input.seekg(lastFinalState, std::ios::beg);
	return wasFinalState;
}


bool Lexer::processUsingInvertedDfa(
	std::istream &input,
	std::size_t lastInitialState,
	std::size_t lastFinalState,
	std::vector<std::size_t> &invertedStates)
{
	std::size_t startState = invertedAutomata.getStartState();
	auto finalStates = invertedAutomata.getFinalStates();
	invertedAutomata.returnToStartPosition();
	std::size_t actualState = invertedAutomata.getActualState();
	invertedStates.push_back(actualState);
	input.seekg(lastFinalState, std::ios::beg);
	for (int i = (int)lastFinalState; i >= (int)lastInitialState; i--)
	{
		if (!invertedAutomata.returnNextState((char)input.peek(), actualState))
		{
			break;
		}
		invertedStates.push_back(actualState);
		input.seekg(-1, std::ios::cur);
	}
	input.seekg(lastFinalState, std::ios::beg);
	return finalStates.find(actualState) != finalStates.end();
}


void Lexer::createMatches(
	std::istream &input,
	std::size_t lastInitialState,
	std::size_t lastFinalState,
	const std::vector<std::pair<std::size_t, std::size_t>> &submatches,
	std::vector<std::string> &matches)
{
	matches.resize(submatches.size() + 1);
	matches[0].resize(lastFinalState - lastInitialState + 1);
	input.clear();
	input.seekg(lastInitialState, std::ios::beg);
	input.read(&matches[0][0], lastFinalState - lastInitialState + 1);
	for (std::size_t i = 0; i < submatches.size(); i++)
	{
		if (submatches[i].first < submatches[i].second)
		{
			matches[i + 1] = matches[0].substr(submatches[i].first, submatches[i].second - submatches[i].first);
		}
	}
}