#pragma once
#pragma warning(disable: 4786)
#include <string>
#include <vector>
#include "MdlLine.h"

using std::string;
using std::vector;

struct MdlError
{
	vector<string> errorRemark;
	vector<string> errorLine;
	vector<int> errorLineNum;
	bool critical;

	void add(string line, int lineNum, string remark, bool crit = true)
	{
		errorLine.push_back(line);
		errorRemark.push_back(remark);
		errorLineNum.push_back(lineNum);
		critical = crit;
	}

	void add(MdlLine *line, string remark, bool crit = true)
	{
		add(line->lineText, line->lineCounter, remark, crit);
	}

	MdlError() {
		critical = false;
	}
};