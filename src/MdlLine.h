#pragma once
#define LINE_EOF                    (-1)
#define LINE_ERROR					0
#define LINE_OPENER					1 // Geoset {
#define LINE_OPENER_LABELED			2 // Model "Foo" {
#define LINE_CLOSER					3 // } or },
#define LINE_FLAG					4 // TwoSided,
#define LINE_LABELED_FLAG			5 // FilterMode None,
#define LINE_LABELED_DATA			6 // BoundsRadius 90.0,
#define LINE_STATIC_LABELED_DATA	7 // static EmissionRate 4,
#define LINE_LABELED_STRING_DATA	8
#define LINE_BRACED_DATA			9 // { 20, 30, 40 },
#define LINE_BRACED_LABELED_DATA	10 // Interval { 0, 3333 },
#define LINE_BRACED_STATIC_DATA		11 // static Color { 1,1,1},
#define LINE_KEY					12 // 100: { 4, 5, 6 },
#define LINE_LONE_INT				13 // 4,
#define LINE_BLANK					14

#define IS_FLOAT					0x00000001
#define IS_INT						0x00000002
#define IS_LABEL					0x00000004
#define IS_CHAR						0x00000008

#pragma warning(disable: 4786)
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

using std::string;
using std::vector;
using std::ifstream;


class MdlLine
{
public:
	int type;

	string chunk;
	string label;


	vector<float> f_listData; bool f_listMatch;
	vector<int> i_listData; bool i_listMatch;

	float f_data; bool f_match;
	int i_data; bool i_match;
	int frame;

	int lineCounter; // If the MdlLine is maintained, this can be
					 // used to keep track of the current line.
	string lineText;

	MdlLine(string);
	MdlLine(ifstream&);
	//MdlLine(ifstream &);
	~MdlLine();
	string getType(void);

	void init(string);
	void init(ifstream&);
private:

	static string trim(string);
	static string getWord(int *, string);
	static bool canBeLabel(string);
	static float canBeFloat(string, bool*);
	static int canBeInt(string, bool*);
	//bool isBracedData(int, vector<int> &);
	bool matchesBracedData(int, vector<int> &, vector<int> &, vector <float> &, vector <char> &);
	static bool isQuoted(string);
};

inline bool MdlLine::isQuoted(string s)
{
	return s[0] == '\"' && s[s.length() - 1] == '\"';
}

inline bool MdlLine::canBeLabel(string s)
{
	return s.length() != 1 || !(s[0] == '{' || s[0] == '}' || s[0] == '\"' || s[0] == ',' || s[0] == ':');
}

inline int MdlLine::canBeInt(string s, bool * success)
{
	unsigned int i;
	auto str_int = new char[s.length() + 1];
	char thisChar;
	for (i = 0; i < s.length(); i++)
	{
		thisChar = s[i];
		if (!((thisChar >= '0' && thisChar <= '9') || (i == 0 && thisChar == '-')))
		{
			(*success) = false;
			delete[]  str_int;
			return 0;
		}
		str_int[i] = thisChar;
	}
	str_int[s.length()] = '\0';

	std::stringstream stringStream(str_int);
	if (!(stringStream >> i))
	{
		(*success) = false;
		delete[]  str_int;
		return 0;
	}

	(*success) = true;
	delete[]  str_int;
	return i;
}


inline float MdlLine::canBeFloat(string s, bool * success)
{
	char* str_float = new char[s.length() + 1];
	char thisChar;
	for (unsigned int i = 0; i < s.length(); i++)
	{
		thisChar = s[i];
		if (!((thisChar >= '0' && thisChar <= '9') || thisChar == '-' || thisChar == '+' || thisChar == 'e' || thisChar == 'E' || thisChar == '.'))
		{
			(*success) = false;
			delete[]  str_float;
			return 0;
		}
		str_float[i] = thisChar;
	}
	str_float[s.length()] = '\0';

	float f;

	std::stringstream ss(str_float);

	if (!(ss >> f))
	{
		(*success) = false;
		delete[]  str_float;
		return 0;
	}

	(*success) = true;
	delete[]  str_float;
	return f;
}


