// MdlxConverterGuhub.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include "MdlxData.h"

int main(int argc, char* argv[])
{
    std::cout << "Hello World!\n";
	vector<string> arguments(argv + 1, argv + argc);

	if (arguments.size() == 0)
		return 0;

	MDLX mdlx;
	auto file = arguments[0];
	if (file.substr(file.length() - 4, 4) == ".mdx") {
		ifstream mdxIn(file, ios::binary);
		
		mdlx.MdxRead(mdxIn);
		mdxIn.close();

		FILE *mdlOut;
		errno_t err = fopen_s(&mdlOut, (file.substr(0, file.length() - 4)+".mdl").c_str(), "w");
		mdlx.MdlWrite(mdlOut);
		fclose(mdlOut);
	}
	else if(file.substr(file.length() - 4, 4) == ".mdl")
	{
		ifstream mdlIn(file);
		mdlx.MdlRead(mdlIn);
		mdlIn.close();

		ofstream mdxOut(file.substr(0, file.length() - 4) + ".mdx", ios::binary);
		mdlx.MdxWrite(mdxOut);
		mdxOut.close();
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
