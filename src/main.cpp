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