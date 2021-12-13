// WordChecker.cpp
//
// ICS 46 Winter 2021
// Project #4: Set the Controls for the Heart of the Sun
//
// Replace and/or augment the implementations below as needed to meet
// the requirements.

#include "WordChecker.hpp"
#include <iostream>


WordChecker::WordChecker(const Set<std::string>& words)
    : words{words}
{
}


bool WordChecker::wordExists(const std::string& word) const
{
    return words.contains(word);
}


std::vector<std::string> WordChecker::findSuggestions(const std::string& word) const
{
    // Nonsensical code because the compiler requires the member variables
    // 'words' to be used somewhere, or else it becomes a warning (which
    // turns into an error).
    std::vector<std::string> result;
    std::vector<std::string> wordFromSwap = swapAdjacent(word);
    std::vector<std::string> wordFromInsert = insertAdjacent(word);
    std::vector<std::string> wordFromDelete = deleteCharacter(word);
    std::vector<std::string> wordFromReplace = replaceCharacter(word);
    std::vector<std::string> wordFromSplit = splitAdjacent(word);
    for(int i = 0; i < wordFromSwap.size(); i++)
    {
        std::vector<std::string>::iterator s;
        s = std::find(result.begin(), result.end(), wordFromSwap[i]);
        if(s == result.end())
        {
            result.push_back(wordFromSwap[i]);
        }
    }
    for(int i = 0; i < wordFromInsert.size(); i++)
    {
        std::vector<std::string>::iterator s;
        s = std::find(result.begin(), result.end(), wordFromInsert[i]);
        if(s == result.end())
        {
            result.push_back(wordFromInsert[i]);
        }
    }
    for(int i = 0; i < wordFromDelete.size(); i++)
    {
        std::vector<std::string>::iterator s;
        s = std::find(result.begin(), result.end(), wordFromDelete[i]);
        if(s == result.end())
        {
            result.push_back(wordFromDelete[i]);
        }
    }
    for(int i = 0; i < wordFromReplace.size(); i++)
    {
        std::vector<std::string>::iterator s;
        s = std::find(result.begin(), result.end(), wordFromReplace[i]);
        if(s == result.end())
        {
            result.push_back(wordFromReplace[i]);
        }
    }
    for(int i = 0; i < wordFromSplit.size(); i++)
    {
        std::vector<std::string>::iterator s;
        s = std::find(result.begin(), result.end(), wordFromSplit[i]);
        if(s == result.end())
        {
            result.push_back(wordFromSplit[i]);
        }
    }
    return result;
}

std::vector<std::string> WordChecker::swapAdjacent(const std::string& word) const
{
    std::vector<std::string> result;
    for(int i = 0; i < word.length() - 1; i++)
    {
        std::string temp = word.substr(0, i) + word.at(i + 1) + word.at(i) + word.substr(i + 2, word.length() - i - 2);
        if(wordExists(temp))
        {
            result.push_back(temp);
        }
    }
    return result;
}

std::vector<std::string> WordChecker::insertAdjacent(const std::string& word) const
{
    std::vector<std::string> letter = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", 
                                       "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", 
                                       "Y", "Z"};
    std::vector<std::string> result;
    for(int i = 0; i < word.length(); i++)
    {
        for(int j = 0; j < 26; j++)
        {
            std::string temp = word.substr(0, i) + letter[j] + word.substr(i, word.length() - i);
            if(wordExists(temp))
            {
                result.push_back(temp);
            }
        }
    }
    for(int j = 0; j < 26; j++)
    {
        std::string temp = word + letter[j];
        if(wordExists(temp))
        {
            result.push_back(temp);
        }
    }
    return result;
}

std::vector<std::string> WordChecker::deleteCharacter(const std::string& word) const
{
    std::vector<std::string> result;
    for(int i = 0; i < word.length(); i++)
    {
        std::string temp = word.substr(0, i) + word.substr(i + 1, word.length() - i - 1);
        if(wordExists(temp))
        {
            result.push_back(temp);
        }
    }
    return result;
}

std::vector<std::string> WordChecker::replaceCharacter(const std::string& word) const
{
    std::vector<std::string> letter = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", 
                                       "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", 
                                       "Y", "Z"};
    std::vector<std::string> result;
    for(int i = 0; i < word.length(); i++)
    {
        for(int j = 0; j < 26; j++)
        {
            std::string temp = word.substr(0, i) + letter[j] + word.substr(i + 1, word.length() - i - 1);
            if(wordExists(temp))
            {
                result.push_back(temp);
            }
        }
    }
    return result;
}

std::vector<std::string> WordChecker::splitAdjacent(const std::string& word) const
{
    std::vector<std::string> result;
    for(int i = 0; i < word.length(); i++)
    {
        std::string temp0 = word.substr(0, i);
        std::string temp1 = word.substr(i, word.length() - i);
        if(wordExists(temp0) && wordExists(temp1))
        {
            std::string temp = temp0 + " " + temp1;
            result.push_back(temp);
        }
    }
    return result;
}