inline void MdlLine::init(ifstream &in)
{
	do {
		if (in.eof())
		{
			type = LINE_EOF;
		}
		else {
			string line;
			getline(in, line);
			init(line);
		}
	} while (type == LINE_BLANK);
}

inline void MdlLine::init(string line)
{
	lineCounter++;

	string lineTrimmed;
	lineTrimmed = trim(line);
	lineText.assign(lineTrimmed);

	//string * word = new string[0];
	vector<string> word(0);
	vector<int> wordType(0);
	vector<int> wordInt(0);
	vector<float> wordFloat(0);
	vector<char> wordChar(0);  // For things like : and ,
	label = "";
	chunk = "";

	int currChar = 0;
	int wordCount = 0;

	while (true) {
		string thisWord = getWord(&currChar, lineTrimmed);
		if (thisWord.length() == 0) break;

		int thisType = 0;
		bool isFloat = false, isInt = false, isLabel = canBeLabel(thisWord), isChar = (thisWord.length() == 1);
		float s_f = canBeFloat(thisWord, &isFloat);
		int s_i = canBeInt(thisWord, &isInt);

		if (isFloat) thisType |= IS_FLOAT;
		if (isInt) thisType |= IS_INT;
		if (isLabel) thisType |= IS_LABEL;
		if (isChar) thisType |= IS_CHAR;

		word.push_back(thisWord);
		wordType.push_back(thisType);
		wordInt.push_back(s_i);
		wordFloat.push_back(s_f);
		wordChar.push_back(isChar ? thisWord[0] : '\0');

		wordCount++;
	}

	/*cout << "Start list:" << endl;
	for (int i=0; i<wordCount; i++) cout << "\t" << word[i] << endl;
	cout << "end list" << endl;*/

	f_listData.clear(); f_listMatch = false;
	i_listData.clear(); i_listMatch = false;

	f_match = false;
	i_match = false;
	f_data = 0; i_data = 0;

	//bool foundFormat=true;

	// Try to match to LINE_BRACED_DATA (
	//matchesBracedData(0, wordType, wordInt, wordFloat, wordChar);

	if (
		wordCount == 0
		) {
		type = LINE_BLANK;
	}
	else if ( // Braced data   { 3, 4, 5 },
		matchesBracedData(0, wordType, wordInt, wordFloat, wordChar)
		) {
		type = LINE_BRACED_DATA;
	}
	else if ( // Frame   4000: { 4,5,6,7 },
		(wordType[0] & IS_INT) == IS_INT &&
		wordChar[1] == ':' &&
		matchesBracedData(2, wordType, wordInt, wordFloat, wordChar)
		) {
		type = LINE_KEY;
		frame = wordInt[0];
	}
	else if ( // Frame   4000: 24.56,
		wordCount == 4 &&
		(wordType[0] & IS_INT) == IS_INT &&
		wordChar[1] == ':' &&
		((wordType[2] & IS_FLOAT) == IS_FLOAT || (wordType[2] & IS_INT) == IS_INT) &&
		wordChar[3] == ','
		) {
		type = LINE_KEY;
		frame = wordInt[0];
		f_listData.push_back(wordFloat[2]);
		i_listData.push_back(wordInt[2]);
		f_listMatch = (wordType[2] & IS_FLOAT) == IS_FLOAT;
		i_listMatch = (wordType[2] & IS_INT) == IS_INT;
	}
	else if ( // Labeled, braced, data   Interval { 4000,6000 },
		(wordType[0] & IS_LABEL) == IS_LABEL &&
		matchesBracedData(1, wordType, wordInt, wordFloat, wordChar)
		) {
		type = LINE_BRACED_LABELED_DATA;
		label.assign(word[0]);
	}
	else if ( // Labeled, braced, static data   static Color { 4000,6000 },
		word[0] == "static" &&
		(wordType[1] & IS_LABEL) == IS_LABEL &&
		matchesBracedData(2, wordType, wordInt, wordFloat, wordChar)
		) {
		type = LINE_BRACED_STATIC_DATA;
		label.assign(word[1]);
	}
	else if ( // Line Closer:  } or },
		(wordCount == 2 && wordChar[0] == '}' && wordChar[1] == ',')
		||
		(wordCount == 1 && wordChar[0] == '}')
		) {
		type = LINE_CLOSER;
	}
	else if ( // Labeled data ( BoundsRadius 300
		wordCount == 3 &&
		(wordType[0] & IS_LABEL) == IS_LABEL &&
		((wordType[1] & IS_FLOAT) == IS_FLOAT || (wordType[1] & IS_INT) == IS_INT) &&
		wordChar[2] == ','
		) {
		type = LINE_LABELED_DATA;
		label.assign(word[0]);
		f_data = wordFloat[1];
		i_data = wordInt[1];
		f_match = (wordType[1] & IS_FLOAT) == IS_FLOAT;
		i_match = (wordType[1] & IS_INT) == IS_INT;
	}
	else if ( // static labeled data
		wordCount == 4 &&
		word[0] == "static" &&
		(wordType[1] & IS_LABEL) == IS_LABEL &&
		((wordType[2] & IS_FLOAT) == IS_FLOAT || (wordType[2] & IS_INT) == IS_INT) &&
		wordChar[3] == ','
		) {
		type = LINE_STATIC_LABELED_DATA;
		label.assign(word[1]);
		f_data = wordFloat[2];
		i_data = wordInt[2];
		f_match = (wordType[2] & IS_FLOAT) == IS_FLOAT;
		i_match = (wordType[2] & IS_INT) == IS_INT;
	}
	else if ( // Labeled string data ( Image "Bleh", )
		wordCount == 3 &&
		(wordType[0] & IS_LABEL) == IS_LABEL &&
		isQuoted(word[1]) &&
		wordChar[2] == ','
		) {
		type = LINE_LABELED_STRING_DATA;
		chunk.assign(word[0]);
		label.assign(word[1].substr(1, word[1].length() - 2));
	}
	else if ( // Opener   Chunk {
		wordCount == 2 &&
		(wordType[0] & IS_LABEL) == IS_LABEL &&
		wordChar[1] == '{'
		) {
		type = LINE_OPENER;
		chunk.assign(word[0]);
	}
	else if ( // Labled opener  Chunk "Label" {
		wordCount == 3 &&
		(wordType[0] & IS_LABEL) == IS_LABEL &&
		isQuoted(word[1]) &&
		wordChar[2] == '{'
		) {
		type = LINE_OPENER_LABELED;
		chunk.assign(word[0]);
		label.assign(word[1].substr(1, word[1].length() - 2));
	}
	else if ( // Numbered opener    Sequences 4 {
		wordCount == 3 &&
		(wordType[0] & IS_LABEL) == IS_LABEL &&
		(wordType[1] & IS_INT) == IS_INT &&
		wordChar[2] == '{'
		) {
		type = LINE_OPENER;
		chunk.assign(word[0]);
	}
	else if ( // Double numbered opener    Faces 1 6 {
		wordCount == 4 &&
		(wordType[0] & IS_LABEL) == IS_LABEL &&
		(wordType[1] & IS_INT) == IS_INT &&
		(wordType[2] & IS_INT) == IS_INT &&
		wordChar[3] == '{'
		) {
		type = LINE_OPENER;
		chunk.assign(word[0]);
	}
	else if ( // 1-d frame  400: 5.6,
		wordCount == 4 &&
		(wordType[0] & IS_INT) == IS_INT &&
		wordChar[1] == ':' &&
		((wordType[2] & IS_FLOAT) == IS_FLOAT || (wordType[2] & IS_INT) == IS_INT) &&
		wordChar[3] == ','
		) {
		// Keys are saved in the vector, even if they are single dimensional
		frame = wordInt[0];
		f_listMatch = (wordType[1] & IS_FLOAT) == IS_FLOAT;
		if (f_listMatch)
		{
			f_listData.clear();
			f_listData.push_back(wordFloat[2]);
		}
		i_listMatch = (wordType[1] & IS_INT) == IS_INT;
		if (i_listMatch)
		{
			i_listData.clear();
			i_listData.push_back(wordInt[2]);
		}
	}
	else if ( // Lone number (for vertex groups)   4,
		wordCount == 2 &&
		(wordType[0] & IS_INT) == IS_INT &&
		wordChar[1] == ','
		) {
		type = LINE_LONE_INT;
		i_match = true;
		i_data = wordInt[0];
	}
	else if ( // Flag  TwoSided,
		wordCount == 2 &&
		(wordType[0] & IS_LABEL) == IS_LABEL &&
		wordChar[1] == ','
		) {
		type = LINE_FLAG;
		label = word[0];
	}
	else if ( // FilterMode None,
		wordCount == 3 &&
		(wordType[0] & IS_LABEL) == IS_LABEL &&
		(wordType[1] & IS_LABEL) == IS_LABEL &&
		wordChar[2] == ','
		) {
		type = LINE_LABELED_FLAG;
		chunk = word[0];
		label = word[1];
	}
	else if ( // DontInherit { Rotation },
		wordCount == 5 &&
		(wordType[0] & IS_LABEL) == IS_LABEL &&
		wordChar[1] == '{' &&
		(wordType[2] & IS_LABEL) == IS_LABEL &&
		wordChar[3] == '}' &&
		wordChar[4] == ','
		) {
		type = LINE_LABELED_FLAG;
		chunk = word[0];
		label = word[2];
	}
	else
	{
		type = LINE_ERROR;
	}

	/*cout << "Type = " << getType() << endl;
	cout << "Chunk = " << chunk << endl;
	cout << "Label = " << label << endl;
	if (f_match) cout << "f_data = " << f_data << endl;
	if (i_match) cout << "i_data = " << i_data << endl;
	if (f_listMatch)
	{
		cout << "f_listData = ";
		for (int i=0; i<f_listData.size(); i++) cout << f_listData[i] << " ";
		cout << endl;
	}
	if (i_listMatch)
	{
		cout << "i_listData = ";
		for (int i=0; i<i_listData.size(); i++) cout << i_listData[i] << " ";
		cout << endl;
	}*/
}

inline MdlLine::MdlLine(string line)
{
	init(line);
}


inline MdlLine::MdlLine(ifstream &in)
{
	init(in);
}

inline MdlLine::~MdlLine()
{

}

inline bool MdlLine::matchesBracedData(int startIndex, vector<int> &wordType, vector<int> &wordInt, vector<float> &wordFloat, vector <char> &wordChar)
{
	f_listData.clear(); f_listMatch = true;
	i_listData.clear(); i_listMatch = true;

	int len = wordType.size();
	if (wordChar[startIndex] != '{' || wordChar[len - 1] != ',' || wordChar[len - 2] != '}')
	{
		f_listMatch = false;
		i_listMatch = false;
		return false;
	}


	for (int i = startIndex + 1; i < len - 2; i += 2)
	{
		if (f_listMatch && (wordType[i] & IS_FLOAT) == IS_FLOAT)
		{
			f_listData.push_back(wordFloat[i]);
		}
		else f_listMatch = false;

		if (i_listMatch && (wordType[i] & IS_INT) == IS_INT)
		{
			i_listData.push_back(wordInt[i]);
		}
		else i_listMatch = false;

		if (wordChar[i + 1] != ((i + 1 == len - 2) ? '}' : ',') || (!f_listMatch && !i_listMatch))
		{
			f_listMatch = i_listMatch = false;
			return false;
		}
	}
	//cout<<wordType[0]<<endl;
	return true;
}

// Returns a string representation of the set type
inline string MdlLine::getType()
{
	switch (type)
	{
	case (LINE_ERROR):
		return "Invalid";
	case (LINE_OPENER):
		return "Chunk-opener";
	case (LINE_OPENER_LABELED):
		return "Labeled chunk-opener";
	case (LINE_CLOSER):
		return "Chunk closer";
	case (LINE_FLAG):
		return "Flag";
	case (LINE_LABELED_DATA):
		if (i_data && !f_data) return "Labeled data (int)";
		else if (!i_data && f_data) return "Labeled data (float)";
		else if (i_data && f_data) return "Labled data (int or float)";
		else return "Labeled data (no data, error)";
	case (LINE_STATIC_LABELED_DATA):
		return "Static labeled data";
	case (LINE_LABELED_STRING_DATA):
		return "Labeled string data";
	case (LINE_BRACED_DATA):
		return "Braced data";
	case (LINE_BRACED_LABELED_DATA):
		return "Braced, labeled data";
	case (LINE_KEY):
		return "Animation key";
	case (LINE_BLANK):
		return "Blank";
	default:
		return "Unknown";
	}
}

inline string MdlLine::getWord(int * startChar, string line)
{
	int currChar = (*startChar);
	int len = line.length();
	if (currChar == len) {
		string s("");
		return s;
	}

	// Skip whitespace until getting to something meaningful
	while (line[currChar] == ' ' || line[currChar] == '\t') currChar++;

	int wordStart = currChar;
	int wordLen = 0;
	while (true) {
		bool endWord = false;
		if (line[currChar] == '{' || line[currChar] == '}' || line[currChar] == '\"' || line[currChar] == ',' || line[currChar] == ':')
		{ // If it is a character that stands alone as a word
			if (wordLen == 0)
			{ // The first thing it ran into was a solo
			  // character, so just return that.
				if (line[currChar] == '\"')
				{
					// Find the next quote
					int nextQuote = line.find_first_of('\"', currChar + 1);
					//cout << "currChar=" << currChar << ", nextQuote=" << nextQuote << endl;
					if (nextQuote == -1) nextQuote = len - 1; // If there's no ending quote, just go to the end. It should end up invalid.

					(*startChar) = nextQuote + 1;
					return line.substr(wordStart, nextQuote - wordStart + 1);
				}
				else
				{
					(*startChar) = currChar + 1;
					char s_chars[2];
					s_chars[0] = line[currChar]; s_chars[1] = '\0';
					string s(s_chars);
					return s;
				}
			}
			else {
				endWord = true;
			}
		}
		if (line[currChar] == ' ' || line[currChar] == '\t' || endWord || currChar == len)
		{ // Because of whitespace or a solo char, the word is done
			string s = line.substr(wordStart, currChar - wordStart);
			(*startChar) = currChar;
			return s;
		}
		wordLen++;
		currChar++;
	}

	// return "Error";
}

inline string MdlLine::trim(string toTrim)
{
	if (toTrim.length() == 0) return "";
	unsigned int trimStart = 0;
	while (trimStart < toTrim.length() && (toTrim[trimStart] == '\t' || toTrim[trimStart] == ' '))
		trimStart++;

	int trimEnd = toTrim.find_first_of("//") - 1;
	if (trimEnd == -1) return "";
	else if (trimEnd < -1) trimEnd = toTrim.length() - 1;
	while ((toTrim[trimEnd] == ' ' || toTrim[trimEnd] == '\t') && trimEnd > trimStart) trimEnd--;

	return toTrim.substr(trimStart, trimEnd - trimStart + 1);
}