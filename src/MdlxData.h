#ifndef MDLXDATA_H
#define MDLXDATA_H


/*

 Done-ness   R=read, W=write, x=mdl, l=mdl
 ---------
							 Rx  Wx  Rl  Wl
[VERS]--|--Version           X   X   X   X
[MODL]--|--Model             X   X   X   X
[SEQS]--|--Sequences         X   X   X   X
[GLBS]--|--GlobalSequences   X   X   X   X
[MTLS]--|--Material          X   X   X   X
[TEXS]--|--Textures          X   X   X   X
[TXAN]--|--TextureAnims      X   X   X   X
[GEOS]--|--Geoset            X   X   X   X
[GEOA]--|--GeosetAnim        X   X   X   X
[BONE]--|--Bone              X   X   X   X
[LITE]--|--Light             X   X   X   X
[HELP]--|--Helper            X   X   X   X
[ATCH]--|--Attachment        X   X   X   X
[PIVT]--|--PivotPoints       X   X   X   X
[PREM]--|--ParticleEmitter
[PRE2]--|--ParticleEmitter2  X   X   X   X
[RIBB]--|--RibbonEmitter     X   X   X   X
[CAMS]--|--Cameras           X   X   X   X
[EVTS]--|--EventObject       X   X   X   X
[CLID]-----CollisionShape    X   X   X   X

*/


#define FILTER_NONE 0
#define FILTER_TRANSPARENT 1
#define FILTER_BLEND 2
#define FILTER_ADDITIVE 3
#define FILTER_ADDALPHA 4
#define FILTER_MODULATE 5
#define FILTER_MODULATE2X 6 // Todo: test this. It is a guess

#define WRAP_WIDTH 0x00000001
#define WRAP_HEIGHT 0x00000002

#define LINE_DONTINTERP 0
#define LINE_LINEAR 1
#define LINE_HERMITE 2
#define LINE_BEZIER 3

#define LOOPING_ON 0
#define LOOPING_OFF 1

// OBJ flags
#define DONT_INHERIT_TRANSLATION 1
#define DONT_INHERIT_SCALING 2
#define DONT_INHERIT_ROTATION 4
#define BILLBOARDED 8
#define BILLBOARDED_X 16
#define BILLBOARDED_Y 32
#define BILLBOARDED_Z 64
#define CAMERA_ANCHORED 128

#define TYPE_BONE 256
#define TYPE_LIGHT 512
#define TYPE_EVENTOBJECT 1024
#define TYPE_ATTACHMENT 2048
#define TYPE_COLLISIONSHAPE 8192
#define TYPE_RIBBON_EMITTER 0 // TODO: Find this!
#define TYPE_PARTICLE_EMITTER 4096

#define LIGHT_OMNIDIRECTIONAL 0
#define LIGHT_DIRECTIONAL 1
#define LIGHT_AMBIENT 2

#define SHAPE_BOX 0
#define SHAPE_SPHERE 2

#define PREM_USES_MDL 0x00008000
#define PREM_USES_TGA 0x00010000

#define PRE2_ALWAYS 0x00001000 // always shows up
#define PRE2_DONT_INHERIT_ROTATION 0x00000004
#define PRE2_UNSHADED 0x00009000
#define PRE2_UNFOGGED 0x00040000
#define PRE2_XY_QUAD 0x00100000
#define PRE2_LINE_EMITTER 0x00020000
#define PRE2_MODEL_SPACE 0x00080000
#define PRE2_SORT_PRIMS_FAR_Z 0x00010000

#define PRE2_FILTER_BLEND 0
#define PRE2_FILTER_ADDITIVE 1
#define PRE2_FILTER_MODULATE 2
#define PRE2_FILTER_ALPHAKEY 4

#define TAGINT(t)	(t[0]|(t[1]<<8)|(t[2]<<16)|(t[3]<<24))
#define MDLREAD(type, holder)	type newOb; newOb.MdlRead(in, line, err); holder.push_back(newOb);
#define MDXREAD(type, holder)	int count = MdxReadInt(in); int end = (int)in.tellg() + count; while (in.tellg()<end) {	type newOb; newOb.MdxRead(in); holder.push_back(newOb);	} assert((int)in.tellg()==end);



#include <cstdarg>
#include <fstream>
#include <vector>
#include <cassert>
#include "MdlLine.h"
#include "MdlError.h"
#include <ctime>
#include <iostream>
#include <sstream>

using std::ifstream;
using std::ofstream;
using std::vector;
using std::string;
using std::cout;
using std::endl;
using std::ios;

class Chunk {
public:
#ifdef TRANSFORM
	float* m_mover(float* dest, float x, float y, float z)
	{
		memset(dest, 0, 16 * sizeof(float));
		dest[0] = dest[5] = dest[10] = dest[15] = 1;
		dest[3] = x;
		dest[7] = y;
		dest[11] = z;
		return dest;
	}
	void m_mul(float* dest, float* a, float* b)
	{
		dest[0] = a[0] * b[0] + a[1] * b[4] + a[2] * b[8] + a[3] * b[12];
		dest[1] = a[0] * b[1] + a[1] * b[5] + a[2] * b[9] + a[3] * b[13];
		dest[2] = a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + a[3] * b[14];
		dest[3] = a[0] * b[3] + a[1] * b[7] + a[2] * b[11] + a[3] * b[15];

		dest[4] = a[4] * b[0] + a[5] * b[4] + a[6] * b[8] + a[7] * b[12];
		dest[5] = a[4] * b[1] + a[5] * b[5] + a[6] * b[9] + a[7] * b[13];
		dest[6] = a[4] * b[2] + a[5] * b[6] + a[6] * b[10] + a[7] * b[14];
		dest[7] = a[4] * b[3] + a[5] * b[7] + a[6] * b[11] + a[7] * b[15];

		dest[8] = a[8] * b[0] + a[9] * b[4] + a[10] * b[8] + a[11] * b[12];
		dest[9] = a[8] * b[1] + a[9] * b[5] + a[10] * b[9] + a[11] * b[13];
		dest[10] = a[8] * b[2] + a[9] * b[6] + a[10] * b[10] + a[11] * b[14];
		dest[11] = a[8] * b[3] + a[9] * b[7] + a[10] * b[11] + a[11] * b[15];

		dest[12] = a[12] * b[0] + a[13] * b[4] + a[14] * b[8] + a[15] * b[12];
		dest[13] = a[12] * b[1] + a[13] * b[5] + a[14] * b[9] + a[15] * b[13];
		dest[14] = a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14];
		dest[15] = a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15];
	}
	void m_ident(float *dest)
	{
		dest[0] = dest[5] = dest[10] = dest[15] = 1;
		dest[1] = dest[2] = dest[3] = dest[4] = 0;
		dest[6] = dest[7] = dest[8] = dest[9] = 0;
		dest[11] = dest[12] = dest[13] = dest[14] = 0;
	}
	float* m_copy(float *dest, float *src)
	{
		for (int i = 0; i < 16; i++) dest[i] = src[i];
		return dest;
	}
#endif

	void MdxWriteShort(ofstream &out, short val)
	{
		out.write(reinterpret_cast<char*>(&val), 2);
	}

	void MdxWriteInt(ofstream &out, int val)
	{
		out.write(reinterpret_cast<char*>(&val), 4);
	}

	// Write an arbitrary number of ints
	void MdxWriteInts(ofstream &out, int count, ...)
	{
		va_list arguments;
		va_start(arguments, count);
		for (int i = 0; i < count; i++) {
			MdxWriteInt(out, va_arg(arguments, int));
		}
		va_end(arguments);
	}

	void MdxWriteFloat(ofstream &out, float val)
	{
		out.write(reinterpret_cast<char*>(&val), 4);
	}

	// Write an arbitrary number of floats
	void MdxWriteFloats(ofstream &out, int count, ...)
	{
		va_list arguments;
		va_start(arguments, count);
		for (int i = 0; i < count; i++) {
			MdxWriteFloat(out, (float)va_arg(arguments, double));
		}
		va_end(arguments);
	}

	void MdxWriteByte(ofstream &out, unsigned char val)
	{
		out.write(reinterpret_cast<char*>(&val), 1);
	}

	void MdxWriteString(ofstream &out, string val, int len)
	{
		char* c = new char[len];
		for (int i = 0; i < len; i++) {
			if (i < val.length())
				c[i] = val.c_str()[i];
			else
				c[i] = 0;
		}
		out.write(c, len);
		delete[] c;
	}

	void MdxReadInt(ifstream &in, int *val)
	{
		in.read(reinterpret_cast<char*>(val), 4);
	}

	int MdxReadInt(ifstream &in)
	{
		int val;
		in.read(reinterpret_cast<char*>(&val), 4);
		return val;
	}

	int MdxReadShort(ifstream &in)
	{
		short val;
		in.read(reinterpret_cast<char*>(&val), 2);
		return (int)val;
	}

	int MdxReadByte(ifstream &in)
	{
		char c;
		in.get(c);
		return (int)c;
	}

	void MdxReadFloat(ifstream &in, float *val)
	{
		in.read(reinterpret_cast<char*>(val), 4);
	}

	float MdxReadFloat(ifstream &in)
	{
		float val;
		in.read(reinterpret_cast<char*>(&val), 4);
		return val;
	}

	void MdxReadFloat(ifstream &in, int count, ...)
	{
		va_list arguments;
		va_start(arguments, count);
		for (int i = 0; i < count; i++) {
			in.read(reinterpret_cast<char*>(va_arg(arguments, float*)), 4);
			//MdxWriteFloat(out, (float)va_arg ( arguments, double ));
		}
		va_end(arguments);
	}

	string MdxReadString(ifstream &in, int len)
	{
		char* c = new char[len];
		in.read(c, len);
		return string(c);
	}

	// Case-insensitive match. Put constants in lcase to speed this up.
	bool strmatch(string s1, string s2) {
		if (s1.size() != s2.size()) return false;
		char c1, c2;
		for (int i = s1.size() - 1; i >= 0; i--) {
			c1 = s1[i]; if (c1 >= 'A' && c1 <= 'Z') c1 += 'a' - 'A';
			c2 = s2[i]; if (c2 >= 'A' && c2 <= 'Z') c2 += 'a' - 'A';
			if (c1 != c2) return false;
		}
		return true;
	}
};

class GenericKeySet :public Chunk {
public:
	int lineType; // (0:don't interp;1:linear;2:hermite;3:bezier)
	int globalSeqId; // 0xFFFFFFFF if none

	string TypeName()
	{
		switch (lineType)
		{
		case (LINE_DONTINTERP): return "DontInterp";
		case (LINE_LINEAR): return "Linear";
		case (LINE_HERMITE): return "Hermite";
		case (LINE_BEZIER): return "Bezier";
		default: return "ERROR-UNKNOWN";
		}
	}

	int NameVal(string val)
	{
		if (strmatch(val, "dontinterp")) return LINE_DONTINTERP;
		else if (strmatch(val, "linear")) return LINE_LINEAR;
		else if (strmatch(val, "hermite")) return LINE_HERMITE;
		else if (strmatch(val, "bezier")) return LINE_BEZIER;
		else return -1;
	}
};

class KMTA_KEY { // TextureID animation
public:
	int frame;
	float state;
	float inTan, outTan; // If lineType>1

	void set(KMTA_KEY key) {
		frame = key.frame;
		state = key.state;
		inTan = key.inTan;
		outTan = key.outTan;
	}
};

class KMTA :public GenericKeySet { // Alpha animation
public:
	vector <KMTA_KEY> key;
	float staticState;

	KMTA() {
		globalSeqId = -1;
		staticState = 0;
		lineType = LINE_LINEAR;
	}

	void MdxRead(ifstream &in)
	{
		int keyCount = MdxReadInt(in);
		lineType = MdxReadInt(in);
		if (lineType > LINE_BEZIER || lineType < LINE_DONTINTERP)
		{
			lineType = LINE_LINEAR;
			assert(false);
		}
		globalSeqId = MdxReadInt(in);
		for (int i = 0; i < keyCount; i++) {
			KMTA_KEY newKey;
			newKey.frame = MdxReadInt(in);
			newKey.state = MdxReadFloat(in);
			if (lineType > LINE_LINEAR)
			{
				newKey.inTan = MdxReadFloat(in);
				newKey.outTan = MdxReadFloat(in);
			}
			key.push_back(newKey);
		}
	}

	int ByteCount() {
		if (key.size() == 0) return 0;

		if (lineType > 1) return 16 + key.size() * 16; // There are InTans and OutTans for each key
		else return 16 + key.size() * 8;
	}

	void MdxWrite(ofstream &out, const char* tag)
	{
		if (key.size() == 0) return;

		out.write(tag, 4);
		MdxWriteInt(out, key.size());
		MdxWriteInt(out, lineType);
		MdxWriteInt(out, globalSeqId);

		for (auto x:key) {
			MdxWriteInt(out, x.frame);
			MdxWriteFloat(out, x.state);
			if (lineType > 1) {
				MdxWriteFloat(out, x.inTan);
				MdxWriteFloat(out, x.outTan);
			}
		}
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		line->init(in); // Type
		if (line->type != LINE_FLAG) {
			err->add(line->lineText, line->lineCounter, "Expected interpolation type.");
			return;
		}
		else {
			lineType = NameVal(line->label);
			if (lineType == -1) // Unrecognized
				err->add(line->lineText, line->lineCounter, "Unrecognized interpolation type. Should be DontInterp, Linear, Hermite, or Bezier.");
		}

		line->init(in);
		if (line->type == LINE_LABELED_DATA && strmatch(line->label, "globalseqId") && line->i_match) {
			globalSeqId = line->i_data;
			line->init(in);
		}

		while (line->type != LINE_EOF && line->type != LINE_CLOSER) {
			if (line->type != LINE_KEY || line->f_listData.size() != 1) {
				err->add(line->lineText, line->lineCounter, "Expected a one-dimensional key.");
				return;
			}
			KMTA_KEY newKey;
			newKey.state = line->f_listData[0];
			newKey.frame = line->frame;
			if (lineType > LINE_LINEAR)
			{ // Tangent time!
				line->init(in);
				if (line->type == LINE_LABELED_DATA && strmatch(line->label, "intan") && line->f_match) {
					newKey.inTan = line->f_data;
				}
				else {
					err->add(line->lineText, line->lineCounter, "Expected InTan");
					return;
				}
				line->init(in);
				if (line->type == LINE_LABELED_DATA && strmatch(line->label, "outtan") && line->f_match) {
					newKey.outTan = line->f_data;
				}
				else {
					err->add(line->lineText, line->lineCounter, "Expected OutTan");
					return;
				}
			}

			key.push_back(newKey);
			line->init(in);
		}
	}

	void MdlWrite(FILE *out, string name, string tab)
	{
		if (key.size() == 0) { // static
			fprintf(out, "%sstatic %s %f,\n", tab.c_str(), name.c_str(), staticState);
		}
		else {

			fprintf(out, "%s%s %d {\n", tab.c_str(), name.c_str(), key.size());
			fprintf(out, "%s\t%s,\n", tab.c_str(), TypeName().c_str());

			if (globalSeqId != -1)
				fprintf(out, "%s\tGlobalSeqId %d,\n", tab.c_str(), globalSeqId);

			for (auto x:key)
			{
				fprintf(out, "%s\t%d: %f,\n", tab.c_str(), x.frame, x.state);
				if (lineType > LINE_LINEAR)
				{
					fprintf(out, "%s\t\tInTan %f,\n", tab.c_str(), x.inTan);
					fprintf(out, "%s\t\tOutTan %f,\n", tab.c_str(), x.outTan);
				}
			}

			fprintf(out, "%s}\n", tab.c_str());
		}
	}
};

class KMTF_KEY {
public:
	int frame;
	int state;
	float inTan, outTan; // If lineType>1

	void set(KMTF_KEY key) {
		frame = key.frame;
		state = key.state;
		inTan = key.inTan;
		outTan = key.outTan;
	}
};

class KMTF :public GenericKeySet {
public:
	vector <KMTF_KEY> key;
	int staticState;

	KMTF() {
		globalSeqId = -1;
		staticState = 0;
	}

	void MdxRead(ifstream &in)
	{
		int keyCount = MdxReadInt(in);
		lineType = MdxReadInt(in);
		if (lineType > LINE_BEZIER || lineType < LINE_DONTINTERP)
		{
			lineType = LINE_LINEAR;
			assert(false);
		}
		globalSeqId = MdxReadInt(in);
		for (int i = 0; i < keyCount; i++) {
			KMTF_KEY newKey;
			MdxReadInt(in, &newKey.frame);
			newKey.state = MdxReadInt(in);
			if (lineType > LINE_LINEAR)
			{
				assert(false);
			}
			key.push_back(newKey);
		}
	}

	int ByteCount() {
		if (key.size() == 0) return 0;

		if (lineType > 1) return 16 + key.size() * 16; // There are InTans and OutTans for each key
		else return 16 + key.size() * 8;
	}

	void MdxWrite(ofstream &out, const char* tag)
	{
		if (key.size() == 0) return;

		out.write(tag, 4);
		MdxWriteInt(out, key.size());
		MdxWriteInt(out, lineType);
		MdxWriteInt(out, globalSeqId);

		for (auto x:key) {
			MdxWriteInt(out, x.frame);
			MdxWriteInt(out, x.state);
			if (lineType > 1) {
				MdxWriteFloat(out, x.inTan);
				MdxWriteFloat(out, x.outTan);
			}
		}
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		line->init(in); // Type
		if (line->type != LINE_FLAG) {
			err->add(line->lineText, line->lineCounter, "Expected interpolation type.");
			return;
		}
		else {
			lineType = NameVal(line->label);
			if (lineType == -1) // Unrecognized
				err->add(line->lineText, line->lineCounter, "Unrecognized interpolation type. Should be DontInterp, Linear, Hermite, or Bezier.");
			if (lineType > LINE_LINEAR)
				err->add(line->lineText, line->lineCounter, "No such interpolation for int values.");
		}

		line->init(in);
		if (line->type == LINE_LABELED_DATA && strmatch(line->label, "globalseqId") && line->i_match) {
			globalSeqId = line->i_data;
			line->init(in);
		}

		while (line->type != LINE_EOF && line->type != LINE_CLOSER) {
			if (line->type != LINE_KEY || line->f_listData.size() != 1) {
				err->add(line->lineText, line->lineCounter, "Expected a one-dimensional key.");
				return;
			}
			KMTF_KEY newKey;
			newKey.state = line->f_listData[0];
			newKey.frame = line->frame;
			key.push_back(newKey);
			line->init(in);
		}
	}

	void MdlWrite(FILE *out, string name, string tab)
	{
		if (key.size() == 0) { // static
			fprintf(out, "%sstatic %s %d,\n", tab.c_str(), name.c_str(), staticState);
		}
		else {
			fprintf(out, "%s%s %d {\n", tab.c_str(), name.c_str(), key.size());
			fprintf(out, "%s\t%s,\n", tab.c_str(), TypeName().c_str());

			if (globalSeqId != -1)
				fprintf(out, "%s\tGlobalSeqId %d,\n", tab.c_str(), globalSeqId);

			for (auto x:key)
			{
				fprintf(out, "%s\t%d: %d,\n", tab.c_str(), x.frame, x.state);
			}

			fprintf(out, "%s}\n", tab.c_str());
		}
	}
};

class KGSC_KEY {
public:
	int frame;
	float x, y, z;
	float inTanx, inTany, inTanz, outTanx, outTany, outTanz; // If lineType>1

	void set(KGSC_KEY key) {
		frame = key.frame;
		x = key.x; y = key.y; z = key.z;
		inTanx = key.inTanx; inTany = key.inTany; inTanz = key.inTanz;
		outTanx = key.outTanx; outTany = key.outTany; outTanz = key.inTanz;
	}

	KGSC_KEY()
	{
		x = y = z = 0;
		inTanx = inTany = inTanz = 0;
		outTanx = outTany = outTanz = 0;
		frame = 0;
	}
};

class KGSC :public GenericKeySet {
public:
	vector <KGSC_KEY> key;
	float staticX, staticY, staticZ;
	bool reverseOrder;

	KGSC() {
		globalSeqId = -1;
		staticX = staticY = staticZ = 0;
		lineType = LINE_LINEAR;
		reverseOrder = false;
	}

	void MdxRead(ifstream &in)
	{
		int keyCount = MdxReadInt(in);
		lineType = MdxReadInt(in);
		if (lineType > LINE_BEZIER || lineType < LINE_DONTINTERP)
		{
			lineType = LINE_LINEAR;
			assert(false);
		}
		globalSeqId = MdxReadInt(in);
		for (int i = 0; i < keyCount; i++) {
			KGSC_KEY newKey;
			newKey.frame = MdxReadInt(in);
			newKey.x = MdxReadFloat(in);
			newKey.y = MdxReadFloat(in);
			newKey.z = MdxReadFloat(in);
			/*if (reverseOrder)
			{
				float temp = newKey.x;
				newKey.x = newKey.z;
				newKey.z = temp;
			}*/
			if (lineType > LINE_LINEAR)
			{
				newKey.inTanx = MdxReadFloat(in);
				newKey.inTany = MdxReadFloat(in);
				newKey.inTanz = MdxReadFloat(in);
				newKey.outTanx = MdxReadFloat(in);
				newKey.outTany = MdxReadFloat(in);
				newKey.outTanz = MdxReadFloat(in);
				/*if (reverseOrder)
				{
					float temp = newKey.inTanx;
					newKey.inTanx = newKey.inTanz;
					newKey.inTanz = temp;
					temp = newKey.outTanx;
					newKey.outTanx = newKey.outTanz;
					newKey.outTanz = temp;
				}*/
			}


			key.push_back(newKey);
		}
	}

	void MdlWrite(FILE *out, string name, string tab)
	{
		if (key.size() == 0) {
			fprintf(out, "%sstatic %s { %f, %f, %f },\n", tab.c_str(), name.c_str(), staticX, staticY, staticZ);
		}
		else {
			fprintf(out, "%s%s %d {\n", tab.c_str(), name.c_str(), key.size());
			fprintf(out, "%s\t%s,\n", tab.c_str(), TypeName().c_str());

			if (globalSeqId != -1)
				fprintf(out, "%s\tGlobalSeqId %d,\n", tab.c_str(), globalSeqId);

			for (auto x: key)
			{
				fprintf(out, "%s\t%d: { %f, %f, %f },\n", tab.c_str(), x.frame, x.x, x.y, x.z);
				if (lineType > LINE_LINEAR)
				{
					fprintf(out, "%s\t\tInTan { %f, %f, %f },\n", tab.c_str(), x.inTanx, x.inTany, x.inTanz);
					fprintf(out, "%s\t\tOutTan { %f, %f, %f },\n", tab.c_str(), x.outTanx, x.outTany, x.outTanz);
				}
			}

			fprintf(out, "%s}\n", tab.c_str());
		}
	}

	void MdxWrite(ofstream &out, const char* tag)
	{
		if (key.size() == 0) return;

		out.write(tag, 4);
		MdxWriteInt(out, key.size());
		MdxWriteInt(out, lineType);
		MdxWriteInt(out, globalSeqId);

		for (auto x:key) {
			MdxWriteInt(out, x.frame);
			/*if (reverseOrder)
			{
				MdxWriteFloats(out, 3, key[i].z, key[i].y, key[i].x);
				if (lineType>LINE_LINEAR) {
					MdxWriteFloats(out, 6,
						key[i].inTanz, key[i].inTany, key[i].inTanx,
						key[i].outTanz, key[i].outTany, key[i].outTanx );
				}
			}
			else
			{*/
			MdxWriteFloats(out, 3, x.x, x.y, x.z);
			if (lineType > LINE_LINEAR) {
				MdxWriteFloats(out, 6,
					x.inTanx, x.inTany, x.inTanz,
					x.outTanx, x.outTany, x.outTanz);
			}
			//}
		}
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		line->init(in); // Type
		if (line->type != LINE_FLAG) {
			err->add(line->lineText, line->lineCounter, "Expected interpolation type.");
			return;
		}
		else {
			lineType = NameVal(line->label);
			if (lineType == -1) // Unrecognized
				err->add(line->lineText, line->lineCounter, "Unrecognized interpolation type. Should be DontInterp, Linear, Hermite, or Bezier.");
		}

		line->init(in);
		if (line->type == LINE_LABELED_DATA && strmatch(line->label, "globalseqid") && line->i_match) {
			globalSeqId = line->i_data;
			line->init(in);
		}

		while (line->type != LINE_EOF && line->type != LINE_CLOSER) {
			if (line->type != LINE_KEY || line->f_listData.size() != 3) {
				err->add(line->lineText, line->lineCounter, "Expected a three-dimensional key.");
				return;
			}
			KGSC_KEY newKey;
			newKey.frame = line->frame;
			newKey.x = line->f_listData[0];
			newKey.y = line->f_listData[1];
			newKey.z = line->f_listData[2];

			if (lineType > LINE_LINEAR)
			{ // Tangent time!
				line->init(in);
				if (line->type == LINE_BRACED_LABELED_DATA && strmatch(line->label, "intan") && line->f_listMatch && line->f_listData.size() == 3) {
					newKey.inTanx = line->f_listData[0];
					newKey.inTany = line->f_listData[1];
					newKey.inTanz = line->f_listData[2];
				}
				else {
					err->add(line->lineText, line->lineCounter, "Expected InTan");
					return;
				}
				line->init(in);
				if (line->type == LINE_BRACED_LABELED_DATA && strmatch(line->label, "outtan") && line->f_listMatch && line->f_listData.size() == 3) {
					newKey.outTanx = line->f_listData[0];
					newKey.outTany = line->f_listData[1];
					newKey.outTanz = line->f_listData[2];
				}
				else {
					err->add(line->lineText, line->lineCounter, "Expected OutTan");
					return;
				}
			}

			key.push_back(newKey);
			line->init(in);
		}
	}

	int ByteCount() {
		if (key.size() == 0)
			return 0;
		else {
			if (lineType > 1) return 16 + key.size() * 40; // 40 = 3 dimensions  *  3 main/in/out  * 4 bytes each + frame
			else return 16 + key.size() * 16;
		}
	}

#ifdef TRANSFORM
	float mat[16];

	void MoveInterp(int type, float *x, float *y, float *z,
		float x1, float y1, float z1, float x1out, float y1out, float z1out,
		float x2, float y2, float z2, float x2in, float y2in, float z2in,
		float t)
	{
		// TODO: implement other than linear
		float it = 1.0 - t;
		if (type == LINE_LINEAR)
		{

			*x = x1 * it + x2 * t;
			*y = y1 * it + y2 * t;
			*z = z1 * it + z2 * t;
		}
		else if (type == LINE_BEZIER || type == LINE_HERMITE)
		{
			float b0, b1, b2, b3;
			if (type == LINE_BEZIER)
			{
				b3 = t * t*t;
				b2 = 3 * t*t*it;
				b1 = 3 * t*it*it;
				b0 = it * it*it;
			}
			else
			{
				b0 = 2 * t*t*t - 3 * t*t + 1;
				b1 = t * t*t - 2 * t*t + t;
				b3 = -2 * t*t*t + 3 * t*t;
				b2 = t * t*t - t * t;;
			}

			// calculate the x,y and z of the curve point by summing
			// the Control vertices weighted by their respective blending
			// functions
			//
			*x = b0 * x1 +
				b1 * x1out +
				b2 * x2in +
				b3 * x2;

			*y = b0 * y1 +
				b1 * y1out +
				b2 * y2in +
				b3 * y2;

			*z = b0 * z1 +
				b1 * z1out +
				b2 * z2in +
				b3 * z2;
		}

	}

	float tx, ty, tz;
	void Transform(int frame
#ifdef DEBUG
		, ofstream &out
#endif
	)
	{
		//float X,Y,Z;
		int keyCount = key.size();
		if (keyCount == 0 || frame < key[0].frame) {
			tx = 0; ty = 0; tz = 0;
		}
		else if (frame >= key[keyCount - 1].frame) {
			tx = key[keyCount - 1].x;
			ty = key[keyCount - 1].y;
			tz = key[keyCount - 1].z;
		}
		else {
#ifdef DEBUG
			out << "Interpolating" << endl;
#endif

			int afterKey = 0;
			for (afterKey = 1; afterKey < key.size(); afterKey++)
			{
				if (frame <= key[afterKey].frame)
					break;
			}
			int beforeKey = afterKey - 1;

			float step = (float)(frame - key[beforeKey].frame) / (float)(key[afterKey].frame - key[beforeKey].frame);
			MoveInterp(lineType, &tx, &ty, &tz,
				key[beforeKey].x, key[beforeKey].y, key[beforeKey].z,
				key[beforeKey].outTanx, key[beforeKey].outTany, key[beforeKey].outTanz,
				key[afterKey].x, key[afterKey].y, key[afterKey].z,
				key[afterKey].inTanx, key[afterKey].inTany, key[afterKey].inTanz,
				step);
#ifdef DEBUG
			out << "between " << key[beforeKey].x << ", " << key[beforeKey].y << ", " << key[beforeKey].z << " and " << key[afterKey].x << ", " << key[afterKey].y << ", " << key[afterKey].z << endl;
			out << "step = " << step << " to " << tx << ", " << ty << ", " << tz << endl;
#endif
		}

		m_mover(mat, tx, ty, tz);

	}
#endif
};

class KGRT_KEY {
public:
	int frame;
	float x, y, z, w;
	float inTanx, inTany, inTanz, inTanw, outTanx, outTany, outTanz, outTanw; // If lineType>1

	void set(KGRT_KEY key) {
		frame = key.frame;
		x = key.x; y = key.y; z = key.z; w = key.w;
		inTanx = key.inTanx; inTany = key.inTany; inTanz = key.inTanz; inTanw = key.inTanw;
		outTanx = key.outTanx; outTany = key.outTany; outTanz = key.outTanz; outTanw = key.outTanw;
	}
};

class KGRT :public GenericKeySet {
public:
	vector<KGRT_KEY> key;

#ifdef TRANSFORM
	float mat[16];
	float tx, ty, tz, tw;

	// see: http://www.cse.ucsc.edu/~pang/160/f98/Gems/GemsIII/quatspin.c
	void slerp(float *result, float *a, float *b, float t)
	{
		float w1, w2;
		bool bflip;

		float cosTheta = a[1] * b[1] + a[2] * b[2] + a[3] * b[3] + a[0] * b[0]; // dot product
		if (bflip = (cosTheta < 0.0)) {
			cosTheta = -cosTheta;
		}

		float theta = (float)acos(cosTheta);
		float sinTheta = (float)sin(theta);

		if (sinTheta > 0.0f)
		{
			w1 = float(sin((1.0f - t)*theta) / sinTheta);
			w2 = float(sin(t*theta) / sinTheta);
		}
		else {
			// CQuat a == CQuat b
			w1 = 1.0f;
			w2 = 0.0f;
		}

		if (bflip) //added
			w2 = -w2;

		result[1] = w1 * a[1] + w2 * b[1];
		result[2] = w1 * a[2] + w2 * b[2];
		result[3] = w1 * a[3] + w2 * b[3];
		result[0] = w1 * a[0] + w2 * b[0];
	}


	void Transform(int frame
#ifdef DEBUG
		, ofstream &out
#endif
	)
	{
		//	float tx,ty,tz,tw;
		int keyCount = key.size();
		if (keyCount == 0 || frame < key[0].frame) {
			tx = 0; ty = 0; tz = 0; tw = 1;
		}
		else if (frame >= key[keyCount - 1].frame) {
			tx = key[keyCount - 1].x;
			ty = key[keyCount - 1].y;
			tz = key[keyCount - 1].z;
			tw = key[keyCount - 1].w;
		}
		else {
			//out << "Interpolating" << endl;
			float before[4], after[4], between[4];

			int afterKey = 0;
			for (afterKey = 1; afterKey < key.size(); afterKey++)
			{
				if (frame <= key[afterKey].frame)
					break;
			}
			int beforeKey = afterKey - 1;
			//TODO: determine those
			before[0] = key[beforeKey].w;
			before[1] = key[beforeKey].x;
			before[2] = key[beforeKey].y;
			before[3] = key[beforeKey].z;
			after[0] = key[afterKey].w;
			after[1] = key[afterKey].x;
			after[2] = key[afterKey].y;
			after[3] = key[afterKey].z;
			float step = (float)(frame - key[beforeKey].frame) / (float)(key[afterKey].frame - key[beforeKey].frame);
			//out << "Step is " << step << " between " << beforeKey << " (frame " << key[beforeKey].frame << ") and " << afterKey << " (frame " << key[afterKey].frame << ")" << endl;
			slerp(between, before, after, step);
			tw = between[0];
			tx = between[1];
			ty = between[2];
			tz = between[3];

		}

		float xx = tx * tx;
		float xy = tx * ty;
		float xz = tx * tz;
		float xw = tx * tw;
		float yy = ty * ty;
		float yz = ty * tz;
		float yw = ty * tw;
		float zz = tz * tz;
		float zw = tz * tw;
		mat[0] = 1 - 2 * (yy + zz);
		mat[1] = 2 * (xy - zw);
		mat[2] = 2 * (xz + yw);
		mat[4] = 2 * (xy + zw);
		mat[5] = 1 - 2 * (xx + zz);
		mat[6] = 2 * (yz - xw);
		mat[8] = 2 * (xz - yw);
		mat[9] = 2 * (yz + xw);
		mat[10] = 1 - 2 * (xx + yy);
		mat[3] = mat[7] = mat[11] = mat[12] = mat[13] = mat[14] = 0;
		mat[15] = 1;

	}
#endif

	KGRT() {
		globalSeqId = -1;
		lineType = LINE_LINEAR;
	}

	void MdxRead(ifstream &in)
	{
		int keyCount = MdxReadInt(in);
		lineType = MdxReadInt(in);
		if (lineType > LINE_BEZIER || lineType < LINE_DONTINTERP)
		{
			lineType = LINE_LINEAR;
			assert(false);
		}
		globalSeqId = MdxReadInt(in);
		for (int i = 0; i < keyCount; i++) {
			KGRT_KEY newKey;
			newKey.frame = MdxReadInt(in);
			newKey.x = MdxReadFloat(in);
			newKey.y = MdxReadFloat(in);
			newKey.z = MdxReadFloat(in);
			newKey.w = MdxReadFloat(in);
			if (lineType > LINE_LINEAR)
			{
				newKey.inTanx = MdxReadFloat(in);
				newKey.inTany = MdxReadFloat(in);
				newKey.inTanz = MdxReadFloat(in);
				newKey.inTanw = MdxReadFloat(in);
				newKey.outTanx = MdxReadFloat(in);
				newKey.outTany = MdxReadFloat(in);
				newKey.outTanz = MdxReadFloat(in);
				newKey.outTanw = MdxReadFloat(in);
			}
			key.push_back(newKey);
		}

	}

	int ByteCount() {
		if (key.size() == 0)
			return 0;
		else {
			if (lineType > 1) return 16 + key.size() * 52; // 4 dimensions  *  3 main/in/out  * 4 bytes each + frame(4b)
			else return 16 + key.size() * 20;
		}
	}

	void MdlWrite(FILE *out, string name, string tab)
	{
		if (key.size() == 0) return;

		fprintf(out, "%s%s %d {\n", tab.c_str(), name.c_str(), key.size());
		fprintf(out, "%s\t%s,\n", tab.c_str(), TypeName().c_str());
		if (globalSeqId != -1)
			fprintf(out, "%s\tGlobalSeqId %d,\n", tab.c_str(), globalSeqId);
		for (auto x: key)
		{
			fprintf(out, "%s\t%d: { %f, %f, %f, %f },\n", tab.c_str(), x.frame, x.x, x.y, x.z, x.w);
			if (lineType > LINE_LINEAR)
			{
				fprintf(out, "%s\t\tInTan { %f, %f, %f, %f },\n", tab.c_str(), x.inTanx, x.inTany, x.inTanz, x.inTanw);
				fprintf(out, "%s\t\tOutTan { %f, %f, %f, %f },\n", tab.c_str(), x.outTanx, x.outTany, x.outTanz, x.outTanw);
			}
		}

		fprintf(out, "%s}\n", tab.c_str());
	}

	void MdxWrite(ofstream &out, const char* tag)
	{
		if (key.size() == 0) return;

		out.write(tag, 4);
		MdxWriteInt(out, key.size());
		MdxWriteInt(out, lineType);
		MdxWriteInt(out, globalSeqId);

		for (auto x: key) {
			MdxWriteInt(out, x.frame);
			MdxWriteFloats(out, 4, x.x, x.y, x.z, x.w);
			if (lineType > 1) {
				MdxWriteFloats(out, 8,
					x.inTanx, x.inTany, x.inTanz, x.inTanw,
					x.outTanx, x.outTany, x.outTanz, x.outTanw);
			}
		}
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		line->init(in); // Type
		if (line->type != LINE_FLAG) {
			err->add(line->lineText, line->lineCounter, "Expected interpolation type.");
			return;
		}
		else {
			lineType = NameVal(line->label);
			if (lineType == -1) // Unrecognized
				err->add(line->lineText, line->lineCounter, "Unrecognized interpolation type. Should be DontInterp, Linear, Hermite, or Bezier.");
		}

		line->init(in);
		if (line->type == LINE_LABELED_DATA && strmatch(line->label, "globalseqid") && line->i_match) {
			globalSeqId = line->i_data;
			line->init(in);
		}

		while (line->type != LINE_EOF && line->type != LINE_CLOSER) {
			if (line->type != LINE_KEY || line->f_listData.size() != 4) {
				err->add(line->lineText, line->lineCounter, "Expected a four-dimensional key.");
				return;
			}
			KGRT_KEY newKey;
			newKey.frame = line->frame;
			newKey.x = line->f_listData[0];
			newKey.y = line->f_listData[1];
			newKey.z = line->f_listData[2];
			newKey.w = line->f_listData[3];

			if (lineType > LINE_LINEAR)
			{ // Tangent time!
				line->init(in);
				if (line->type == LINE_BRACED_LABELED_DATA && strmatch(line->label, "intan") && line->f_listMatch && line->f_listData.size() == 4) {
					newKey.inTanx = line->f_listData[0];
					newKey.inTany = line->f_listData[1];
					newKey.inTanz = line->f_listData[2];
					newKey.inTanw = line->f_listData[3];
				}
				else {
					err->add(line->lineText, line->lineCounter, "Expected InTan");
					return;
				}
				line->init(in);
				if (line->type == LINE_BRACED_LABELED_DATA && strmatch(line->label, "outtan") && line->f_listMatch && line->f_listData.size() == 4) {
					newKey.outTanx = line->f_listData[0];
					newKey.outTany = line->f_listData[1];
					newKey.outTanz = line->f_listData[2];
					newKey.outTanw = line->f_listData[3];
				}
				else {
					err->add(line->lineText, line->lineCounter, "Expected OutTan");
					return;
				}
			}

			key.push_back(newKey);
			line->init(in);
		}
	}
};

class PIVT : public Chunk
{
public:
	float x, y, z;
	PIVT() {
		x = y = z = 0;
	}

	void MdlWrite(FILE *out, string tab)
	{
		fprintf(out, "%s{ %f, %f, %f },\n", tab.c_str(), x, y, z);
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		x = line->f_listData[0];
		y = line->f_listData[1];
		z = line->f_listData[2];
	}

	void MdxRead(ifstream &in)
	{
		x = MdxReadFloat(in);
		y = MdxReadFloat(in);
		z = MdxReadFloat(in);
	}

	void MdxWrite(ofstream &out)
	{
		MdxWriteFloat(out, x);
		MdxWriteFloat(out, y);
		MdxWriteFloat(out, z);
	}
};


class MODL :public Chunk {
public:
	string	name;				//(0x150 bytes)
	float	boundsRadius;
	float	minExtx, minExty, minExtz;
	float	maxExtx, maxExty, maxExtz;
	int		blendTime;

	void MdlWrite(FILE *out)
	{
		fprintf(out, "Model \"%s\" {\n", name.c_str());
		fprintf(out, "\tBlendTime %d,\n", blendTime);
		if (minExtx || minExty || minExtz || maxExtx || maxExty || maxExtz)
		{
			fprintf(out, "\tMinimumExtent { %f, %f, %f },\n", minExtx, minExty, minExtz);
			fprintf(out, "\tMaximumExtent { %f, %f, %f },\n", maxExtx, maxExty, maxExtz);
			if (boundsRadius != 0.0f)
				fprintf(out, "\tBoundsRadius %f,\n", boundsRadius);
		}
		fprintf(out, "}\n");
	}

	void MdxWriteStringTagged(ofstream &out, string val, int len)
	{
		char* c;
		c = new char[len];
		for (int i = 0; i < len; i++) {
			if (i < val.length())
				c[i] = val.c_str()[i];
			else
				c[i] = 0;
		}

		if (val.length() < len - 5)
		{
			c[len - 4] = 'P';
			c[len - 3] = 'D';
			c[len - 2] = 'R';
		}

		out.write(c, len);


		delete[] c;
	}

	void MdxWrite(ofstream &out)
	{
		out.write("MODL", 4);

		MdxWriteInt(out, 372); // chunk len
		MdxWriteStringTagged(out, name, 336);
		MdxWriteInt(out, 0); // unknown
		MdxWriteFloats(out, 7,
			boundsRadius,
			minExtx, minExty, minExtz,
			maxExtx, maxExty, maxExtz);
		MdxWriteInt(out, blendTime);
	}

	void MdxRead(ifstream &in)
	{
		int size;
		MdxReadInt(in, &size); // length
		name = MdxReadString(in, 336);
		MdxReadInt(in, &size); // throwaway
		MdxReadFloat(in, 7, &boundsRadius, &minExtx, &minExty, &minExtz, &maxExtx, &maxExty, &maxExtz);
		MdxReadInt(in, &blendTime);

		//cout << size << endl;
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		name = line->label;

		while (line->type != LINE_EOF) {
			line->init(in);
			if (line->type == LINE_BRACED_LABELED_DATA && line->f_listMatch && strmatch(line->label, "maximumextent")) {
				maxExtx = line->f_listData[0];
				maxExty = line->f_listData[1];
				maxExtz = line->f_listData[2];
			}
			else if (line->type == LINE_BRACED_LABELED_DATA && line->f_listMatch && strmatch(line->label, "minimumextent")) {
				minExtx = line->f_listData[0];
				minExty = line->f_listData[1];
				minExtz = line->f_listData[2];
			}
			else if (line->type == LINE_LABELED_DATA && line->f_match && strmatch(line->label, "boundsradius"))
				boundsRadius = line->f_data;
			else if (line->type == LINE_LABELED_DATA && line->i_match && strmatch(line->label, "blendtime"))
				blendTime = line->i_data;
			else if (line->type == LINE_LABELED_DATA && (
				strmatch(line->label, "numgeosets") ||
				strmatch(line->label, "numgeosetanims") ||
				strmatch(line->label, "numbones") ||
				strmatch(line->label, "numhelpers") ||
				strmatch(line->label, "numlights") ||
				strmatch(line->label, "numattachments") ||
				strmatch(line->label, "numparticleemitters") ||
				strmatch(line->label, "numparticleemitters2") ||
				strmatch(line->label, "numribbonemitters") ||
				strmatch(line->label, "numevents")
				)) {
				// Ignore
			}
			else if (line->type == LINE_CLOSER)
				return;
			else {
				err->add(line->lineText, line->lineCounter, "Unexpected in a Model chunk");
			}
		}
	}

	MODL() {
		name = "";
		boundsRadius = 0;
		minExtx = minExty = minExtz = 0;
		maxExtx = maxExty = maxExtz = 0;
		blendTime = 100;
	}
};

class TEX :public Chunk {
public:
	int	replaceableID;
	string	texturePath;		//(0x100 bytes)
	int	wrapping;		//(1:WrapWidth;2:WrapHeight;3:Both)

	TEX()
	{
		replaceableID = 0;
		texturePath = "";
		wrapping = 0;
	}

	void MdlWrite(FILE *out)
	{
		fprintf(out, "\tBitmap {\n");
		fprintf(out, "\t\tImage \"%s\",\n", texturePath.c_str());
		if (replaceableID != 0)
			fprintf(out, "\t\tReplaceableId %d,\n", replaceableID);
		if ((wrapping&WRAP_WIDTH) == WRAP_WIDTH)
			fprintf(out, "\t\tWrapWidth,\n");
		if ((wrapping&WRAP_HEIGHT) == WRAP_HEIGHT)
			fprintf(out, "\t\tWrapHeight,\n");
		fprintf(out, "\t}\n");
	}

	void MdxWrite(ofstream &out)
	{
		MdxWriteInt(out, replaceableID);
		MdxWriteString(out, texturePath, 256);
		MdxWriteInt(out, 0);
		MdxWriteInt(out, wrapping);
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		while (true) {
			line->init(in);
			if (line->type == LINE_LABELED_STRING_DATA && strmatch(line->chunk, "image")) {
				texturePath = line->label;
			}
			else if (line->type == LINE_LABELED_DATA && line->i_match && strmatch(line->label, "replaceableid"))
				replaceableID = line->i_data;
			else if (line->type == LINE_FLAG && strmatch(line->label, "wrapwidth"))
				wrapping |= 1;
			else if (line->type == LINE_FLAG && strmatch(line->label, "wrapheight"))
				wrapping |= 2;
			else if (line->type == LINE_CLOSER || line->type == LINE_EOF)
				return;
			else {
				err->add(line->lineText, line->lineCounter, "Unexpected in a Bitmap chunk");
			}
		}
	}

	void MdxRead(ifstream &in)
	{
		MdxReadInt(in, &replaceableID);
		texturePath = MdxReadString(in, 256);
		MdxReadInt(in);
		MdxReadInt(in, &wrapping);
	}
};

class MTL_LAY :public Chunk
{
public:
	int filterMode;		//(0:none;1:transparent;2:blend;3:additive;4:addalpha;5:modulate)
	int shading;		//+1:unshaded;+2:SphereEnvMap;+16:twosided;
	//int textureID;      //  +32:unfogged;+64:NoDepthTest;+128:NoDepthSet)
	int tVertexAnimId;	// 0xFFFFFFFF if none
	int coordId;
	//float alpha;	
	KMTA kmta; // alpha
	KMTF kmtf; // textureID

	MTL_LAY()
	{
		filterMode = FILTER_NONE;
		//textureID = 0;
		shading = 0;
		coordId = 0;
		tVertexAnimId = -1;
		kmta.staticState = 1;
		//alpha = 1;
	}

	void MdlWrite(FILE *out)
	{
		fprintf(out, "\t\tLayer {\n");
		fprintf(out, "\t\t\tFilterMode %s,\n", MdlFilterModeName().c_str());
		if ((shading & 1) == 1) fprintf(out, "\t\t\tUnshaded,\n");
		if ((shading & 2) == 2) fprintf(out, "\t\t\tSphereEnvMap,\n");
		if ((shading & 16) == 16) fprintf(out, "\t\t\tTwoSided,\n");
		if ((shading & 32) == 32) fprintf(out, "\t\t\tUnfogged,\n");
		if ((shading & 64) == 64) fprintf(out, "\t\t\tNoDepthTest,\n");
		if ((shading & 128) == 128) fprintf(out, "\t\t\tNoDepthSet,\n");

		kmtf.MdlWrite(out, "TextureID", "\t\t\t");
		if (tVertexAnimId != -1)
			fprintf(out, "\t\t\tTVertexAnimId %d,\n", tVertexAnimId);
		if (kmta.key.size() != 0 || kmta.staticState != 1)
			kmta.MdlWrite(out, "Alpha", "\t\t\t");

		fprintf(out, "\t\t}\n");
	}

	void MdxWrite(ofstream &out)
	{
		MdxWriteInts(out, 6, ByteCount(), filterMode, shading,
			kmtf.staticState, tVertexAnimId, coordId);

		MdxWriteFloat(out, kmta.staticState);
		kmta.MdxWrite(out, "KMTA");
		kmtf.MdxWrite(out, "KMTF");
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		while (line->type != LINE_EOF) {
			line->init(in);
			if (line->type == LINE_LABELED_FLAG && strmatch(line->chunk, "filtermode") && strmatch(line->label, "none")) {
				filterMode = FILTER_NONE;
			}
			else if (line->type == LINE_LABELED_FLAG && strmatch(line->chunk, "filtermode") && strmatch(line->label, "transparent")) {
				filterMode = FILTER_TRANSPARENT;
			}
			else if (line->type == LINE_LABELED_FLAG && strmatch(line->chunk, "filtermode") && strmatch(line->label, "blend")) {
				filterMode = FILTER_BLEND;
			}
			else if (line->type == LINE_LABELED_FLAG && strmatch(line->chunk, "filtermode") && strmatch(line->label, "additive")) {
				filterMode = FILTER_ADDITIVE;
			}
			else if (line->type == LINE_LABELED_FLAG && strmatch(line->chunk, "filtermode") && strmatch(line->label, "addalpha")) {
				filterMode = FILTER_ADDALPHA;
			}
			else if (line->type == LINE_LABELED_FLAG && strmatch(line->chunk, "filtermode") && strmatch(line->label, "modulate")) {
				filterMode = FILTER_MODULATE;
			}
			else if (line->type == LINE_LABELED_FLAG && strmatch(line->chunk, "filtermode") && strmatch(line->label, "modulate2x")) {
				filterMode = FILTER_MODULATE2X;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "unshaded")) {
				shading |= 1;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "sphereenvmap")) {
				shading |= 2;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "twosided")) {
				shading |= 16;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "unfogged")) {
				shading |= 32;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "nodepthtest")) {
				shading |= 64;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "nodepthset")) {
				shading |= 128;
			}
			else if (line->type == LINE_LABELED_DATA && strmatch(line->label, "tvertexanimid") && line->i_match) {
				tVertexAnimId = line->i_data;
			}
			else if (line->type == LINE_LABELED_DATA && strmatch(line->label, "coordid") && line->i_match) {
				coordId = line->i_data;
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "textureid") && line->i_match) {
				kmtf.staticState = line->i_data;
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "alpha") && line->f_match) {
				kmta.staticState = line->f_data;
			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "textureid")) {
				kmtf.MdlRead(in, line, err);
			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "alpha")) {
				kmta.MdlRead(in, line, err);
			}
			else if (line->type == LINE_CLOSER)
				return;
			else {
				err->add(line->lineText, line->lineCounter, "Unexpected in a Layer chunk");
			}
		}
	}

	void MdxRead(ifstream &in)
	{
		int start = in.tellg();
		int end = start + MdxReadInt(in);
		filterMode = MdxReadInt(in);
		shading = MdxReadInt(in);
		kmtf.staticState = MdxReadInt(in);
		tVertexAnimId = MdxReadInt(in);
		coordId = MdxReadInt(in);
		kmta.staticState = MdxReadFloat(in);

		char tag[5];
		tag[4] = 0;
		while (in.tellg() < end)
		{
			in.read(tag, 4);
			if (strcmp(tag, "KMTA") == 0)
				kmta.MdxRead(in);
			else if (strcmp(tag, "KMTF") == 0)
				kmtf.MdxRead(in);
			else { assert(false); }
		}
	}

	int ByteCount()
	{
		return (28 + kmta.ByteCount() + kmtf.ByteCount());
	}
private:
	// Converts teh current filterMode to its MDL equivalent
	string MdlFilterModeName()
	{
		switch (filterMode)
		{
		case(FILTER_NONE): return "None";
		case (FILTER_TRANSPARENT): return "Transparent";
		case (FILTER_BLEND): return "Blend";
		case (FILTER_ADDITIVE): return "Additive";
		case (FILTER_ADDALPHA): return "AddAlpha";
		case (FILTER_MODULATE): return "Modulate";
		case (FILTER_MODULATE2X): return "Modulate2x";
		default: return "ERROR-UNKNOWN";
		}
	}
};

class MTL : public Chunk // Material
{
public:
	int priorityPlane;
	int renderMode;
	vector <MTL_LAY> lays;

	MTL()
	{
		priorityPlane = 0;
		renderMode = 0;
	}

	void MdlWrite(FILE *out)
	{
		fprintf(out, "\tMaterial {\n");
		if ((renderMode & 1) == 1) fprintf(out, "\t\tConstantColor,\n");
		if ((renderMode & 16) == 1) fprintf(out, "\t\tSortPrimsFarZ,\n");
		if ((renderMode & 32) == 1) fprintf(out, "\t\tFullResolution,\n");
		if (priorityPlane != 0) fprintf(out, "\t\tPriorityPlane %d,\n", priorityPlane);
		for (int i = 0; i < lays.size(); i++)
		{
			lays[i].MdlWrite(out);
		}
		fprintf(out, "\t}\n");
	}

	void MdxWrite(ofstream &out)
	{
		MdxWriteInt(out, ByteCount());
		MdxWriteInt(out, priorityPlane);
		MdxWriteInt(out, renderMode);
		out.write("LAYS", 4);
		MdxWriteInt(out, lays.size());
		for (int i = 0; i < lays.size(); i++)
		{
			lays[i].MdxWrite(out);
		}
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		while (line->type != LINE_EOF) {
			line->init(in);
			if (line->type == LINE_FLAG && strmatch(line->label, "constantcolor")) {
				renderMode += 1;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "sortprimsfarz")) {
				renderMode += 16;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "fullresolution")) {
				renderMode += 32;
			}
			else if (line->type == LINE_LABELED_DATA && line->i_match && strmatch(line->label, "priorityplane")) {
				priorityPlane = line->i_data;
			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "layer")) {
				MTL_LAY newLay; newLay.MdlRead(in, line, err);
				lays.push_back(newLay);
			}
			else if (line->type == LINE_CLOSER)
				return;
			else {
				err->add(line->lineText, line->lineCounter, "Unexpected in a Model chunk");
			}
		}
	}

	void MdxRead(ifstream &in)
	{
		MdxReadInt(in); // Read size
		priorityPlane = MdxReadInt(in);
		renderMode = MdxReadInt(in);

		int tag = MdxReadInt(in);
		assert(tag == TAGINT("LAYS"));
		int layCount = MdxReadInt(in);
		for (int i = 0; i < layCount; i++)
		{
			MTL_LAY newLay;
			newLay.MdxRead(in);
			lays.push_back(newLay);
		}
	}

	int ByteCount()
	{
		int total = 20; // nbytesi, PriorityPlane, RenderMode, 'LAYS', nlays
		for (int i = 0; i < lays.size(); i++)
		{
			total += lays[i].ByteCount();
		}
		return total;
	}
};


class OBJ : public Chunk
{
public:
	string name;
	int objectID;
	int parent;
	int type;
	// HELP:0;BONE:256;LITE:512;EVTS:1024;ATCH:2048;CLID:8192;
	//   +1(DontInherit Translation) +16(BillboardedLockX)
	//   +2(DontInherit Scaling)	 +32(BillboardedLockY)
	//   +4(DontInherit Rotation)	 +64(BillboardedLockZ)
	//   +8(Billboarded)		 	+128(CameraAnchored)
	KGSC kgtr; // Translation
	KGRT kgrt; // Rotation
	KGSC kgsc; // Scaling
	KMTA katv; // Visibility

#ifdef TRANSFORM
	float mat[16];
	float rotMat[16];

	void Transform(int frame, float* parentMat, float* parentRotMat, PIVT p
#ifdef DEBUG
		, ofstream &out
#endif
	)
	{
		//kgtr.Transform(frame);
		kgrt.Transform(frame
#ifdef DEBUG
			, out
#endif
		);
		kgtr.Transform(frame
#ifdef DEBUG
			, out
#endif
		);

		// TODO: multiply move, translation, scale

		float temp0[16], temp1[16], temp2[16];

		// TEMP
		//m_ident(parentMat);
		//m_ident(parentRotMat);

		// Parent
		//m_mul(rotMat, parentRotMat, kgrt.mat);
		//m_mul(mat, parentMat, kgrt.mat);

		m_ident(mat);

		m_copy(temp0, mat);
		m_mul(mat, temp0, parentMat);

		//m_copy(mat, parentMat);

		// Movement
		m_mul(temp2, mat, kgtr.mat);


		// Rotation
		m_mul(temp0, temp2, m_mover(temp1, p.x, p.y, p.z));
		m_mul(temp2, temp0, kgrt.mat);
		m_mul(mat, temp2, m_mover(temp1, -p.x, -p.y, -p.z));


		/*// A second try, doing local stuff first

		// Rotation
		m_mover(temp1, -p.x, -p.y, -p.z); // Center pivot point on origin
		m_mul(temp2, temp1, kgrt.mat); // Rotate
		m_mover(temp1, p.x, p.y, p.z);
		m_mul(temp0, temp2, temp1); // Move back

		// Parent
		m_mul(rotMat, parentRotMat, kgrt.mat);
		m_mul(mat, temp0, parentMat);


		// Movement
		//m_mul(mat, temp0, kgtr.mat);*/
	}
#endif

	void MdxRead(ifstream &in)
	{
		int start = in.tellg();
		int end = start + MdxReadInt(in);
		name = MdxReadString(in, 80);

		//ASSERT (name.length()!=0) 

		objectID = MdxReadInt(in);
		parent = MdxReadInt(in);
		type = MdxReadInt(in);
		char tag[5];
		tag[4] = 0;
		while (in.tellg() < end)
		{
			in.read(tag, 4);
			if (strcmp(tag, "KGTR") == 0)
				kgtr.MdxRead(in);
			else if (strcmp(tag, "KGRT") == 0)
				kgrt.MdxRead(in);
			else if (strcmp(tag, "KGSC") == 0)
				kgsc.MdxRead(in);
			else if (strcmp(tag, "KATV") == 0)
				katv.MdxRead(in);
			else {
				string s = name;
				assert(false);
			}
		}
		assert((int)in.tellg() == end);
	}

	void MdlWriteProps(FILE *out)
	{
		fprintf(out, "\tObjectId %d,\n", objectID);
		if (parent != -1)
			fprintf(out, "\tParent %d,\n", parent);
		if ((type&BILLBOARDED_X)) fprintf(out, "\tBillboardedLockX,\n");
		if ((type&BILLBOARDED_Y)) fprintf(out, "\tBillboardedLockY,\n");
		if ((type&BILLBOARDED_Z)) fprintf(out, "\tBillboardedLockZ,\n");
		if ((type&CAMERA_ANCHORED)) fprintf(out, "\tCameraAnchored,\n");
		if ((type&BILLBOARDED)) fprintf(out, "\tBillboarded,\n");
		if ((type&DONT_INHERIT_TRANSLATION)) fprintf(out, "\tDontInherit { Translation },\n");
		if ((type&DONT_INHERIT_ROTATION)) fprintf(out, "\tDontInherit { Rotation },\n");
		if ((type&DONT_INHERIT_SCALING)) fprintf(out, "\tDontInherit { Scaling },\n");
	}

	void MdlWriteAnims(FILE *out)
	{
		if (kgtr.key.size() > 0)
			kgtr.MdlWrite(out, "Translation", "\t");
		if (kgrt.key.size() > 0)
			kgrt.MdlWrite(out, "Rotation", "\t");
		if (kgsc.key.size() > 0)
			kgsc.MdlWrite(out, "Scaling", "\t");
		if (katv.key.size() > 0)
			katv.MdlWrite(out, "Visibility", "\t");
	}

	int ByteCount() {
		return
			4 + // inclusive length
			80 +	// name string
			12 + // attributes
			kgtr.ByteCount() +
			kgrt.ByteCount() +
			kgsc.ByteCount() +
			katv.ByteCount()
			;
	}

	OBJ() {
		parent = -1;
		type = 0;
		objectID = 0;
	}

	void MdxWrite(ofstream &out)
	{
		MdxWriteInt(out, ByteCount());
		MdxWriteString(out, name, 80);
		MdxWriteInt(out, objectID);
		MdxWriteInt(out, parent);
		MdxWriteInt(out, type);
		kgtr.MdxWrite(out, "KGTR");
		kgrt.MdxWrite(out, "KGRT");
		kgsc.MdxWrite(out, "KGSC");
		katv.MdxWrite(out, "KATV");
	}

	// See if this line is part of an OBJ. Deal with it if is it
	// and return whether or not it was.
	bool MdlReadLine(ifstream &in, MdlLine *line, MdlError *err)
	{
		if (line->type == LINE_LABELED_DATA && strmatch(line->label, "parent") && line->i_match)
			parent = line->i_data;
		else if (line->type == LINE_LABELED_DATA && strmatch(line->label, "objectid") && line->i_match)
			objectID = line->i_data;
		else if (line->type == LINE_OPENER && strmatch(line->chunk, "translation"))
			kgtr.MdlRead(in, line, err);
		else if (line->type == LINE_OPENER && strmatch(line->chunk, "rotation"))
			kgrt.MdlRead(in, line, err);
		else if (line->type == LINE_OPENER && strmatch(line->chunk, "scaling"))
			kgsc.MdlRead(in, line, err);
		else if (line->type == LINE_OPENER && strmatch(line->chunk, "visibility"))
			katv.MdlRead(in, line, err);
		else if (line->type == LINE_LABELED_FLAG && strmatch(line->chunk, "dontinherit") && strmatch(line->label, "translation"))
			type |= DONT_INHERIT_TRANSLATION;
		else if (line->type == LINE_LABELED_FLAG && strmatch(line->chunk, "dontinherit") && strmatch(line->label, "scaling"))
			type |= DONT_INHERIT_ROTATION;
		else if (line->type == LINE_LABELED_FLAG && strmatch(line->chunk, "dontinherit") && strmatch(line->label, "rotation"))
			type |= DONT_INHERIT_SCALING;
		else if (line->type == LINE_FLAG && strmatch(line->label, "Billboarded"))
			type |= BILLBOARDED;
		else if (line->type == LINE_FLAG && strmatch(line->label, "BillboardedLockX"))
			type |= BILLBOARDED_X;
		else if (line->type == LINE_FLAG && strmatch(line->label, "BillboardedLockY"))
			type |= BILLBOARDED_Y;
		else if (line->type == LINE_FLAG && strmatch(line->label, "BillboardedLockZ"))
			type |= BILLBOARDED_Z;
		else if (line->type == LINE_FLAG && strmatch(line->label, "CameraAnchored"))
			type |= CAMERA_ANCHORED;
		else
			return false;
		return true;
	}
};

class BONE : public Chunk {
public:
	int geosetID;
	int geosetAnimID;

	OBJ obj;

	int ByteCount() {
		return obj.ByteCount();
	}

	BONE() {
		geosetID = -1;
		geosetAnimID = -1;
		obj.type = TYPE_BONE;
	}

	void MdxRead(ifstream &in)
	{
		obj.MdxRead(in);
		geosetID = MdxReadInt(in);
		geosetAnimID = MdxReadInt(in);
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		obj.name = line->label;

		do {
			line->init(in);
			if (obj.MdlReadLine(in, line, err)) {

			}
			else if (line->type == LINE_LABELED_DATA && line->i_match && strmatch(line->label, "geosetid")) {
				geosetID = line->i_data;
			}
			else if (line->type == LINE_LABELED_DATA && line->i_match && strmatch(line->label, "geosetanimid")) {
				geosetAnimID = line->i_data;
			}
			else if (line->type == LINE_LABELED_FLAG && strmatch(line->chunk, "geosetid") && strmatch(line->label, "multiple")) {
				geosetID = -1;
			}
			else if (line->type == LINE_LABELED_FLAG && strmatch(line->chunk, "geosetanimid") && strmatch(line->label, "none")) {
				geosetAnimID = -1;
			}
			else if (line->type == LINE_EOF || line->type == LINE_CLOSER)
				break;
			else {
				err->add(line->lineText, line->lineCounter, "Unexpected in a Bone chunk");
				break;
			}
		} while (true);
	}

	void MdlWrite(FILE *out)
	{
		fprintf(out, "Bone \"%s\" {\n", obj.name.c_str());

		obj.MdlWriteProps(out);

		if (geosetID != -1) fprintf(out, "\tGeosetId %d,\n", geosetID);
		else fprintf(out, "\tGeosetId Multiple,\n");

		if (geosetAnimID != -1) fprintf(out, "\tGeosetAnimId %d,\n", geosetAnimID);
		else fprintf(out, "\tGeosetAnimId None,\n");

		obj.MdlWriteAnims(out);

		fprintf(out, "}\n");
	}

	void MdxWrite(ofstream &out)
	{
		obj.MdxWrite(out);
		MdxWriteInt(out, geosetID);
		MdxWriteInt(out, geosetAnimID);
	}


};

class GEOS_Vertex
{
public:
	float x, y, z;
	float nx, ny, nz;
	vector<float> u;
	vector<float> v;

#ifdef TRANSFORM
	float tx, ty, tz;
	float tnx, tny, tnz;
	float tu, tv;

	float screenX, screenY, screenZ;

	bool selected;
	bool uvSelected;
#endif
	int matrix;

	GEOS_Vertex(): x(0), y(0), z(0), nx(0), ny(0), nz(0), matrix(0)
	{
	}

	GEOS_Vertex(float vx, float vy, float vz, float vu, float vv, int vmatrix): nx(0), ny(0), nz(0)
	{
		x = vx;
		y = vy;
		z = vz;
		u.push_back(vu);
		v.push_back(vv);
		matrix = vmatrix;
	}

	bool matches(GEOS_Vertex vert)
	{
		for (int i = 0; i < u.size() && i < vert.u.size(); i++)
		{
			if (u[i] != vert.u[i] || v[i] != vert.v[i]) {
				return false;
			}
		}
		return (
			x == vert.x && y == vert.y && z == vert.z &&
			nx == vert.nx && ny == vert.ny && nz == vert.nz &&
			matrix == vert.matrix
			);
	}
};

// Matrices. Contains a list of bones to follow
class GEOS_MATS
{
public:
	vector<int> bone;
};

class GEOS_ANIM
{
public:
	float	boundsRadius;
	float	minExtx, minExty, minExtz;
	float	maxExtx, maxExty, maxExtz;

	GEOS_ANIM() {
		boundsRadius = 0;
		minExtx = minExty = minExtz = 0;
		maxExtx = maxExty = maxExtz = 0;
	}
};

class GEOS : public Chunk
{
public:
	vector<GEOS_Vertex> vert;
	vector<int> face;
#ifdef TRANSFORM
	vector<bool> faceSel;
#endif
	vector<GEOS_MATS> mats;
	vector<GEOS_ANIM> anim;

	int materialID;
	int selectionGroup;
	int selectable;		//(0:none;4:Unselectable)
	float boundsRadius;
	float minExtx, minExty, minExtz;
	float maxExtx, maxExty, maxExtz;

	GEOS(): materialID(0), boundsRadius(0), minExtx(0), minExty(0), minExtz(0), maxExtx(0), maxExty(0), maxExtz(0)
	{
#ifdef TRANSFORM
		visible = true;
#endif
		selectable = 0;
		selectionGroup = 0;
	}

	int EnsureVertExists(GEOS_Vertex thisVert)
	{
		for (int i = 0; i < vert.size(); i++)
		{
			if (vert[i].matches(thisVert))
			{
				return i;
			}
		}

		vert.push_back(thisVert);
		return vert.size() - 1;
	}

	void MdxRead(ifstream &in)
	{
		MdxReadInt(in); // read size
		int tag, i;

		tag = MdxReadInt(in);
		assert(tag == TAGINT("VRTX"));
		int vertCount = MdxReadInt(in);
		for (i = 0; i < vertCount; i++)
		{
			GEOS_Vertex v;
			v.x = MdxReadFloat(in);
			v.y = MdxReadFloat(in);
			v.z = MdxReadFloat(in);
			vert.push_back(v);
		}

		tag = MdxReadInt(in);
		assert(tag == TAGINT("NRMS"));
		int normCount = MdxReadInt(in);
		assert(vertCount == normCount);
		for (i = 0; i < normCount; i++)
		{
			vert[i].nx = MdxReadFloat(in);
			vert[i].ny = MdxReadFloat(in);
			vert[i].nz = MdxReadFloat(in);
		}

		// Primitive type. Should be four (triangles)
		tag = MdxReadInt(in);
		assert(tag == TAGINT("PTYP"));
		int ptypCount = MdxReadInt(in);
		for (i = 0; i < ptypCount; i++)
		{
			MdxReadInt(in);
		}

		// How many vertices to include in each of the about groups
		tag = MdxReadInt(in);
		assert(tag == TAGINT("PCNT"));
		int pcntCount = MdxReadInt(in);
		for (i = 0; i < pcntCount; i++)  // orignally was ptypCount
		{
			MdxReadInt(in);
		}

		// Actaul face data
		tag = MdxReadInt(in);
		assert(tag == TAGINT("PVTX"));
		int pvtxCount = MdxReadInt(in);
		for (i = 0; i < pvtxCount; i++)
		{
			face.push_back(MdxReadShort(in));
		}

		// Vertex groups
		tag = MdxReadInt(in);
		assert(tag == TAGINT("GNDX"));
		int gndxCount = MdxReadInt(in);
		assert(gndxCount == vertCount);
		for (i = 0; i < gndxCount; i++)
		{
			vert[i].matrix = MdxReadByte(in);
		}

		// Matrix content counts. A list of lengths for matrices
		tag = MdxReadInt(in);
		assert(tag == TAGINT("MTGC"));
		int mtgcCount = MdxReadInt(in);
		vector<int> matrixLength(mtgcCount);
		for (i = 0; i < mtgcCount; i++)
		{
			matrixLength[i] = MdxReadInt(in);
		}

		// Matrix values
		tag = MdxReadInt(in);
		assert(tag == TAGINT("MATS"));
		int matsCount = MdxReadInt(in);
		int currMat = -1, currInMat = 0;
		for (i = 0; i < matsCount; i++)
		{
			if (currMat == -1 || currInMat == matrixLength[currMat])
			{
				GEOS_MATS m;
				mats.push_back(m);
				currMat++;
				currInMat = 0;
			}
			mats[currMat].bone.push_back(MdxReadInt(in));
			currInMat++;
		}

		materialID = MdxReadInt(in);
		selectionGroup = MdxReadInt(in);
		selectable = MdxReadInt(in);
		boundsRadius = MdxReadFloat(in);
		minExtx = MdxReadFloat(in);
		minExty = MdxReadFloat(in);
		minExtz = MdxReadFloat(in);
		maxExtx = MdxReadFloat(in);
		maxExty = MdxReadFloat(in);
		maxExtz = MdxReadFloat(in);

		int animCount = MdxReadInt(in);
		for (i = 0; i < animCount; i++)
		{
			GEOS_ANIM a;
			a.boundsRadius = MdxReadFloat(in);
			a.minExtx = MdxReadFloat(in);
			a.minExty = MdxReadFloat(in);
			a.minExtz = MdxReadFloat(in);
			a.maxExtx = MdxReadFloat(in);
			a.maxExty = MdxReadFloat(in);
			a.maxExtz = MdxReadFloat(in);
			anim.push_back(a);
		}

		// UVBS
		tag = MdxReadInt(in);
		assert(tag == TAGINT("UVAS"));
		int uvCount = MdxReadInt(in);
		for (i = 0; i < uvCount; i++)
		{
			tag = MdxReadInt(in);
			assert(tag == TAGINT("UVBS"));
			int uvVertCount = MdxReadInt(in);
			for (int j = 0; j < uvVertCount; j++)
			{
				vert[j].u.push_back(MdxReadFloat(in));
				vert[j].v.push_back(MdxReadFloat(in));
			}
		}

#ifdef TRANSFORM
		faceSel.clear();
		for (i = 0; i < face.size() / 3; i++)
		{
			faceSel.push_back(false);
		}
#endif
	}

	void MdlWrite(FILE *out)
	{
		int j;
		fprintf(out, "Geoset {\n");

		// Vertices
		int vertCount = vert.size();
		fprintf(out, "\tVertices %d {\n", vertCount);
		for (j = 0; j < vertCount; j++)
		{
			fprintf(out, "\t\t{ %f, %f, %f },\n", vert[j].x, vert[j].y, vert[j].z);
		}
		fprintf(out, "\t}\n");

		// Normals
		fprintf(out, "\tNormals %d {\n", vertCount);
		for (j = 0; j < vertCount; j++)
		{
			fprintf(out, "\t\t{ %f, %f, %f },\n", vert[j].nx, vert[j].ny, vert[j].nz);
		}
		fprintf(out, "\t}\n");

		// TVertices
		for (int i = 0; i < vert[0].u.size(); i++) {
			fprintf(out, "\tTVertices %d {\n", vertCount);
			for (j = 0; j < vertCount; j++)
			{
				fprintf(out, "\t\t{ %f, %f },\n", vert[j].u[0], vert[j].v[0]);
			}
			fprintf(out, "\t}\n");
		}


		// VertexGroup
		fprintf(out, "\tVertexGroup {\n");
		for (j = 0; j < vertCount; j++)
		{
			fprintf(out, "\t\t%d,\n", vert[j].matrix);
		}
		fprintf(out, "\t}\n");

		// Faces
		int mdlFaceCount = face.size(); // *3, technically
		fprintf(out, "\tFaces 1 %d {\n", mdlFaceCount);
		fprintf(out, "\t\tTriangles {\n");
		string strFaces = "\t\t\t{ ";
		for (j = 0; j < mdlFaceCount; j++) {
			std::stringstream stringStream;
			stringStream << face[j];
			strFaces.append(stringStream.str());
			if (j < mdlFaceCount - 1) strFaces.append(", ");
		}
		strFaces.append(" },\n");
		fprintf(out, "%s", strFaces.c_str());
		fprintf(out, "\t\t}\n");
		fprintf(out, "\t}\n");

		// Groups
		int groupCount = mats.size();
		int groupItemCount = 0;
		for (j = 0; j < groupCount; j++) groupItemCount += mats[j].bone.size();

		fprintf(out, "\tGroups %d %d {\n", groupCount, groupItemCount);
		for (j = 0; j < groupCount; j++)
		{
			string strThisGroup = "\t\tMatrices { ";
			for (unsigned int k = 0; k < mats[j].bone.size(); k++) {
				std::stringstream stringStream;
				stringStream << mats[j].bone[k];
				strThisGroup.append(stringStream.str());

				if (k < mats[j].bone.size() - 1) strThisGroup.append(", ");
			}
			strThisGroup.append(" },\n");
			fprintf(out, "%s", strThisGroup.c_str());
		}
		fprintf(out, "\t}\n");

		// Bounding data
		fprintf(out, "\tMinimumExtent { %f, %f, %f },\n", minExtx, minExty, minExtz);
		fprintf(out, "\tMaximumExtent { %f, %f, %f },\n", maxExtx, maxExty, maxExtz);
		if (boundsRadius != 0.0f)
			fprintf(out, "\tBoundsRadius %f,\n", boundsRadius);

		for (j = 0; j < anim.size(); j++)
		{
			fprintf(out, "\tAnim {\n");
			fprintf(out, "\t\tMinimumExtent { %f, %f, %f },\n", anim[j].minExtx, anim[j].minExty, anim[j].minExtz);
			fprintf(out, "\t\tMaximumExtent { %f, %f, %f },\n", anim[j].maxExtx, anim[j].maxExty, anim[j].maxExtz);
			if (anim[j].boundsRadius != 0.0f)
				fprintf(out, "\t\tBoundsRadius %f,\n", anim[j].boundsRadius);
			fprintf(out, "\t}\n");
		}

		fprintf(out, "\tMaterialID %d,\n", materialID);
		fprintf(out, "\tSelectionGroup %d,\n", selectionGroup);
		if (selectable == 4) fprintf(out, "\tUnselectable,\n");
		fprintf(out, "}\n");
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		int i;

		// Vertices
		line->init(in);
		if (line->type != LINE_OPENER || !strmatch(line->chunk, "vertices"))
		{
			err->add(line->lineText, line->lineCounter, "Expected: Vertices # {");
			return;
		}
		line->init(in);
		while (line->type != LINE_EOF && line->type != LINE_CLOSER)
		{
			if (line->type == LINE_BRACED_DATA && line->f_listMatch && line->f_listData.size() == 3)
			{
				GEOS_Vertex v;
				v.x = line->f_listData[0];
				v.y = line->f_listData[1];
				v.z = line->f_listData[2];
				vert.push_back(v);
			}
			else {
				err->add(line->lineText, line->lineCounter, "Expected a vertex.");
				return;
			}
			line->init(in);
		}
		int vertexCount = vert.size();

		// Normals
		line->init(in);
		if (line->type != LINE_OPENER || !strmatch(line->chunk, "normals"))
		{
			err->add(line->lineText, line->lineCounter, "Expected \"Normals # {\"");
			return;
		}

		line->init(in);
		int normalCount = 0;
		while (line->type == LINE_BRACED_DATA && line->f_listMatch && line->f_listData.size() == 3)
		{
			if (normalCount == vertexCount)
			{
				err->add(line, "Too many normals.");
				return;
			}
			vert[normalCount].nx = line->f_listData[0];
			vert[normalCount].ny = line->f_listData[1];
			vert[normalCount].nz = line->f_listData[2];
			normalCount++;
			line->init(in);
		}
		if (line->type != LINE_CLOSER)
		{
			err->add(line->lineText, line->lineCounter, "Expected a normal or a closing }.");
			return;
		}
		else if (normalCount != vert.size())
		{
			err->add(line->lineText, line->lineCounter, "Wrong number of normals (did not match number of vertices).");
		}

		// TVertices
		line->init(in);
		while (line->type == LINE_OPENER && strmatch(line->chunk, "tvertices"))
		{
			line->init(in);
			int currTVert = 0;
			while (line->type == LINE_BRACED_DATA && line->f_listMatch && line->f_listData.size() == 2)
			{
				if (currTVert == vertexCount)
				{
					err->add(line, "Too many TVertices.");
					return;
				}
				vert[currTVert].u.push_back(line->f_listData[0]);
				vert[currTVert].v.push_back(line->f_listData[1]);
				currTVert++;
				line->init(in);
			}
			if (line->type != LINE_CLOSER)
			{
				err->add(line->lineText, line->lineCounter, "Expected a tvertex or a closing }.");
				return;
			}

			line->init(in); // Beginning of next chunk
		}

		// VertexGroup
		if (line->type != LINE_OPENER || !strmatch(line->chunk, "vertexgroup"))
		{
			err->add(line->lineText, line->lineCounter, "Expected \"VertexGroup {\"");
			return;
		}

		line->init(in);
		int vgCount = 0;
		while (line->type == LINE_LONE_INT && line->i_match)
		{
			if (vgCount == vertexCount)
			{
				err->add(line, "Too many VertexGroup entries.");
				return;
			}
			vert[vgCount].matrix = line->i_data;
			vgCount++;
			line->init(in);
		}
		if (line->type != LINE_CLOSER)
		{
			err->add(line, "Expected a vertex group entry or a closing }.");
			return;
		}
		else if (vgCount != vert.size())
		{
			err->add(line, "Wrong number of vertex group entries (did not match number of vertices).");
		}

		// Faces
		line->init(in);
		if (line->type != LINE_OPENER || !strmatch(line->chunk, "faces"))
		{
			err->add(line, "Expected \"Faces # # {\"");
			return;
		}
		line->init(in);
		if (line->type != LINE_OPENER || !strmatch(line->chunk, "triangles"))
		{
			err->add(line, "Expected \"Triangles {\"");
			return;
		}
		line->init(in);
		while (line->type == LINE_BRACED_DATA && line->i_listMatch)
		{
			for (i = 0; i < line->i_listData.size(); i++)
			{
				face.push_back(line->i_listData[i]);
			}
			line->init(in);
		}
		for (i = 0; i < 2; i++)
		{
			if (line->type != LINE_CLOSER)
			{
				err->add(line, "Expected a closing }.");
				return;
			}
			else if (normalCount != vert.size())
			{
				err->add(line, "Wrong number of normals (did not match number of vertices).");
			}
			line->init(in);
		}


#ifdef TRANSFORM
		faceSel.clear();
		for (i = 0; i < face.size() / 3; i++)
		{
			faceSel.push_back(false);
		}
#endif

		// Matrices
		if (line->type != LINE_OPENER || !strmatch(line->chunk, "groups"))
		{
			err->add(line, "Expected \"Groups # # {\"");
			return;
		}
		line->init(in);
		while (line->type == LINE_BRACED_LABELED_DATA && line->i_listMatch)
		{
			GEOS_MATS m;
			for (i = 0; i < line->i_listData.size(); i++)
				m.bone.push_back(line->i_listData[i]);
			mats.push_back(m);
			line->init(in);
		}
		if (line->type != LINE_CLOSER)
		{
			err->add(line, "Expected a matrix or a closing }.");
			return;
		}

		selectable = 0;
		while (line->type != LINE_EOF) {
			line->init(in);
			if (line->type == LINE_OPENER && strmatch(line->chunk, "anim")) {
				GEOS_ANIM a;
				line->init(in);
				while (line->type != LINE_CLOSER && line->type != LINE_EOF)
				{
					if (line->type == LINE_BRACED_LABELED_DATA && strmatch(line->label, "maximumextent") && line->f_listMatch && line->f_listData.size() == 3) {
						a.maxExtx = line->f_listData[0];
						a.maxExty = line->f_listData[1];
						a.maxExtz = line->f_listData[2];
					}
					else if (line->type == LINE_BRACED_LABELED_DATA && strmatch(line->label, "minimumextent") && line->f_listMatch && line->f_listData.size() == 3) {
						a.minExtx = line->f_listData[0];
						a.minExty = line->f_listData[1];
						a.minExtz = line->f_listData[2];
					}
					else if (line->type == LINE_LABELED_DATA && line->f_match && strmatch(line->label, "boundsradius")) {
						a.boundsRadius = line->f_data;
					}
					else {
						err->add(line, "Unexpected in an Anim chunk.");
						return;
					}

					line->init(in);
				}
				anim.push_back(a);
			}
			else if (line->type == LINE_BRACED_LABELED_DATA && line->f_listMatch && strmatch(line->label, "minimumextent")) {
				minExtx = line->f_listData[0];
				minExty = line->f_listData[1];
				minExtz = line->f_listData[2];
			}
			else if (line->type == LINE_BRACED_LABELED_DATA && line->f_listMatch && strmatch(line->label, "maximumextent")) {
				maxExtx = line->f_listData[0];
				maxExty = line->f_listData[1];
				maxExtz = line->f_listData[2];
			}
			else if (line->type == LINE_LABELED_DATA && line->f_match && strmatch(line->label, "boundsradius"))
				boundsRadius = line->f_data;
			else if (line->type == LINE_LABELED_DATA && line->i_match && strmatch(line->label, "materialid"))
				materialID = line->i_data;
			else if (line->type == LINE_LABELED_DATA && line->i_match && strmatch(line->label, "selectiongroup"))
				selectionGroup = line->i_data;
			else if (line->type == LINE_FLAG && strmatch(line->label, "unselectable"))
				selectable = 4;
			else if (line->type == LINE_CLOSER)
				return;
			else {
				err->add(line->lineText, line->lineCounter, "Unexpected in a Geoset chunk");
			}
		}


	}


	void MdxWrite(ofstream &out)
	{
		int i;
		MdxWriteInt(out, ByteCount() + 4);

		out.write("VRTX", 4);
		MdxWriteInt(out, vert.size());
		for (i = 0; i < vert.size(); i++)
		{
			MdxWriteFloat(out, vert[i].x);
			MdxWriteFloat(out, vert[i].y);
			MdxWriteFloat(out, vert[i].z);
		}

		out.write("NRMS", 4);
		MdxWriteInt(out, vert.size());
		for (i = 0; i < vert.size(); i++)
		{
			MdxWriteFloat(out, vert[i].nx);
			MdxWriteFloat(out, vert[i].ny);
			MdxWriteFloat(out, vert[i].nz);
		}

		// Specifies one set of triangles (most compact structure)
		out.write("PTYP", 4);
		MdxWriteInt(out, 1);
		MdxWriteInt(out, 4); //gohere

		// Specifies length of that triangle set
		out.write("PCNT", 4);
		MdxWriteInt(out, 1);
		MdxWriteInt(out, face.size());

		out.write("PVTX", 4);
		MdxWriteInt(out, face.size());
		for (i = 0; i < face.size(); i++) {
			MdxWriteShort(out, (short)face[i]);
		}

		// Vertex groups; assigns vertices to matrices
		out.write("GNDX", 4);
		MdxWriteInt(out, vert.size());
		for (i = 0; i < vert.size(); i++)
		{
			MdxWriteByte(out, (unsigned char)vert[i].matrix);
		}

		out.write("MTGC", 4);
		MdxWriteInt(out, mats.size());
		int matrixTotal = 0;
		for (i = 0; i < mats.size(); i++)
		{
			MdxWriteInt(out, mats[i].bone.size());
			matrixTotal += mats[i].bone.size();
		}

		out.write("MATS", 4);
		MdxWriteInt(out, matrixTotal);
		for (i = 0; i < mats.size(); i++)
		{
			for (int j = 0; j < mats[i].bone.size(); j++)
			{
				MdxWriteInt(out, mats[i].bone[j]);
			}
		}

		// Misc data
		MdxWriteInts(out, 3, materialID, selectionGroup, selectable);
		MdxWriteFloats(out, 7, boundsRadius, minExtx, minExty, minExtz,
			maxExtx, maxExty, maxExtz);

		// Anims
		MdxWriteInt(out, anim.size());
		for (i = 0; i < anim.size(); i++)
		{
			MdxWriteFloats(out, 7, anim[i].boundsRadius,
				anim[i].minExtx, anim[i].minExty, anim[i].minExtz,
				anim[i].maxExtx, anim[i].maxExty, anim[i].maxExtz);
		}

		// UV info
		out.write("UVAS", 4);
		int uvCount = vert[0].u.size(); // Assumed the same for all verts
		MdxWriteInt(out, uvCount);
		for (i = 0; i < uvCount; i++)
		{
			out.write("UVBS", 4);
			MdxWriteInt(out, vert.size());
			for (int j = 0; j < vert.size(); j++)
			{
				MdxWriteFloat(out, vert[j].u[i]);
				MdxWriteFloat(out, vert[j].v[i]);
			}
		}
	}

	int ByteCount()
	{
		int groupsLen = 0;
		for (int i = 0; i < mats.size(); i++) {
			groupsLen += mats[i].bone.size();
		}
		return
			8 + vert.size() * 12 +	// VTRX
			8 + vert.size() * 12 +	// NRMS
			24 +				// PTYP, PCNT
			8 + face.size() * 2 +	// PVTX
			8 + vert.size() * 1 +	// GNDX
			8 + mats.size() * 4 +	// MTGC
			8 + groupsLen * 4 + 	// MATS
			40 +				// MaterialID through maxExtx
			4 + anim.size() * 28 +	// anims
			8 +					// UVAS
			vert[0].u.size()*(8 + vert.size() * 8)	// UVBS
			;
	}

#ifdef TRANSFORM

	bool visible; // Is this geoset toggled to show?

	void AddFace(int f1, int f2, int f3, bool sel = true)
	{
		face.push_back(f1);
		face.push_back(f2);
		face.push_back(f3);
		faceSel.push_back(sel);
	}

	void Transform(int frame, vector<BONE> bone
#ifdef DEBUG
		, ofstream &out
#endif
	)
	{
#ifdef DEBUG
		out << "vert.size()=" << vert.size() << ", mats.size()=" << mats.size() << endl;
#endif
		for (int i = 0; i < vert.size(); i++)
		{
			/*vert[i].tx = vert[i].x;
			vert[i].ty = vert[i].y;
			vert[i].tz = vert[i].z;
			vert[i].tnx = vert[i].nx;
			vert[i].tny = vert[i].ny;
			vert[i].tnz = vert[i].nz;*/
			vert[i].tu = vert[i].u[0];
			vert[i].tv = vert[i].v[0];

			// TODO: the normal will have to be multiplied by ONLY the rotatation matrix

			float mat[16];
			int k;
			for (k = 0; k < 16; k++)
			{
				mat[k] = 0;
			}
			for (int j = 0; j < mats[vert[i].matrix].bone.size(); j++)
			{
				int boneNum = mats[vert[i].matrix].bone[j];
				for (k = 0; k < 16; k++)
					mat[k] += bone[boneNum].obj.mat[k];
			}
			for (k = 0; k < 16; k++)
			{
				mat[k] /= (float)mats[vert[i].matrix].bone.size();
			}
			//vert[i].matrix
		   //float* mat = bone[boneNum].obj.mat;
		   //float* rotMat = bone[boneNum].obj.rotMat;
			vert[i].tx =
				vert[i].x * mat[0] +
				vert[i].y * mat[1] +
				vert[i].z * mat[2] +
				mat[3];
			vert[i].ty =
				vert[i].x * mat[4] +
				vert[i].y * mat[5] +
				vert[i].z * mat[6] +
				mat[7];
			vert[i].tz =
				vert[i].x * mat[8] +
				vert[i].y * mat[9] +
				vert[i].z * mat[10] +
				mat[11];
			vert[i].tnx = vert[i].nx * mat[0] + vert[i].ny * mat[1] + vert[i].nz * mat[2] + mat[3];
			vert[i].tny = vert[i].nx * mat[4] + vert[i].ny * mat[5] + vert[i].nz * mat[6] + mat[7];
			vert[i].tnz = vert[i].nx * mat[8] + vert[i].ny * mat[9] + vert[i].nz * mat[10] + mat[11];
		}
	}

	void GetFaceNormal(int which, float *fnx, float *fny, float *fnz)
	{
		int j = which * 3;

		GEOS_Vertex* v1 = &vert[face[j]];
		GEOS_Vertex* v2 = &vert[face[j + 1]];
		GEOS_Vertex* v3 = &vert[face[j + 2]];
		float v1x = v1->x - v2->x;
		float v1y = v1->y - v2->y;
		float v1z = v1->z - v2->z;
		float v2x = v2->x - v3->x;
		float v2y = v2->y - v3->y;
		float v2z = v2->z - v3->z;
		/*
		int f1 = face[j];
		int f2 = face[j+1];
		int f3 = face[j+2];


		float v1x = vert[f1].x - vert[f2].x;
		float v1y = vert[f1].y - vert[f2].y;
		float v1z = vert[f1].z - vert[f2].z;
		float v2x = vert[f2].x - vert[f3].x;
		float v2y = vert[f2].y - vert[f3].y;
		float v2z = vert[f2].z - vert[f3].z;*/

		*fnx = v1y * v2z - v1z * v2y;
		*fny = v1z * v2x - v1x * v2z;
		*fnz = v1x * v2y - v1y * v2x;
	}

	void RecalculateNormals()
	{
		//float *faceNormalX, *faceNormalY, &faceNormalZ;
		int i, j;
		int faceCount = face.size() / 3;

		//faceNormalX = new float[faceCount];
		//faceNormalY = new float[faceCount];
		//faceNormalZ = new float[faceCount];

		int vertCount = vert.size();

		//int* faceForVert;
		//faceForVert = new int[vertCount];
		for (i = 0; i < vertCount; i++)
		{
			vert[i].nx = vert[i].ny = vert[i].nz = 0.0f;
			//faceForVert[i] = 0;
		}

		int f1, f2, f3;
		float fnx, fny, fnz;
		for (i = 0; i < faceCount; i++)
		{
			// Dot product of two edge vectors
			j = i * 3;

			f1 = face[j];
			f2 = face[j + 1];
			f3 = face[j + 2];

			/*
						float v1x = vert[f1].x - vert[f2].x;
						float v1y = vert[f1].y - vert[f2].y;
						float v1z = vert[f1].z - vert[f2].z;
						float v2x = vert[f2].x - vert[f3].x;
						float v2y = vert[f2].y - vert[f3].y;
						float v2z = vert[f2].z - vert[f3].z;

						fnx = v1y*v2z - v1z*v2y;
						fny = v1z*v2x - v1x*v2z;
						fnz = v1x*v2y - v1y*v2x;*/
			GetFaceNormal(i, &fnx, &fny, &fnz);

			vert[f1].nx += fnx; vert[f1].ny += fny; vert[f1].nz += fnz; //faceForVert[f1]++;
			vert[f2].nx += fnx; vert[f2].ny += fny; vert[f2].nz += fnz; //faceForVert[f2]++;
			vert[f3].nx += fnx; vert[f3].ny += fny; vert[f3].nz += fnz; //faceForVert[f3]++;
		}

		for (i = 0; i < vertCount; i++)
		{
			float dis = sqrt(vert[i].nx*vert[i].nx + vert[i].ny*vert[i].ny + vert[i].nz*vert[i].nz);
			if (dis == 0)
			{
				vert[i].nx = vert[i].ny = 0.0f;
				vert[i].nz = 1.0f;
			}
			else
			{
				vert[i].nx /= dis;
				vert[i].ny /= dis;
				vert[i].nz /= dis;
			}
		}

		//delete[] faceCount;
	}
#endif
};

class SEQ : public Chunk
{
public:
	string	name;		//(0x50 bytes)
	int	start, end;
	float	moveSpeed;
	int	nonLooping;		//(0:loop; 1:no loop)
	float	rarity;
	float	boundsRadius;
	float	minExtx, minExty, minExtz;
	float	maxExtx, maxExty, maxExtz;

	SEQ()
	{
		name = "";
		nonLooping = LOOPING_ON;
		start = end = 0;
		moveSpeed = rarity = 0;
		boundsRadius = 0;
		minExtx = minExty = minExtz = 0;
		maxExtx = maxExty = maxExtz = 0;
	}

	void MdxRead(ifstream &in)
	{
		name = MdxReadString(in, 80);
		MdxReadInt(in, &start);
		MdxReadInt(in, &end);
		MdxReadFloat(in, &moveSpeed);
		MdxReadInt(in, &nonLooping);
		MdxReadFloat(in, &rarity);
		MdxReadInt(in);
		MdxReadFloat(in, 7, &boundsRadius, &minExtx, &minExty, &minExtz, &maxExtx, &maxExty, &maxExtz);
	}

	void MdlWrite(FILE *out)
	{
		fprintf(out, "\tAnim \"%s\" {\n", name.c_str());
		fprintf(out, "\t\tInterval { %d, %d },\n", start, end);
		if (nonLooping == LOOPING_OFF)
			fprintf(out, "\t\tNonLooping,\n");
		if (moveSpeed != 0) fprintf(out, "\t\tMoveSpeed %f,\n", moveSpeed);
		if (rarity != 0) fprintf(out, "\t\tRarity %f,\n", rarity);
		if (boundsRadius != 0 || minExtx != 0 || minExty != 0 || minExtz != 0 || maxExtx != 0 || maxExty != 0 || maxExtz != 0)
		{
			fprintf(out, "\t\tMinimumExtent { %f, %f, %f },\n", minExtx, minExty, minExtz);
			fprintf(out, "\t\tMaximumExtent { %f, %f, %f },\n", maxExtx, maxExty, maxExtz);
			fprintf(out, "\t\tBoundsRadius %f,\n", boundsRadius);
		}
		fprintf(out, "\t}\n");
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		name = line->label;
		while (line->type != LINE_EOF) {
			line->init(in);
			if (line->type == LINE_BRACED_LABELED_DATA && line->f_listMatch && line->label == "MaximumExtent") {
				maxExtx = line->f_listData[0];
				maxExty = line->f_listData[1];
				maxExtz = line->f_listData[2];
			}
			else if (line->type == LINE_BRACED_LABELED_DATA && line->f_listMatch && line->label == "MinimumExtent") {
				minExtx = line->f_listData[0];
				minExty = line->f_listData[1];
				minExtz = line->f_listData[2];
			}
			else if (line->type == LINE_LABELED_DATA && line->f_match && line->label == "BoundsRadius")
				boundsRadius = line->f_data;
			else if (line->type == LINE_BRACED_LABELED_DATA && line->i_listMatch && line->label == "Interval") {
				start = line->i_listData[0];
				end = line->i_listData[1];
			}
			else if (line->type == LINE_LABELED_DATA && line->f_match && line->label == "Rarity")
				rarity = line->f_data;
			else if (line->type == LINE_LABELED_DATA && line->f_match && line->label == "MoveSpeed")
				moveSpeed = line->f_data;
			else if (line->type == LINE_FLAG && line->label == "NonLooping")
				nonLooping = 1;
			else if (line->type == LINE_CLOSER)
				return;
			else {
				err->add(line->lineText, line->lineCounter, "Unexpected in a Sequence chunk");
			}
		}
	}

	void MdxWrite(ofstream &out)
	{
		MdxWriteString(out, name, 80);
		MdxWriteInt(out, start);
		MdxWriteInt(out, end);
		MdxWriteFloat(out, moveSpeed);
		MdxWriteInt(out, nonLooping);
		MdxWriteFloat(out, rarity);
		MdxWriteInt(out, 0);
		MdxWriteFloat(out, boundsRadius);
		MdxWriteFloat(out, minExtx);
		MdxWriteFloat(out, minExty);
		MdxWriteFloat(out, minExtz);
		MdxWriteFloat(out, maxExtx);
		MdxWriteFloat(out, maxExty);
		MdxWriteFloat(out, maxExtz);
	}
};

class VERS :public Chunk
{
public:
	int version;

	VERS() {
		version = 800;
	}

	void MdxWrite(ofstream &out)
	{
		out.write("VERS", 4);
		MdxWriteInt(out, 4); // Byte count
		MdxWriteInt(out, version);
	}

	void MdxRead(ifstream &in)
	{
		int len;
		MdxReadInt(in, &len);
		MdxReadInt(in, &version);
	}

	void MdlWrite(FILE *out)
	{
		fprintf(out, "Version {\n");
		fprintf(out, "\tFormatVersion %d,\n", version);
		fprintf(out, "}\n");
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		while (line->type != LINE_EOF) {
			line->init(in);
			if (line->type == LINE_LABELED_DATA && line->i_match && line->label == "FormatVersion")
				version = line->i_data;
			else if (line->type == LINE_CLOSER) {
				return;
			}
			else {
				err->add(line->lineText, line->lineCounter, "Unexpected in a Version chunk");
			}
		}
	}
};

class ATCH :public Chunk
{
public:
	string path; // 256 chars
	int attachmentID;
	OBJ obj;

	ATCH()
	{
		obj.type = TYPE_ATTACHMENT;
		path = "";
		attachmentID = -1;
	}

	void MdxWrite(ofstream &out)
	{
		MdxWriteInt(out, ByteCount());

		KMTA temp_katv = obj.katv;
		KMTA emptyKatv;
		obj.katv = emptyKatv;

		obj.MdxWrite(out);
		MdxWriteString(out, path, 256);
		MdxWriteInt(out, 0);
		MdxWriteInt(out, attachmentID);

		temp_katv.MdxWrite(out, "KATV");
		obj.katv = temp_katv;
	}

	void MdxRead(ifstream &in)
	{
		int len;
		int end = in.tellg();
		len = MdxReadInt(in);
		end += len;
		obj.MdxRead(in);
		// int i = in.tellg();
		path = MdxReadString(in, 256);
		MdxReadInt(in);
		attachmentID = MdxReadInt(in);
		char tag[5];
		tag[4] = 0;
		while (in.tellg() < end)
		{
			in.read(tag, 4);
			if (strcmp(tag, "KATV") == 0)
				obj.katv.MdxRead(in);
			else {
				assert(false);
			}
		}
		assert((int)in.tellg() == end);
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err, int defAttachmentID)
	{
		obj.name = line->label;

		attachmentID = defAttachmentID;
		do {
			line->init(in);

			if (line->type == LINE_LABELED_DATA && line->i_match && strmatch(line->label, "attachmentid")) {
				attachmentID = line->i_data;
			}
			else if (line->type == LINE_LABELED_STRING_DATA && strmatch(line->chunk, "path")) {
				path = line->label;
			}
			else if (obj.MdlReadLine(in, line, err)) {

			}
			else if (line->type == LINE_EOF || line->type == LINE_CLOSER)
				break;
			else {
				err->add(line->lineText, line->lineCounter, "Unexpected in a Light chunk");
				break;
			}
		} while (true);
	}

	void MdlWrite(FILE *out, int defAttachmentID)
	{
		fprintf(out, "Attachment \"%s\" {\n", obj.name.c_str());
		obj.MdlWriteProps(out);

		if (attachmentID != defAttachmentID)
		{
			fprintf(out, "\tAttachmentID %d,\n", attachmentID);
		}
		if (path.length() > 0)
		{
			fprintf(out, "\tPath \"%s\",\n", path.c_str());
		}
		obj.MdlWriteAnims(out);
		fprintf(out, "}\n");
	}

	int ByteCount()
	{
		return
			obj.ByteCount() +
			256 +	// path
			12		// attachmentID, inclusive lengths, int ???
			;
	}
};




class CAMS :public Chunk
{
public:
	KGSC targetMovement;
	KGSC movement;
	KMTA roll;
	string name;
	float posX, posY, posZ;
	float fieldOfView;
	float farClip, nearClip;
	float targetX, targetY, targetZ;


	CAMS(): posX(0), posY(0), posZ(0), fieldOfView(0), farClip(0), nearClip(0), targetX(0), targetY(0), targetZ(0)
	{
		name = "";
	}

	void MdxWrite(ofstream &out)
	{
		MdxWriteInt(out, ByteCount());
		MdxWriteString(out, name, 80);
		MdxWriteFloats(out, 4, posX, posY, posZ, fieldOfView);
		MdxWriteFloat(out, farClip);
		MdxWriteFloat(out, nearClip);
		MdxWriteFloats(out, 3, targetX, targetY, targetZ);
		targetMovement.MdxWrite(out, "KCTR");
		roll.MdxWrite(out, "KCRL");
		movement.MdxWrite(out, "KTTR");
	}

	void MdxRead(ifstream &in)
	{
		int len;
		int end = in.tellg();
		len = MdxReadInt(in);
		end += len;


		name = MdxReadString(in, 80);
		MdxReadFloat(in, 4, &posX, &posY, &posZ, &fieldOfView);

		farClip = MdxReadFloat(in);
		nearClip = MdxReadFloat(in);
		//, &farClip, &nearClip, &targetX, &targetY, &targetZ);

		MdxReadFloat(in, 3, &targetX, &targetY, &targetZ);

		char tag[5];
		tag[4] = 0;
		while (in.tellg() < end)
		{
			in.read(tag, 4);
			if (strcmp(tag, "KCTR") == 0)
				targetMovement.MdxRead(in);
			else if (strcmp(tag, "KCRL") == 0)
				roll.MdxRead(in);
			else if (strcmp(tag, "KTTR") == 0)
				movement.MdxRead(in);
			else {
				assert(false);
			}
		}
		assert((int)in.tellg() == end);
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		name = line->label;
		/*
		Camera <string_name> {
			Position { <float_x>, <float_y>, <float_z> },
			(Translation { <float_x>, <float_y>, <float_z> })
			(Rotation { <float_a>, <float_b>, <float_c>, <float_d> })
			FieldOfView <float>,
			FarClip <float>,
			NearClip <float>,
			Target {
				Position { <float_x>, <float_y>, <float_z> },
				(Translation { <float_x>, <float_y>, <float_z> })
			}
		}*/

		do {
			line->init(in);

			if (line->type == LINE_BRACED_LABELED_DATA && line->f_listMatch && line->f_listData.size() == 3 && strmatch(line->label, "position")) {
				posX = line->f_listData[0];
				posY = line->f_listData[1];
				posZ = line->f_listData[2];

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "translation")) {
				movement.MdlRead(in, line, err);
			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "rotation")) {
				roll.MdlRead(in, line, err);

			}
			else if (line->type == LINE_LABELED_DATA && strmatch(line->label, "fieldofview") && line->f_match) {
				fieldOfView = line->f_data;
			}
			else if (line->type == LINE_LABELED_DATA && strmatch(line->label, "farclip") && line->f_match) {
				farClip = line->f_data;
			}
			else if (line->type == LINE_LABELED_DATA && strmatch(line->label, "nearclip") && line->f_match) {
				nearClip = line->f_data;
			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "target")) {
				do {
					line->init(in);
					if (line->type == LINE_EOF || line->type == LINE_CLOSER)
					{
						break;
					}
					else if (line->type == LINE_BRACED_LABELED_DATA && line->f_listMatch && line->f_listData.size() == 3 && strmatch(line->label, "position")) {
						targetX = line->f_listData[0];
						targetY = line->f_listData[1];
						targetZ = line->f_listData[2];
					}
					else if (line->type == LINE_OPENER && strmatch(line->chunk, "translation")) {
						targetMovement.MdlRead(in, line, err);
					}
					else {
						err->add(line->lineText, line->lineCounter, "Unexpected in a Camera-target chunk");
						break;
					}
				} while (!err->critical);
			}
			else if (line->type == LINE_EOF || line->type == LINE_CLOSER)
				break;
			else {
				err->add(line->lineText, line->lineCounter, "Unexpected in a Camera chunk");
				break;
			}
		} while (!err->critical);
	}

	void MdlWrite(FILE *out)
	{
		fprintf(out, "Camera \"%s\" {\n", name.c_str());
		fprintf(out, "\tPosition { %f, %f, %f },\n", posX, posY, posZ);
		if (movement.key.size() > 0)
			movement.MdlWrite(out, "Translation", "\t");
		if (roll.key.size() > 0)
			roll.MdlWrite(out, "Rotation", "\t");
		fprintf(out, "\tFieldOfView %f,\n", fieldOfView);
		fprintf(out, "\tFarClip %f,\n", farClip);
		fprintf(out, "\tNearClip %f,\n", nearClip);
		fprintf(out, "\tTarget {\n");
		fprintf(out, "\t\tPosition { %f, %f, %f },\n", targetX, targetY, targetZ);
		if (targetMovement.key.size() > 0)
			targetMovement.MdlWrite(out, "Translation", "\t\t");
		fprintf(out, "\t}\n");
		fprintf(out, "}\n");
	}

	int ByteCount()
	{
		return
			roll.ByteCount() +
			movement.ByteCount() +
			targetMovement.ByteCount() +
			120
			;
	}
};

class PREM :public Chunk
{
public:
	OBJ obj;
	KMTA emissionRate;	//KPEE
	KMTA gravity;		//KPEG
	KMTA longitude;		//KPLN
	KMTA latitude;		//KPLT
	KMTA visibility;	//KPEV
	string modelPath;
	KMTA lifeSpan;		//KPES
	KMTA initVelocity;	//KPEL

	PREM()
	{
		obj.type = TYPE_PARTICLE_EMITTER;
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		obj.name = line->label;
		modelPath = "";

		obj.type = TYPE_PARTICLE_EMITTER;

		do {
			line->init(in);

			if (line->type == LINE_FLAG && strmatch(line->label, "emitterusesmdl")) {
				obj.type |= PREM_USES_MDL;
				obj.type &= ~PREM_USES_TGA;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "emitterusestga")) {
				obj.type |= PREM_USES_TGA;
				obj.type &= ~PREM_USES_MDL;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "gravity")) {
				gravity.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "gravity")) {
				gravity.staticState = line->f_data;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "longitude")) {
				longitude.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "longitude")) {
				longitude.staticState = line->f_data;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "latitude")) {
				latitude.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "latitude")) {
				latitude.staticState = line->f_data;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "emissionrate")) {
				emissionRate.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "emissionrate")) {
				emissionRate.staticState = line->f_data;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "visibility")) {
				visibility.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "visibility")) {
				visibility.staticState = line->f_data;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "particle")) {
				do {
					line->init(in);

					if (line->type == LINE_EOF || line->type == LINE_CLOSER) {
						break;

					}
					else if (line->type == LINE_OPENER && strmatch(line->chunk, "lifespan")) {
						lifeSpan.MdlRead(in, line, err);
					}
					else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "lifespan")) {
						lifeSpan.staticState = line->f_data;

					}
					else if (line->type == LINE_OPENER && strmatch(line->chunk, "initvelocity")) {
						initVelocity.MdlRead(in, line, err);
					}
					else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "initvelocity")) {
						initVelocity.staticState = line->f_data;
					}
					else if (line->type == LINE_LABELED_STRING_DATA && strmatch(line->chunk, "path")) {
						modelPath = line->label;
					}
					else {
						err->add(line->lineText, line->lineCounter, "Unexpected in a ParticleEmitter's Particle chunk");
						break;
					}
				} while (true);

			}
			else if (obj.MdlReadLine(in, line, err)) {

			}
			else if (line->type == LINE_EOF || line->type == LINE_CLOSER)
				break;
			else {
				err->add(line->lineText, line->lineCounter, "Unexpected in a ParticleEmitter chunk");
				break;
			}
		} while (true);
	}

	void MdlWrite(FILE *out)
	{
		fprintf(out, "ParticleEmitter \"%s\" {\n", obj.name.c_str());
		fprintf(out, "\tObjectId %d,\n", obj.objectID);
		if (obj.parent != -1)
			fprintf(out, "\tParent %d,\n", obj.parent);

		if ((obj.type&PREM_USES_TGA) == PREM_USES_TGA)
			fprintf(out, "\tEmitterUsesTGA,\n");
		else
		{
			assert((obj.type&PREM_USES_MDL) == PREM_USES_MDL);
			fprintf(out, "\tEmitterUsesMDL,\n");
		}

		emissionRate.MdlWrite(out, "EmissionRate", "\t");
		gravity.MdlWrite(out, "Gravity", "\t");
		longitude.MdlWrite(out, "Longitude", "\t");
		latitude.MdlWrite(out, "Latitude", "\t");
		visibility.MdlWrite(out, "Visibility", "\t");
		fprintf(out, "\tParticle {\n");
		lifeSpan.MdlWrite(out, "LifeSpan", "\t\t");
		initVelocity.MdlWrite(out, "InitVelocity", "\t\t");
		fprintf(out, "\t\tPath \"%s\",\n", modelPath.c_str());
		fprintf(out, "\t}\n");

		fprintf(out, "}\n");
	}

	void MdxWrite(ofstream &out)
	{
		MdxWriteInt(out, ByteCount());
		MdxWriteInt(out, ByteCountKG());
		MdxWriteString(out, obj.name, 80);
		MdxWriteInt(out, obj.objectID);
		MdxWriteInt(out, obj.parent);
		MdxWriteInt(out, obj.type);
		obj.kgtr.MdxWrite(out, "KGTR");
		obj.kgrt.MdxWrite(out, "KGRT");
		obj.kgsc.MdxWrite(out, "KGSC");

		MdxWriteFloat(out, emissionRate.staticState);
		MdxWriteFloat(out, gravity.staticState);
		MdxWriteFloat(out, longitude.staticState);
		MdxWriteFloat(out, latitude.staticState);
		MdxWriteString(out, modelPath, 256);
		MdxWriteInt(out, 0);
		MdxWriteFloat(out, lifeSpan.staticState);
		MdxWriteFloat(out, initVelocity.staticState);

		emissionRate.MdxWrite(out, "KPEE");
		gravity.MdxWrite(out, "KPEG");
		longitude.MdxWrite(out, "KPLN");
		latitude.MdxWrite(out, "KPLT");
		lifeSpan.MdxWrite(out, "KPEL");
		initVelocity.MdxWrite(out, "KPES");
		visibility.MdxWrite(out, "KPEV");
	}

	void MdxRead(ifstream &in)
	{
		int start = in.tellg();
		int end = start + MdxReadInt(in);
		int kgEnd = start + MdxReadInt(in) + 4;

		obj.name = MdxReadString(in, 80);
		obj.objectID = MdxReadInt(in);
		obj.parent = MdxReadInt(in);
		obj.type = MdxReadInt(in);

		char tag[5];
		tag[4] = 0;
		while (in.tellg() < kgEnd)
		{
			in.read(tag, 4);
			if (strcmp(tag, "KGTR") == 0)
				obj.kgtr.MdxRead(in);
			else if (strcmp(tag, "KGRT") == 0)
				obj.kgrt.MdxRead(in);
			else if (strcmp(tag, "KGSC") == 0)
				obj.kgsc.MdxRead(in);
			else {
				assert(false);
			}
		}
		assert((int)in.tellg() == kgEnd);
		emissionRate.staticState = MdxReadFloat(in);
		gravity.staticState = MdxReadFloat(in);
		longitude.staticState = MdxReadFloat(in);
		latitude.staticState = MdxReadFloat(in);

		modelPath = MdxReadString(in, 256);
		MdxReadInt(in);

		lifeSpan.staticState = MdxReadFloat(in);
		initVelocity.staticState = MdxReadFloat(in);

		while (in.tellg() < end)
		{
			in.read(tag, 4);
			if (strcmp(tag, "KPEE") == 0)
				emissionRate.MdxRead(in);
			else if (strcmp(tag, "KPEG") == 0)
				gravity.MdxRead(in);
			else if (strcmp(tag, "KPLN") == 0)
				longitude.MdxRead(in);
			else if (strcmp(tag, "KPLT") == 0)
				latitude.MdxRead(in);
			else if (strcmp(tag, "KPEL") == 0)
				lifeSpan.MdxRead(in);
			else if (strcmp(tag, "KPES") == 0)
				initVelocity.MdxRead(in);
			else if (strcmp(tag, "KPEV") == 0)
				visibility.MdxRead(in);

			else {
				assert(false);
			}
		}
		assert((int)in.tellg() == end);
	}

	int ByteCount()
	{
		return
			ByteCountKG() +
			emissionRate.ByteCount() +
			gravity.ByteCount() +
			longitude.ByteCount() +
			latitude.ByteCount() +
			lifeSpan.ByteCount() +
			initVelocity.ByteCount() +
			visibility.ByteCount() +
			288
			;
	}
	int ByteCountKG()
	{
		return
			obj.kgtr.ByteCount() +
			obj.kgrt.ByteCount() +
			obj.kgsc.ByteCount() +
			96
			;
	}
};

class PRE2 :public Chunk
{
public:
	OBJ obj;
	int flags;

	float lifespan;
	int filterMode;
	int rows, cols;
	int flag2;
	float tailLength;
	float time;

	float r[3], g[3], b[3];
	unsigned char a[3];
	float particleScaling[3];
	int lifeSpanUVAnim[3];
	int decayUVAnim[3];
	int tailUVAnim[3];
	int tailDecayUVAnim[3];
	int textureId;
	int squirt;
	int priorityPlane;
	int replaceableId;

	KMTA speed; //KP2S
	KMTA latitude; // KP2L
	KMTA emissionRate; // KP2E
	KMTA visibility; //KP2V
	KMTA length; //KP2N
	KMTA width; //KP2W
	KMTA gravity; //????
	KMTA variation; //????

	PRE2(): flags(0), lifespan(0), filterMode(0), rows(0), cols(0), flag2(0), tailLength(0), time(0), r{}, g{}, b{},
	        a{}, particleScaling{}, lifeSpanUVAnim{}, decayUVAnim{}, tailUVAnim{}, tailDecayUVAnim{}, textureId(0)
	{
		squirt = 0;
		priorityPlane = 0;
		replaceableId = 0;
	}

	void MdxWrite(ofstream &out)
	{
		MdxWriteInt(out, ByteCount());
		MdxWriteInt(out, ByteCountKG());


		//nbytesi
		//nbytesikg

		MdxWriteString(out, obj.name, 0x50);
		MdxWriteInt(out, obj.objectID);
		MdxWriteInt(out, obj.parent);
		MdxWriteInt(out, flags);
		obj.kgtr.MdxWrite(out, "KGTR");
		obj.kgrt.MdxWrite(out, "KGRT");
		obj.kgsc.MdxWrite(out, "KGSC");

		MdxWriteFloat(out, speed.staticState);
		MdxWriteFloat(out, variation.staticState);
		MdxWriteFloat(out, latitude.staticState);
		MdxWriteFloat(out, gravity.staticState);
		MdxWriteFloat(out, lifespan);
		MdxWriteFloat(out, emissionRate.staticState);
		MdxWriteFloat(out, length.staticState);
		MdxWriteFloat(out, width.staticState);
		MdxWriteInt(out, filterMode);
		MdxWriteInt(out, rows);
		MdxWriteInt(out, cols);
		MdxWriteInt(out, flag2);
		MdxWriteFloat(out, tailLength);
		MdxWriteFloat(out, time);
		int i;
		for (i = 0; i < 3; i++)
		{
			MdxWriteFloat(out, r[i]);
			MdxWriteFloat(out, g[i]);
			MdxWriteFloat(out, b[i]);
		}

		for (i = 0; i < 3; i++)
			MdxWriteByte(out, a[i]);

		for (i = 0; i < 3; i++)
			MdxWriteFloat(out, particleScaling[i]);
		for (i = 0; i < 3; i++)
			MdxWriteInt(out, lifeSpanUVAnim[i]);
		for (i = 0; i < 3; i++)
			MdxWriteInt(out, decayUVAnim[i]);
		for (i = 0; i < 3; i++)
			MdxWriteInt(out, tailUVAnim[i]);
		for (i = 0; i < 3; i++)
			MdxWriteInt(out, tailDecayUVAnim[i]);

		MdxWriteInt(out, textureId);
		MdxWriteInt(out, squirt);
		MdxWriteInt(out, priorityPlane);
		MdxWriteInt(out, replaceableId);


		speed.MdxWrite(out, "KP2S");
		variation.MdxWrite(out, "KP2R");
		latitude.MdxWrite(out, "KP2L");
		gravity.MdxWrite(out, "KP2G");
		emissionRate.MdxWrite(out, "KP2E");
		visibility.MdxWrite(out, "KP2V");
		length.MdxWrite(out, "KP2N");
		width.MdxWrite(out, "KP2W");


	}

	void MdxRead(ifstream &in)
	{
		int end = in.tellg();
		int byteCount = MdxReadInt(in);//679
		end += byteCount;
		int byteCountWithKG = MdxReadInt(in);//472
		// int kgBytes = byteCount - byteCountWithKG;//207
		obj.name = MdxReadString(in, 80);
		obj.objectID = MdxReadInt(in);
		obj.parent = MdxReadInt(in);
		flags = MdxReadInt(in);

		int kgEnd = (int)in.tellg() + byteCountWithKG - 96;//1820

		char tag[5];
		tag[4] = 0;
		while (in.tellg() < kgEnd)//1348 -> 1724 = 376
		{
			in.read(tag, 4);
			if (strcmp(tag, "KGTR") == 0)
				obj.kgtr.MdxRead(in);
			else if (strcmp(tag, "KGRT") == 0)
				obj.kgrt.MdxRead(in);
			else if (strcmp(tag, "KGSC") == 0)
				obj.kgsc.MdxRead(in);
			else {
				//string s = name;
				//int foo = in.tellg();
				assert(false);
			}
		}
		assert((int)in.tellg() == kgEnd);

		MdxReadFloat(in, 8, &speed.staticState, &variation.staticState, &latitude.staticState, &gravity.staticState, &lifespan, &emissionRate.staticState, &length.staticState, &width.staticState);
		//gohere
		filterMode = MdxReadInt(in);
		rows = MdxReadInt(in);
		cols = MdxReadInt(in);
		flag2 = MdxReadInt(in);
		tailLength = MdxReadFloat(in);
		time = MdxReadFloat(in);
		int i;
		for (i = 0; i < 3; i++) {
			r[i] = MdxReadFloat(in);
			g[i] = MdxReadFloat(in);
			b[i] = MdxReadFloat(in);
		}
		for (i = 0; i < 3; i++) {
			a[i] = MdxReadByte(in);
		}

		for (i = 0; i < 3; i++) particleScaling[i] = MdxReadFloat(in);
		for (i = 0; i < 3; i++) lifeSpanUVAnim[i] = MdxReadInt(in);
		for (i = 0; i < 3; i++) decayUVAnim[i] = MdxReadInt(in);
		for (i = 0; i < 3; i++) tailUVAnim[i] = MdxReadInt(in);
		for (i = 0; i < 3; i++)
			tailDecayUVAnim[i] = MdxReadInt(in);
		textureId = MdxReadInt(in);
		squirt = MdxReadInt(in);
		priorityPlane = MdxReadInt(in);
		replaceableId = MdxReadInt(in);

		/*
				(KP2S)
				(KP2L)
				(KP2E)
				(KP2V)
				(KP2N)
				(KP2W)
		*/
		while (in.tellg() < end)
		{
			in.read(tag, 4);
			if (strcmp(tag, "KP2S") == 0)
				speed.MdxRead(in);
			else if (strcmp(tag, "KP2L") == 0)
				latitude.MdxRead(in);
			else if (strcmp(tag, "KP2E") == 0)
				emissionRate.MdxRead(in);
			else if (strcmp(tag, "KP2V") == 0)
				visibility.MdxRead(in);
			else if (strcmp(tag, "KP2N") == 0)
				length.MdxRead(in);
			else if (strcmp(tag, "KP2W") == 0)
				width.MdxRead(in);
			else if (strcmp(tag, "KP2G") == 0)
				gravity.MdxRead(in);
			else if (strcmp(tag, "KP2R") == 0)
				variation.MdxRead(in);

			else {
				assert(false);
			}
		}
		assert((int)in.tellg() == end);
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		obj.name = line->label;
		squirt = 0;
		priorityPlane = 0;
		replaceableId = 0;
		flags = PRE2_ALWAYS;

		do {
			line->init(in);

			if (line->type == LINE_LABELED_FLAG && strmatch(line->chunk, "dontinherit") && strmatch(line->label, "rotation")) {
				flags |= PRE2_DONT_INHERIT_ROTATION;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "SortPrimsFarZ")) {
				flags |= PRE2_SORT_PRIMS_FAR_Z;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "Unshaded")) {
				flags |= PRE2_UNSHADED;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "LineEmitter")) {
				flags |= PRE2_LINE_EMITTER;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "Unfogged")) {
				flags |= PRE2_UNFOGGED;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "ModelSpace")) {
				flags |= PRE2_MODEL_SPACE;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "XYQuad")) {
				flags |= PRE2_XY_QUAD;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "squirt")) {
				squirt = 1;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "speed")) {
				speed.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "speed")) {
				speed.staticState = line->f_data;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "variation")) {
				variation.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "variation")) {
				variation.staticState = line->f_data;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "latitude")) {
				latitude.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "latitude")) {
				latitude.staticState = line->f_data;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "gravity")) {
				gravity.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "gravity")) {
				gravity.staticState = line->f_data;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "visibility")) {
				visibility.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "visibility")) {
				visibility.staticState = line->f_data;

			}
			else if (line->type == LINE_LABELED_DATA && strmatch(line->label, "lifespan")) {
				lifespan = line->f_data;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "emissionrate")) {
				emissionRate.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "emissionrate")) {
				emissionRate.staticState = line->f_data;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "width")) {
				width.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "width")) {
				width.staticState = line->f_data;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "length")) {
				length.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "length")) {
				length.staticState = line->f_data;

			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "blend")) {
				filterMode = PRE2_FILTER_BLEND;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "additive")) {
				filterMode = PRE2_FILTER_ADDITIVE;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "modulate")) {
				filterMode = PRE2_FILTER_MODULATE;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "alphakey")) {
				filterMode = PRE2_FILTER_ALPHAKEY;

			}
			else if (line->type == LINE_LABELED_DATA && line->i_match && strmatch(line->label, "rows")) {
				rows = line->i_data;
			}
			else if (line->type == LINE_LABELED_DATA && line->i_match && strmatch(line->label, "columns")) {
				cols = line->i_data;

			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "head")) {
				flag2 = 0;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "tail")) {
				flag2 = 1;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "both")) {
				flag2 = 2;

			}
			else if (line->type == LINE_LABELED_DATA && line->f_match && strmatch(line->label, "taillength")) {
				tailLength = line->f_data;
			}
			else if (line->type == LINE_LABELED_DATA && line->f_match && strmatch(line->label, "time")) {
				time = line->f_data;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "SegmentColor"))
			{
				for (int i = 0; i < 3; i++)
				{
					line->init(in);
					if (line->type == LINE_BRACED_LABELED_DATA && strmatch(line->label, "color") && line->f_listMatch && line->f_listData.size() == 3) {
						b[i] = line->f_listData[0];
						g[i] = line->f_listData[1];
						r[i] = line->f_listData[2];
					}
					else
					{
						err->add(line->lineText, line->lineCounter, "Expected a color.");
					}
				}
				line->init(in);
				if (line->type != LINE_CLOSER)
				{
					err->add(line->lineText, line->lineCounter, "Expected a closing brace to end the SegmentColor section.");
				}
			}
			else if (line->type == LINE_BRACED_LABELED_DATA && strmatch(line->label, "alpha") && line->i_listMatch && line->i_listData.size() == 3) {
				for (int i = 0; i < 3; i++)
					a[i] = line->i_listData[i];
			}
			else if (line->type == LINE_BRACED_LABELED_DATA && strmatch(line->label, "ParticleScaling") && line->f_listMatch && line->f_listData.size() == 3) {
				for (int i = 0; i < 3; i++)
					particleScaling[i] = line->f_listData[i];
			}
			else if (line->type == LINE_BRACED_LABELED_DATA && strmatch(line->label, "LifeSpanUVAnim") && line->i_listMatch && line->i_listData.size() == 3) {
				for (int i = 0; i < 3; i++)
					lifeSpanUVAnim[i] = line->i_listData[i];
			}
			else if (line->type == LINE_BRACED_LABELED_DATA && strmatch(line->label, "DecayUVAnim") && line->i_listMatch && line->i_listData.size() == 3) {
				for (int i = 0; i < 3; i++)
					decayUVAnim[i] = line->i_listData[i];
			}
			else if (line->type == LINE_BRACED_LABELED_DATA && strmatch(line->label, "TailUVAnim") && line->i_listMatch && line->i_listData.size() == 3) {
				for (int i = 0; i < 3; i++)
					tailUVAnim[i] = line->i_listData[i];
			}
			else if (line->type == LINE_BRACED_LABELED_DATA && strmatch(line->label, "TailDecayUVAnim") && line->i_listMatch && line->i_listData.size() == 3) {
				for (int i = 0; i < 3; i++)
					tailDecayUVAnim[i] = line->i_listData[i];

			}
			else if (line->type == LINE_LABELED_DATA && line->i_match && strmatch(line->label, "TextureID")) {
				textureId = line->i_data;
			}
			else if (line->type == LINE_LABELED_DATA && line->i_match && strmatch(line->label, "ReplaceableId")) {
				replaceableId = line->i_data;
			}
			else if (line->type == LINE_LABELED_DATA && line->i_match && strmatch(line->label, "PriorityPlane")) {
				priorityPlane = line->i_data;

				//} else if (line->type==LINE_OPENER && strmatch(line->chunk,"visibility")) {
				//	obj.katv.MdlRead(in, line, err);
			}
			else if (obj.MdlReadLine(in, line, err)) {

			}
			else if (line->type == LINE_EOF || line->type == LINE_CLOSER)
				break;
			else {
				err->add(line->lineText, line->lineCounter, "Unexpected in a ParticleEmitter2 chunk");
				break;
			}
		} while (true);
	}

	void MdlWrite(FILE *out)
	{
		fprintf(out, "ParticleEmitter2 \"%s\" {\n", obj.name.c_str());
		fprintf(out, "\tObjectId %d,\n", obj.objectID);
		if (obj.parent != -1)
			fprintf(out, "\tParent %d,\n", obj.parent);

		if ((flags&PRE2_DONT_INHERIT_ROTATION) == PRE2_DONT_INHERIT_ROTATION)
			fprintf(out, "\tDontInherit { Rotation },\n");
		if ((flags&PRE2_SORT_PRIMS_FAR_Z) == PRE2_SORT_PRIMS_FAR_Z)
			fprintf(out, "\tSortPrimsFarZ,\n");
		if ((flags&PRE2_UNSHADED) == PRE2_UNSHADED)
			fprintf(out, "\tUnshaded,\n");
		if ((flags&PRE2_LINE_EMITTER) == PRE2_LINE_EMITTER)
			fprintf(out, "\tLineEmitter,\n");
		if ((flags&PRE2_UNFOGGED) == PRE2_UNFOGGED)
			fprintf(out, "\tUnfogged,\n");
		if ((flags&PRE2_MODEL_SPACE) == PRE2_MODEL_SPACE)
			fprintf(out, "\tModelSpace,\n");
		if ((flags&PRE2_XY_QUAD) == PRE2_XY_QUAD)
			fprintf(out, "\tXYQuad,\n");

		speed.MdlWrite(out, "Speed", "\t");
		variation.MdlWrite(out, "Variation", "\t");
		latitude.MdlWrite(out, "Latitude", "\t");
		gravity.MdlWrite(out, "Gravity", "\t");
		if (visibility.key.size() > 0 || visibility.staticState != 0.0f)
			visibility.MdlWrite(out, "Visibility", "\t");


		if (squirt == 1) fprintf(out, "\tSquirt,\n");
		fprintf(out, "\tLifeSpan %f,\n", lifespan);

		emissionRate.MdlWrite(out, "EmissionRate", "\t");
		width.MdlWrite(out, "Width", "\t");
		length.MdlWrite(out, "Length", "\t");

		string s;
		switch (filterMode)
		{
		case PRE2_FILTER_BLEND:    s = "Blend"; break;
		case PRE2_FILTER_ADDITIVE: s = "Additive"; break;
		case PRE2_FILTER_MODULATE: s = "Modulate"; break;
		case PRE2_FILTER_ALPHAKEY: s = "AlphaKey"; break;
		default: assert(false);
		}
		char fm[16];
		for (int i = 0; i < 16; i++) {
			if (i < s.length())
				fm[i] = s.c_str()[i];
			else
				fm[i] = 0;
		}

		fprintf(out, "\t%s,\n", fm);

		fprintf(out, "\tRows %d,\n", rows);
		fprintf(out, "\tColumns %d,\n", cols);

		switch (flag2)
		{
		case 0: s = "Head"; break;
		case 1: s = "Tail"; break;
		case 2: s = "Both"; break;
		default: ;
		}
		for (int i = 0; i < 16; i++) {
			if (i < s.length())
				fm[i] = s.c_str()[i];
			else
				fm[i] = 0;
		}

		fprintf(out, "\t%s,\n", fm);

		fprintf(out, "\tTailLength %f,\n", tailLength);
		fprintf(out, "\tTime %f,\n", time);
		fprintf(out, "\tSegmentColor {\n");
		for (int i = 0; i < 3; i++)
		{
			fprintf(out, "\t\tColor { %f, %f, %f },\n", b[i], g[i], r[i]);
		}
		fprintf(out, "\t},\n");

		fprintf(out, "\tAlpha { %d, %d, %d },\n", (int)a[0], (int)a[1], (int)a[2]);
		fprintf(out, "\tParticleScaling { %f, %f, %f },\n", particleScaling[0], particleScaling[1], particleScaling[2]);
		fprintf(out, "\tLifeSpanUVAnim { %d, %d, %d },\n", lifeSpanUVAnim[0], lifeSpanUVAnim[1], lifeSpanUVAnim[2]);
		fprintf(out, "\tDecayUVAnim { %d, %d, %d },\n", decayUVAnim[0], decayUVAnim[1], decayUVAnim[2]);
		fprintf(out, "\tTailUVAnim { %d, %d, %d },\n", tailUVAnim[0], tailUVAnim[1], tailUVAnim[2]);
		fprintf(out, "\tTailDecayUVAnim { %d, %d, %d },\n", tailDecayUVAnim[0], tailDecayUVAnim[1], tailDecayUVAnim[2]);

		fprintf(out, "\tTextureID %d,\n", textureId);
		if (replaceableId != 0)
			fprintf(out, "\tReplaceableId %d,\n", replaceableId);
		if (priorityPlane != 0)
			fprintf(out, "\tPriorityPlane %d,\n", priorityPlane);

		if (obj.kgtr.key.size() > 0)
			obj.kgtr.MdlWrite(out, "Translation", "\t");
		if (obj.kgrt.key.size() > 0)
			obj.kgrt.MdlWrite(out, "Rotation", "\t");
		if (obj.kgsc.key.size() > 0)
			obj.kgsc.MdlWrite(out, "Scaling", "\t");

		fprintf(out, "}\n");
	}

	int ByteCount()
	{
		return
			obj.kgtr.ByteCount() +
			obj.kgrt.ByteCount() +
			obj.kgsc.ByteCount() +
			//-169+
			80 + // name
			48 * 4 - 1 + // atts: that -1 is -9 for bytes instead of floats, +8 for the lens
			speed.ByteCount() +
			emissionRate.ByteCount() +
			visibility.ByteCount() +
			latitude.ByteCount() +
			width.ByteCount() +
			length.ByteCount() +
			gravity.ByteCount() +
			variation.ByteCount()
			;

	}
	int ByteCountKG()
	{
		return
			obj.kgtr.ByteCount() +
			obj.kgrt.ByteCount() +
			obj.kgsc.ByteCount() +
			96
			;
	}


};


class GEOA : public Chunk
{
public:
	KGSC kgac; // color
	KMTA kgao; // visibility

	//float staticAlpha;				//(1.0:use KGAO)
	int	colorAnimation;				//(0:none;1:DropShadow;2:Color;3:Both)
	//float colorR, colorG, colorB;	//(default:1)
	int	geosetID;

	GEOA(): colorAnimation(0)
	{
		kgac.reverseOrder = true;
		kgac.staticX = kgac.staticY = kgac.staticZ = 1;
		kgao.staticState = 1;
		geosetID = 1;
	}

	void MdxWrite(ofstream &out)
	{
		MdxWriteInt(out, ByteCount());
		MdxWriteFloat(out, kgao.staticState);
		MdxWriteInt(out, colorAnimation);
		MdxWriteFloat(out, kgac.staticZ);
		MdxWriteFloat(out, kgac.staticY);
		MdxWriteFloat(out, kgac.staticX);
		MdxWriteInt(out, geosetID);
		kgao.MdxWrite(out, "KGAO");
		kgac.MdxWrite(out, "KGAC");
	}

	void MdlWrite(FILE *out)
	{
		fprintf(out, "GeosetAnim {\n");
		if ((colorAnimation & 1) == 1) fprintf(out, "\tDropShadow,\n");
		kgao.MdlWrite(out, "Alpha", "\t");
		if ((colorAnimation & 2) == 2 && (kgac.key.size() > 0 || kgac.staticX != 1.0f || kgac.staticY != 1.0f || kgac.staticZ != 1.0f))
			kgac.MdlWrite(out, "Color", "\t");

		fprintf(out, "\tGeosetId %d,\n", geosetID);
		fprintf(out, "}\n");
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		do {
			line->init(in);
			if (line->type == LINE_FLAG && strmatch(line->label, "dropshadow")) {
				colorAnimation += 1;
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "alpha")) {
				kgao.staticState = line->f_data;
			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "alpha")) {
				kgao.MdlRead(in, line, err);
			}
			else if (line->type == LINE_BRACED_STATIC_DATA && line->f_listData.size() == 3 && line->f_listMatch && strmatch(line->label, "color")) {
				kgac.staticX = line->f_listData[0];
				kgac.staticY = line->f_listData[1];
				kgac.staticZ = line->f_listData[2];
				colorAnimation |= 2;
			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "color")) {
				kgac.MdlRead(in, line, err);
				colorAnimation |= 2;
			}
			else if (line->type == LINE_LABELED_DATA && line->i_match && strmatch(line->label, "geosetid")) {
				geosetID = line->i_data;
			}
			else if (line->type == LINE_EOF || line->type == LINE_CLOSER)
				break;
			else {
				err->add(line->lineText, line->lineCounter, "Unexpected in a Model chunk");
				break;
			}
		} while (true);
	}

	void MdxRead(ifstream &in)
	{
		int start = in.tellg();
		int end = start + MdxReadInt(in);

		kgao.staticState = MdxReadFloat(in);
		colorAnimation = MdxReadInt(in);
		kgac.staticZ = MdxReadFloat(in);
		kgac.staticY = MdxReadFloat(in);
		kgac.staticX = MdxReadFloat(in);
		geosetID = MdxReadInt(in);

		char tag[5];
		tag[4] = 0;
		while (in.tellg() < end)
		{
			in.read(tag, 4);
			if (strcmp(tag, "KGAO") == 0)
				kgao.MdxRead(in);
			else if (strcmp(tag, "KGAC") == 0)
				kgac.MdxRead(in);
			else { assert(false); }
		}
		assert((int)in.tellg() == end);
	}

	int ByteCount() {
		return
			28 + // length (4b) + all those floats and longs
			kgao.ByteCount() +
			kgac.ByteCount()
			;
	}
};

class TXAN :public Chunk {
public:
	KGSC ktat;
	KGRT ktar;
	KGSC ktas;

	void MdxRead(ifstream &in)
	{
		int start = in.tellg();
		int end = start + MdxReadInt(in);

		char tag[5];
		tag[4] = 0;
		while (in.tellg() < end)
		{
			in.read(tag, 4);
			if (strcmp(tag, "KTAT") == 0)
				ktat.MdxRead(in);
			else if (strcmp(tag, "KTAR") == 0)
				ktar.MdxRead(in);
			else if (strcmp(tag, "KTAS") == 0)
				ktas.MdxRead(in);
			else { assert(false); }
		}
		assert((int)in.tellg() == end);
	}

	void MdlWrite(FILE *out)
	{
		fprintf(out, "\tTVertexAnim {\n");
		if (ktat.key.size() > 0)
			ktat.MdlWrite(out, "Translation", "\t\t");
		if (ktar.key.size() > 0)
			ktar.MdlWrite(out, "Rotation", "\t\t");
		if (ktas.key.size() > 0)
			ktas.MdlWrite(out, "Scaling", "\t\t");
		fprintf(out, "\t}\n");
	}


	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		line->init(in);
		while (line->type != LINE_EOF && line->type != LINE_CLOSER && !err->critical) {
			if (line->type == LINE_OPENER && line->chunk == "Translation")
				ktat.MdlRead(in, line, err);
			else if (line->type == LINE_OPENER && line->chunk == "Rotation")
				ktar.MdlRead(in, line, err);
			else if (line->type == LINE_OPENER && line->chunk == "Scaling")
				ktas.MdlRead(in, line, err);
			else
				err->add(line->lineText, line->lineCounter, "Unexpected in a TVertexAnim chunk");
			line->init(in);
		}
	}

	void MdxWrite(ofstream &out)
	{
		MdxWriteInt(out, ByteCount());
		ktat.MdxWrite(out, "KTAT");
		ktar.MdxWrite(out, "KTAR");
		ktas.MdxWrite(out, "KTAS");
	}

	int ByteCount() {
		return 4 +
			ktat.ByteCount() +
			ktar.ByteCount() +
			ktas.ByteCount();
	}
};

class HELP :public OBJ
{
public:
	void MdlWrite(FILE *out)
	{
		fprintf(out, "Helper \"%s\" {\n", name.c_str());
		MdlWriteProps(out);
		MdlWriteAnims(out);
		fprintf(out, "}\n");
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		name = line->label;

		do {
			line->init(in);
			if (MdlReadLine(in, line, err)) {

			}
			else if (line->type == LINE_EOF || line->type == LINE_CLOSER)
				break;
			else {
				err->add(line->lineText, line->lineCounter, "Unexpected in a Helper chunk");
				break;
			}
		} while (true);
	}
};

class LITE :public Chunk
{
public:
	OBJ obj;
	int type;  //0:Omnidirectional;1:Directional;2:Ambient
	KMTA klas; // attStart;
	KMTA klae; // attEnd;
	KGSC klac; // color
	KMTA klai; // intensity
	KGSC klbc; // ambCol
	KMTA klbi; // ambIntensity
	KMTA klav; // visibility

	LITE()
	{
		type = TYPE_LIGHT;
		klbc.reverseOrder = true;
		klbc.reverseOrder = true;
	}

	void MdxRead(ifstream &in)
	{
		int start = in.tellg();
		int end = start + MdxReadInt(in);
		obj.MdxRead(in);
		type = MdxReadInt(in);
		klas.staticState = MdxReadFloat(in);
		klae.staticState = MdxReadFloat(in);
		klac.staticZ = MdxReadFloat(in);
		klac.staticY = MdxReadFloat(in);
		klac.staticX = MdxReadFloat(in);
		klai.staticState = MdxReadFloat(in);
		klbc.staticZ = MdxReadFloat(in);
		klbc.staticY = MdxReadFloat(in);
		klbc.staticX = MdxReadFloat(in);
		klbi.staticState = MdxReadFloat(in);

		char tag[5];
		tag[4] = 0;
		while (in.tellg() < end)
		{
			in.read(tag, 4);
			if (strcmp(tag, "KLAS") == 0) klas.MdxRead(in);
			else if (strcmp(tag, "KLAE") == 0) klae.MdxRead(in);
			else if (strcmp(tag, "KLAC") == 0) klac.MdxRead(in);
			else if (strcmp(tag, "KLAI") == 0) klai.MdxRead(in);
			else if (strcmp(tag, "KLBC") == 0) klbc.MdxRead(in);
			else if (strcmp(tag, "KLBI") == 0) klbi.MdxRead(in);
			else if (strcmp(tag, "KLAV") == 0) klav.MdxRead(in);
			else { assert(false); }
		}
		assert((int)in.tellg() == end);
	}

	int ByteCount()
	{// Check this
		return obj.ByteCount() + 48 +
			klas.ByteCount() + klae.ByteCount() +
			klac.ByteCount() + klbc.ByteCount() +
			klai.ByteCount() + klbi.ByteCount() +
			klav.ByteCount();
	}

	void MdxWrite(ofstream &out)
	{
		MdxWriteInt(out, ByteCount());
		obj.MdxWrite(out);

		MdxWriteInt(out, type);
		MdxWriteFloat(out, klas.staticState);
		MdxWriteFloat(out, klae.staticState);
		MdxWriteFloat(out, klac.staticZ);
		MdxWriteFloat(out, klac.staticY);
		MdxWriteFloat(out, klac.staticX);
		MdxWriteFloat(out, klai.staticState);
		MdxWriteFloat(out, klbc.staticZ);
		MdxWriteFloat(out, klbc.staticY);
		MdxWriteFloat(out, klbc.staticX);
		MdxWriteFloat(out, klbi.staticState);

		klas.MdxWrite(out, "KLAS");
		klae.MdxWrite(out, "KLAE");
		klai.MdxWrite(out, "KLAI");
		klav.MdxWrite(out, "KLAV");
		klac.MdxWrite(out, "KLAC");
		klbc.MdxWrite(out, "KLBC");
		klbi.MdxWrite(out, "KLBI");
	}

	void MdlWrite(FILE *out)
	{
		fprintf(out, "Light \"%s\" {\n", obj.name.c_str());

		obj.MdlWriteProps(out);

		switch (type)
		{
		case(0): fprintf(out, "\tOmnidirectional,\n"); break;
		case(1): fprintf(out, "\tDirectional,\n"); break;
		case(2): fprintf(out, "\tAmbient,\n"); break;
		default: ;
		}
		klas.MdlWrite(out, "AttenuationStart", "\t");
		klae.MdlWrite(out, "AttenuationEnd", "\t");
		klai.MdlWrite(out, "Intensity", "\t");
		klac.MdlWrite(out, "Color", "\t");
		klbi.MdlWrite(out, "AmbIntensity", "\t");
		klbc.MdlWrite(out, "AmbColor", "\t");
		if (klav.key.size() > 0)
			klav.MdlWrite(out, "Visibility", "\t");

		obj.MdlWriteAnims(out);

		fprintf(out, "}\n");
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		obj.name = line->label;

		do {
			line->init(in);

			//It turns out that omnilights was not converted properly during .mdl to .mdx conversion.
			//When a corrupted model is used in a map, this leads to map's lightning to be totally broken.
			//It happened because TYPE_LIGHT was added to the light's type (Omnidirectional, Directional,
			//Ambient) value. (~profet)

			if (line->type == LINE_FLAG && strmatch(line->label, "omnidirectional")) {
				type = /*TYPE_LIGHT+*/LIGHT_OMNIDIRECTIONAL;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "directional")) {
				type = /*TYPE_LIGHT+*/LIGHT_DIRECTIONAL;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "ambient")) {
				type = /*TYPE_LIGHT+*/LIGHT_AMBIENT;

			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "attenuationstart")) {
				klas.staticState = line->f_data;
			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "attenuationstart")) {
				klas.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "attenuationend")) {
				klae.staticState = line->f_data;
			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "attenuationend")) {
				klae.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "intensity")) {
				klai.staticState = line->f_data;
			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "intensity")) {
				klai.MdlRead(in, line, err);
			}
			else if (line->type == LINE_BRACED_STATIC_DATA && strmatch(line->label, "color") && line->f_listData.size() == 3) {
				klac.staticX = line->f_listData[0];
				klac.staticY = line->f_listData[1];
				klac.staticZ = line->f_listData[2];
			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "color")) {
				klac.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "ambintensity")) {
				klbi.staticState = line->f_data;
			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "ambintensity")) {
				klbi.MdlRead(in, line, err);
			}
			else if (line->type == LINE_BRACED_STATIC_DATA && strmatch(line->label, "ambcolor") && line->f_listData.size() == 3) {
				klbc.staticX = line->f_listData[0];
				klbc.staticY = line->f_listData[1];
				klbc.staticZ = line->f_listData[2];
			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "ambcolor")) {
				klbc.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "visibility")) {
				klbi.staticState = line->f_data;
			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "visibility")) {
				klav.MdlRead(in, line, err);
			}
			else if (obj.MdlReadLine(in, line, err)) {

			}
			else if (line->type == LINE_EOF || line->type == LINE_CLOSER)
				break;
			else {
				err->add(line->lineText, line->lineCounter, "Unexpected in a Light chunk");
				break;
			}
		} while (true);
	}
};

class EVTS :public Chunk
{
public:
	OBJ obj;
	vector<int> frame;

	EVTS()
	{
		obj.type = TYPE_EVENTOBJECT;
	}

	void MdxRead(ifstream &in)
	{
		obj.MdxRead(in);
		MdxReadInt(in);
		int trackCount = MdxReadInt(in);
		MdxReadInt(in);
		for (int i = 0; i < trackCount; i++)
			frame.push_back(MdxReadInt(in));
	}

	void MdxWrite(ofstream &out)
	{
		obj.MdxWrite(out);
		out.write("KEVT", 4);
		MdxWriteInt(out, frame.size());
		MdxWriteInt(out, -1);
		for (int i = 0; i < frame.size(); i++)
			MdxWriteInt(out, frame[i]);
	}

	int ByteCount()
	{
		return obj.ByteCount() + 12 + frame.size() * 4;
	}

	void MdlWrite(FILE *out)
	{
		fprintf(out, "EventObject \"%s\" {\n", obj.name.c_str());
		obj.MdlWriteProps(out);

		if (frame.size() > 0)
		{
			fprintf(out, "\tEventTrack %d {\n", frame.size());
			for (int i = 0; i < frame.size(); i++)
				fprintf(out, "\t\t%d,\n", frame[i]);
			fprintf(out, "\t}\n");
		}

		obj.MdlWriteAnims(out);
		fprintf(out, "}\n");
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		obj.name = line->label;

		do {
			line->init(in);

			if (line->type == LINE_OPENER && strmatch(line->chunk, "eventtrack")) {
				while (true)
				{
					line->init(in);
					if (line->type == LINE_LONE_INT)
						frame.push_back(line->i_data);
					else if (line->type == LINE_CLOSER || line->type == LINE_EOF)
						break;
					else
						err->add(line, "Expected a frame.");
				}
			}
			else if (obj.MdlReadLine(in, line, err)) {

			}
			else if (line->type == LINE_EOF || line->type == LINE_CLOSER)
				break;
			else {
				err->add(line->lineText, line->lineCounter, "Unexpected in a Light chunk");
				break;
			}
		} while (true);
	}

};

class CLID :public Chunk
{
public:
	OBJ obj;
	int shape;
	float vertex[2][3]; // Sphere shapes only use the first of this
	float boundsRadius;


	CLID(): shape(0), vertex{}, boundsRadius(0)
	{
		obj.type = TYPE_COLLISIONSHAPE;
	}

	void MdxRead(ifstream &in)
	{
		obj.MdxRead(in);
		// int pos = in.tellg();
		shape = MdxReadInt(in);
		MdxReadFloat(in, 3, &vertex[0][0], &vertex[0][1], &vertex[0][2]);
		if (shape == SHAPE_SPHERE)
		{
			boundsRadius = MdxReadFloat(in);
		}
		else
		{
			MdxReadFloat(in, 3, &vertex[1][0], &vertex[1][1], &vertex[1][2]);
		}
	}

	void MdxWrite(ofstream &out)
	{
		obj.MdxWrite(out);
		MdxWriteInt(out, shape);
		MdxWriteFloat(out, vertex[0][0]);
		MdxWriteFloat(out, vertex[0][1]);
		MdxWriteFloat(out, vertex[0][2]);
		if (shape == SHAPE_SPHERE)
		{
			MdxWriteFloat(out, boundsRadius);
		}
		else if (shape == SHAPE_BOX)
		{
			MdxWriteFloat(out, vertex[1][0]);
			MdxWriteFloat(out, vertex[1][1]);
			MdxWriteFloat(out, vertex[1][2]);
		}
		else
		{
			assert(false);
		}
	}

	int ByteCount()
	{ // check this
		return obj.ByteCount() + 16 + (shape == SHAPE_SPHERE ? 4 : 12);
	}

	void MdlWrite(FILE *out)
	{
		fprintf(out, "CollisionShape \"%s\" {\n", obj.name.c_str());
		obj.MdlWriteProps(out);

		if (shape == SHAPE_SPHERE)
			fprintf(out, "\tSphere,\n");
		else
			fprintf(out, "\tBox,\n");

		fprintf(out, "\tVertices %d {\n", (shape == SHAPE_SPHERE) ? 1 : 2);
		fprintf(out, "\t\t{ %f, %f, %f },\n", vertex[0][0], vertex[0][1], vertex[0][2]);
		if (shape == SHAPE_BOX)
			fprintf(out, "\t\t{ %f, %f, %f },\n", vertex[1][0], vertex[1][1], vertex[1][2]);
		fprintf(out, "\t}\n");
		if (shape == SHAPE_SPHERE)
			fprintf(out, "\tBoundsRadius %f,\n", boundsRadius);

		obj.MdlWriteAnims(out);
		fprintf(out, "}\n");
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		shape = -1; // marking this as undefined so that an error can be put up
		obj.name = line->label;

		do {
			line->init(in);

			if (line->type == LINE_OPENER && strmatch(line->chunk, "vertices")) {
				int v = 0;
				while (true)
				{
					if (shape == -1)
						err->add(line, "Shape has not been defined");

					line->init(in);
					if (line->type == LINE_BRACED_DATA && line->f_listMatch && line->f_listData.size() == 3)
					{
						for (int i = 0; i < 3; i++)
							vertex[v][i] = line->f_listData[i];
						v++;
					}
					else if (line->type == LINE_CLOSER || line->type == LINE_EOF)
						break;
					else
						err->add(line, "Expected a vertex.");
				}
				if ((shape == SHAPE_SPHERE && v != 1) || (shape == SHAPE_BOX && v != 2))
				{
					err->add(line, "Wrong number of vertices.");
				}
			}
			else if (line->type == LINE_LABELED_DATA && line->f_match && strmatch(line->label, "boundsradius")) {
				boundsRadius = line->f_data;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "sphere")) {
				if (shape != -1)
					err->add(line, "Shape already defined.");
				shape = SHAPE_SPHERE;
			}
			else if (line->type == LINE_FLAG && strmatch(line->label, "box")) {
				if (shape != -1)
					err->add(line, "Shape already defined.");
				shape = SHAPE_BOX;
			}
			else if (obj.MdlReadLine(in, line, err)) {

			}
			else if (line->type == LINE_EOF || line->type == LINE_CLOSER)
				break;
			else {
				err->add(line->lineText, line->lineCounter, "Unexpected in a Light chunk");
				break;
			}
		} while (true);
	}
};

class RIBB :public Chunk
{
public:
	OBJ obj;


	KMTA heightAbove;
	KMTA heightBelow;
	KMTA alpha;
	KGSC color;
	KMTF textureSlot;
	KMTA visibility;
	int rows, cols;
	int emissionRate;
	float lifeSpan;
	float gravity;
	int materialID;
	int flags;

	RIBB(): rows(0), cols(0), emissionRate(0), lifeSpan(0), gravity(0), materialID(0)
	{
		obj.type = TYPE_RIBBON_EMITTER;
		flags = 0x00400000;
		color.reverseOrder = true;
	}


	void MdxRead(ifstream &in)
	{
		int start = in.tellg();
		int byteCount = MdxReadInt(in);
		int end = start + byteCount;
		int byteCountKG = MdxReadInt(in);
		int kgEnd = start + byteCountKG + 4;

		obj.name = MdxReadString(in, 80);
		obj.objectID = MdxReadInt(in);
		obj.parent = MdxReadInt(in);
		flags = MdxReadInt(in);


		char tag[5];
		tag[4] = 0;
		while (in.tellg() < kgEnd)
		{
			in.read(tag, 4);
			if (strcmp(tag, "KGTR") == 0)
				obj.kgtr.MdxRead(in);
			else if (strcmp(tag, "KGRT") == 0)
				obj.kgrt.MdxRead(in);
			else if (strcmp(tag, "KGSC") == 0)
				obj.kgsc.MdxRead(in);
			else {
				assert(false);
			}
		}
		assert((int)in.tellg() == kgEnd);


		MdxReadFloat(in, 7, &heightAbove.staticState, &heightBelow.staticState, &alpha.staticState, &color.staticZ, &color.staticY, &color.staticX, &lifeSpan);

		MdxReadInt(in);
		emissionRate = MdxReadInt(in);
		rows = MdxReadInt(in);
		cols = MdxReadInt(in);
		materialID = MdxReadInt(in);
		gravity = MdxReadFloat(in);

		while (in.tellg() < end)
		{
			in.read(tag, 4);
			if (strcmp(tag, "KRHA") == 0)
				heightAbove.MdxRead(in);
			else if (strcmp(tag, "KRHB") == 0)
				heightBelow.MdxRead(in);
			else if (strcmp(tag, "KRAL") == 0)
				alpha.MdxRead(in);
			else if (strcmp(tag, "KRCO") == 0)
				color.MdxRead(in);
			else if (strcmp(tag, "KRVS") == 0)
				visibility.MdxRead(in);
			else if (strcmp(tag, "KRTX") == 0)
				textureSlot.MdxRead(in);
			else {
				assert(false);
			}
		}
		assert((int)in.tellg() == end);
	}

	void MdxWrite(ofstream &out)
	{
		MdxWriteInt(out, ByteCount());
		MdxWriteInt(out, ByteCountKG());

		MdxWriteString(out, obj.name, 80);
		MdxWriteInt(out, obj.objectID);
		MdxWriteInt(out, obj.parent);
		MdxWriteInt(out, flags);
		obj.kgtr.MdxWrite(out, "KGTR");
		obj.kgrt.MdxWrite(out, "KGRT");
		obj.kgsc.MdxWrite(out, "KGSC");
		MdxWriteFloat(out, heightAbove.staticState);
		MdxWriteFloat(out, heightBelow.staticState);
		MdxWriteFloat(out, alpha.staticState);
		MdxWriteFloat(out, color.staticZ);
		MdxWriteFloat(out, color.staticY);
		MdxWriteFloat(out, color.staticX);
		MdxWriteFloat(out, lifeSpan);
		MdxWriteInt(out, 0);
		MdxWriteInt(out, emissionRate);
		MdxWriteInt(out, rows);
		MdxWriteInt(out, cols);
		MdxWriteInt(out, materialID);
		MdxWriteFloat(out, gravity);


		heightAbove.MdxWrite(out, "KRHA");
		heightBelow.MdxWrite(out, "KRHB");
		alpha.MdxWrite(out, "KRAL");
		color.MdxWrite(out, "KRCO");
		visibility.MdxWrite(out, "KRVS");
		textureSlot.MdxWrite(out, "KRTX");
	}


	/*long	nbytesi;
	long	nbytesikg;		// inclusive bytecount including KGXXs
	ASCII	Name;			(0x50 bytes)
	long	ObjectID;
	long	Parent; 		(0xFFFFFFFF if none)
	long	Flags;			(0x00400000)
	(KGTR)
	(KGRT)
	(KGSC)
	float	HeightAbove;
	float	HeightBelow;
	float	Alpha;
	float	ColorR, ColorG, ColorB;
	float	LifeSpan;
	long	???;			(0)
	long	EmissionRate;
	long	Rows;
	long	Columns;
	long	MaterialID;
	float	Gravity;
	(KRVS)
	(KRHA)
	(KRHB)

*/

	int ByteCount()
	{ // TODO
		return
			152 +
			obj.kgtr.ByteCount() +
			obj.kgsc.ByteCount() +
			obj.kgrt.ByteCount() +
			heightAbove.ByteCount() +
			heightBelow.ByteCount() +
			alpha.ByteCount() +
			color.ByteCount() +
			textureSlot.ByteCount() +
			visibility.ByteCount()
			;
	}
	int ByteCountKG()
	{ // TODO
		return
			96 +
			obj.kgtr.ByteCount() +
			obj.kgsc.ByteCount() +
			obj.kgrt.ByteCount()

			;
	}

	void MdlWrite(FILE *out)
	{
		fprintf(out, "RibbonEmitter \"%s\" {\n", obj.name.c_str());
		obj.MdlWriteProps(out);

		heightAbove.MdlWrite(out, "HeightAbove", "\t");
		heightBelow.MdlWrite(out, "HeightBelow", "\t");
		alpha.MdlWrite(out, "Alpha", "\t");
		color.MdlWrite(out, "Color", "\t");
		textureSlot.MdlWrite(out, "TextureSlot", "\t");
		if (!visibility.key.empty() || visibility.staticState != 0.0f)
			visibility.MdlWrite(out, "Visibility", "\t");

		fprintf(out, "\tEmissionRate %d,\n", emissionRate);
		fprintf(out, "\tLifeSpan %f,\n", lifeSpan);
		if (gravity != 0.0f)
			fprintf(out, "\tGravity %f,\n", gravity);
		fprintf(out, "\tRows %d,\n", rows);
		fprintf(out, "\tColumns %d,\n", cols);
		fprintf(out, "\tMaterialID %d,\n", materialID);

		obj.MdlWriteAnims(out);
		fprintf(out, "}\n");
	}

	void MdlRead(ifstream &in, MdlLine* line, MdlError * err)
	{
		obj.name = line->label;
		gravity = 0.0f;

		do {
			line->init(in);

			if (line->type == LINE_OPENER && strmatch(line->chunk, "heightabove")) {
				heightAbove.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "heightabove")) {
				heightAbove.staticState = line->f_data;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "heightbelow")) {
				heightBelow.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "heightbelow")) {
				heightBelow.staticState = line->f_data;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "alpha")) {
				alpha.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "alpha")) {
				alpha.staticState = line->f_data;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "color")) {
				color.MdlRead(in, line, err);
			}
			else if (line->type == LINE_BRACED_STATIC_DATA && strmatch(line->label, "color") && line->f_listData.size() == 3) {
				color.staticX = line->f_listData[0];
				color.staticY = line->f_listData[1];
				color.staticZ = line->f_listData[2];

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "textureslot")) {
				textureSlot.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "textureslot")) {
				textureSlot.staticState = line->f_data;

			}
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "visibility")) {
				visibility.MdlRead(in, line, err);
			}
			else if (line->type == LINE_STATIC_LABELED_DATA && strmatch(line->label, "visibility")) {
				visibility.staticState = line->f_data;

			}
			else if (line->type == LINE_LABELED_DATA && strmatch(line->label, "emissionrate") && line->i_match) {
				emissionRate = line->i_data;
			}
			else if (line->type == LINE_LABELED_DATA && strmatch(line->label, "rows") && line->i_match) {
				rows = line->i_data;
			}
			else if (line->type == LINE_LABELED_DATA && strmatch(line->label, "columns") && line->i_match) {
				cols = line->i_data;
			}
			else if (line->type == LINE_LABELED_DATA && strmatch(line->label, "materialid") && line->i_match) {
				materialID = line->i_data;
			}
			else if (line->type == LINE_LABELED_DATA && strmatch(line->label, "gravity") && line->f_match) {
				gravity = line->f_data;
			}
			else if (line->type == LINE_LABELED_DATA && strmatch(line->label, "lifespan") && line->f_match) {
				lifeSpan = line->f_data;


			}
			else if (obj.MdlReadLine(in, line, err)) {

			}
			else if (line->type == LINE_EOF || line->type == LINE_CLOSER)
				break;
			else {
				err->add(line->lineText, line->lineCounter, "Unexpected in a Light chunk");
				break;
			}
		} while (true);
	}

};

class GLBS :public Chunk {
public:
	int frames;
	void MdlRead(ifstream &in, MdlLine* line, MdlError * err) { frames = line->i_data; }
	void MdlWrite(FILE *out) { fprintf(out, "\tDuration %d,\n", frames); }
	void MdxRead(ifstream &in) { frames = MdxReadInt(in); }
	void MdxWrite(ofstream &out) { MdxWriteInt(out, frames); }
};

class MDLX :public Chunk { // Mdl or mdx
public:
	VERS vers;
	MODL modl;
	vector<TEX> tex;
	vector<MTL> mtl;
	vector<GEOS> geos;
	vector<BONE> bone;
	vector<PIVT> pivt;
	vector<SEQ> seq;
	vector<TXAN> txan;
	vector<GEOA> geoa;
	vector<LITE> lite;
	vector<HELP> help;
	vector<EVTS> evts;
	vector<CLID> clid;
	vector<ATCH> atch;
	vector<PREM> prem;
	vector<PRE2> pre2;
	vector<RIBB> ribb;
	vector<CAMS> cams;
	vector<GLBS> glbs;



	/**************************\
   |       MDL Writing          |
	\**************************/
	void MdlWriteComment(FILE *out) {
		/*
		time_t rawtime;
		struct tm * timeinfo;

		time(&rawtime);
		timeinfo = localtime(&rawtime);
		fprintf(out, "// Converted %s", asctime(timeinfo));
		fprintf(out, "// MdlxConv Version %s.\n", __DATE__);
		*/
	}
	void MdlWriteVers(FILE *out)
	{
		vers.MdlWrite(out);
	}

	void MdlWriteModl(FILE *out)
	{
		modl.MdlWrite(out);
	}

	void MdlWriteSeqs(FILE *out)
	{
		if (seq.size() > 0)
		{
			fprintf(out, "Sequences %d {\n", seq.size());
			for (int i = 0; i < seq.size(); i++)
				seq[i].MdlWrite(out);
			fprintf(out, "}\n");
		}
	}
	void MdlWriteGlbs(FILE *out)
	{
		if (glbs.size() > 0)
		{
			fprintf(out, "GlobalSequences %d {\n", glbs.size());
			for (int i = 0; i < glbs.size(); i++)
				glbs[i].MdlWrite(out);
			fprintf(out, "}\n");
		}
	}
	void MdlWriteMtls(FILE *out)
	{
		if (mtl.size() > 0)
		{
			fprintf(out, "Materials %d {\n", mtl.size());
			for (int i = 0; i < mtl.size(); i++)
				mtl[i].MdlWrite(out);
			fprintf(out, "}\n");
		}
	}

	void MdlWriteTexs(FILE *out)
	{
		if (tex.size() > 0)
		{
			fprintf(out, "Textures %d {\n", tex.size());
			for (int i = 0; i < tex.size(); i++)
				tex[i].MdlWrite(out);
			fprintf(out, "}\n");
		}
	}
	void MdlWriteTxan(FILE *out)
	{
		if (txan.size() > 0)
		{
			fprintf(out, "TextureAnims %d {\n", txan.size());
			for (int i = 0; i < txan.size(); i++)
				txan[i].MdlWrite(out);
			fprintf(out, "}\n");
		}
	}

	void MdlWriteGeoa(FILE *out)
	{
		for (int i = 0; i < geoa.size(); i++)
			geoa[i].MdlWrite(out);
	}
	void MdlWriteGeos(FILE *out)
	{
		for (int i = 0; i < geos.size(); i++)
			geos[i].MdlWrite(out);
	}

	void MdlWriteBones(FILE *out)
	{
		for (int i = 0; i < bone.size(); i++)
			bone[i].MdlWrite(out);
	}

	void MdlWriteLites(FILE *out)
	{
		for (int i = 0; i < lite.size(); i++)
			lite[i].MdlWrite(out);
	}
	void MdlWritePivts(FILE *out)
	{
		fprintf(out, "PivotPoints %d {\n", pivt.size());
		for (int i = 0; i < pivt.size(); i++)
			pivt[i].MdlWrite(out, "\t");
		fprintf(out, "}\n");
	}
	void MdlWriteEvts(FILE *out)
	{
		for (int i = 0; i < evts.size(); i++)
			evts[i].MdlWrite(out);
	}
	void MdlWriteClid(FILE *out)
	{
		for (int i = 0; i < clid.size(); i++)
			clid[i].MdlWrite(out);
	}
	void MdlWriteHelps(FILE *out)
	{
		for (int i = 0; i < help.size(); i++)
			help[i].MdlWrite(out);
	}
	void MdlWriteCams(FILE *out)
	{
		for (int i = 0; i < cams.size(); i++)
			cams[i].MdlWrite(out);
	}

	void MdlWrite(FILE* out)
	{
		int i;
		MdlWriteComment(out);
		MdlWriteVers(out);
		MdlWriteModl(out);
		MdlWriteSeqs(out);
		MdlWriteGlbs(out);
		MdlWriteTexs(out);
		MdlWriteMtls(out);
		MdlWriteTxan(out);
		MdlWriteGeos(out);
		MdlWriteGeoa(out);
		MdlWriteBones(out);
		MdlWriteLites(out);
		MdlWriteHelps(out);
		for (i = 0; i < atch.size(); i++)
			atch[i].MdlWrite(out, i);
		MdlWritePivts(out);
		for (i = 0; i < prem.size(); i++)
			prem[i].MdlWrite(out);
		for (i = 0; i < pre2.size(); i++)
			pre2[i].MdlWrite(out);
		for (i = 0; i < ribb.size(); i++) ribb[i].MdlWrite(out);
		MdlWriteCams(out);
		MdlWriteEvts(out);
		MdlWriteClid(out);

	}

	/**************************\
   |       MDX Writing          |
	\**************************/
	void MdxWriteVers(ofstream &out)
	{
		vers.MdxWrite(out);
	}

	void MdxWriteModl(ofstream &out)
	{
		modl.MdxWrite(out);
	}

	void MdxWriteSeqs(ofstream &out)
	{
		out.write("SEQS", 4);
		MdxWriteInt(out, seq.size() * 132);
		for (int i = 0; i < seq.size(); i++) {
			seq[i].MdxWrite(out);
		}
	}

	void MdxWriteGlbs(ofstream &out)
	{
		if (glbs.size() == 0) return;
		out.write("GLBS", 4);
		MdxWriteInt(out, glbs.size() * 4);
		for (int i = 0; i < glbs.size(); i++) {
			glbs[i].MdxWrite(out);
		}
	}
	void MdxWriteMtls(ofstream &out)
	{
		int i;
		if (mtl.size() == 0) return;

		int byteCount = 0;
		for (i = 0; i < mtl.size(); i++) byteCount += mtl[i].ByteCount();
		out.write("MTLS", 4);
		MdxWriteInt(out, byteCount);
		for (i = 0; i < mtl.size(); i++)
		{
			mtl[i].MdxWrite(out);
		}
	}

	void MdxWriteTexs(ofstream &out)
	{
		out.write("TEXS", 4);
		MdxWriteInt(out, tex.size() * 268);
		for (int i = 0; i < tex.size(); i++)
		{
			tex[i].MdxWrite(out);
		}
	}

	void MdxWriteGeos(ofstream &out)
	{
		int i;
		if (geos.size() == 0) return;

		int byteCount = 0;
		for (i = 0; i < geos.size(); i++) byteCount += geos[i].ByteCount() + 4;
		out.write("GEOS", 4);
		MdxWriteInt(out, byteCount);
		for (i = 0; i < geos.size(); i++)
		{
			geos[i].MdxWrite(out);
		}
	}

	void MdxWriteGeoa(ofstream &out)
	{
		int i;
		if (geoa.size() == 0) return;

		int byteCount = 0;
		for (i = 0; i < geoa.size(); i++) byteCount += geoa[i].ByteCount();
		out.write("GEOA", 4);
		MdxWriteInt(out, byteCount);
		for (i = 0; i < geoa.size(); i++)
		{
			geoa[i].MdxWrite(out);
		}
	}

	void MdxWriteBone(ofstream &out)
	{
		int i;
		if (bone.size() == 0) return;

		int byteCount = 0;
		for (i = 0; i < bone.size(); i++) byteCount += bone[i].ByteCount() + 8;
		out.write("BONE", 4);
		MdxWriteInt(out, byteCount);
		for (i = 0; i < bone.size(); i++)
		{
			bone[i].MdxWrite(out);
		}
	}

	void MdxWriteLite(ofstream &out)
	{
		int i;
		if (lite.size() == 0) return;

		int byteCount = 0;
		for (i = 0; i < lite.size(); i++) byteCount += lite[i].ByteCount();
		out.write("LITE", 4);
		MdxWriteInt(out, byteCount);
		for (i = 0; i < lite.size(); i++)
		{
			lite[i].MdxWrite(out);
		}
	}


	void MdxWriteHelp(ofstream &out)
	{
		int i;
		if (help.size() == 0) return;

		int byteCount = 0;
		for (i = 0; i < help.size(); i++) byteCount += help[i].ByteCount();
		out.write("HELP", 4);
		MdxWriteInt(out, byteCount);
		for (i = 0; i < help.size(); i++)
		{
			help[i].MdxWrite(out);
		}
	}

	void MdxWritePivt(ofstream &out)
	{
		out.write("PIVT", 4);
		MdxWriteInt(out, pivt.size() * 12);
		for (int i = 0; i < pivt.size(); i++) {
			pivt[i].MdxWrite(out);
			//MdxWriteFloats(out, 3, pivt[i].x, pivt[i].y, pivt[i].z );
		}
	}

	void MdxWritePrem(ofstream &out)
	{
		int i;
		if (prem.size() == 0) return;

		int byteCount = 0;
		for (i = 0; i < prem.size(); i++) byteCount += prem[i].ByteCount();
		out.write("PREM", 4);
		MdxWriteInt(out, byteCount);
		for (i = 0; i < prem.size(); i++)
		{
			prem[i].MdxWrite(out);
		}
	}
	void MdxWritePre2(ofstream &out)
	{
		int i;
		if (pre2.size() == 0) return;

		int byteCount = 0;
		for (i = 0; i < pre2.size(); i++) byteCount += pre2[i].ByteCount();
		out.write("PRE2", 4);
		MdxWriteInt(out, byteCount);
		for (i = 0; i < pre2.size(); i++)
		{
			pre2[i].MdxWrite(out);
		}
	}

	void MdxWriteRibb(ofstream &out)
	{
		int i;
		if (ribb.size() == 0) return;

		int byteCount = 0;
		for (i = 0; i < ribb.size(); i++) byteCount += ribb[i].ByteCount();
		out.write("RIBB", 4);
		MdxWriteInt(out, byteCount);
		for (i = 0; i < ribb.size(); i++)
		{
			ribb[i].MdxWrite(out);
		}
	}
	void MdxWriteAtch(ofstream &out)
	{
		int i;
		if (atch.size() == 0) return;

		int byteCount = 0;
		for (i = 0; i < atch.size(); i++) byteCount += atch[i].ByteCount();
		out.write("ATCH", 4);
		MdxWriteInt(out, byteCount);
		for (i = 0; i < atch.size(); i++)
		{
			atch[i].MdxWrite(out);
		}
	}


	void MdxWriteTxan(ofstream &out)
	{
		int i;
		if (txan.size() == 0) return;

		int byteCount = 0;
		for (i = 0; i < txan.size(); i++) byteCount += txan[i].ByteCount();
		out.write("TXAN", 4);
		MdxWriteInt(out, byteCount);
		for (i = 0; i < txan.size(); i++)
		{
			txan[i].MdxWrite(out);
		}
	}
	void MdxWriteEvts(ofstream &out)
	{
		int i;
		if (evts.size() == 0) return;

		int byteCount = 0;
		for (i = 0; i < evts.size(); i++) byteCount += evts[i].ByteCount();
		out.write("EVTS", 4);
		MdxWriteInt(out, byteCount);
		for (i = 0; i < evts.size(); i++)
		{
			evts[i].MdxWrite(out);
		}
	}
	void MdxWriteClid(ofstream &out)
	{
		int i;
		if (clid.size() == 0) return;

		int byteCount = 0;
		for (i = 0; i < clid.size(); i++) byteCount += clid[i].ByteCount();
		out.write("CLID", 4);
		MdxWriteInt(out, byteCount);
		for (i = 0; i < clid.size(); i++)
		{
			clid[i].MdxWrite(out);
		}
	}

	void MdxWriteCams(ofstream &out)
	{
		int i;
		if (cams.size() == 0) return;

		int byteCount = 0;
		for (i = 0; i < cams.size(); i++) byteCount += cams[i].ByteCount();
		out.write("CAMS", 4);
		MdxWriteInt(out, byteCount);
		for (i = 0; i < cams.size(); i++)
		{
			cams[i].MdxWrite(out);
		}
	}
	void MdxWrite(ofstream &out)
	{
		out.write("MDLX", 4);
		MdxWriteVers(out);
		MdxWriteModl(out);
		MdxWriteSeqs(out);
		MdxWriteGlbs(out);
		MdxWriteMtls(out);
		MdxWriteTexs(out);
		MdxWriteTxan(out);
		MdxWriteGeos(out);
		MdxWriteGeoa(out);
		MdxWriteBone(out);
		MdxWriteLite(out);
		MdxWriteHelp(out);
		MdxWriteAtch(out);
		MdxWritePivt(out);
		MdxWritePrem(out);
		MdxWritePre2(out);
		MdxWriteRibb(out);
		MdxWriteCams(out);
		MdxWriteEvts(out);
		MdxWriteClid(out);
	}

	/**************************\
   |       MDX Reading          |
	\**************************/
	void MdxRead(ifstream &in)
	{
		clear();

		char tag[5];
		tag[4] = 0;
		in.read(tag, 4);
		cout << "Tag: " << tag << endl;
		if (strcmp(tag, "MDLX") != 0) return;
		// int i = 0;
		while (in.eof() == false)
		{
			in.read(tag, 4);
			if (in.eof())
			{
				break;
			}
			cout << "Tag: " << tag << endl;
			if (strcmp(tag, "VERS") == 0) {
				vers.MdxRead(in);
			}
			else if (strcmp(tag, "MODL") == 0) modl.MdxRead(in);
			else if (strcmp(tag, "SEQS") == 0) {
				int seqCount;
				MdxReadInt(in, &seqCount);
				seqCount /= 132;
				for (int i = 0; i < seqCount; i++) {
					SEQ newSeq;
					newSeq.MdxRead(in);
					seq.push_back(newSeq);
				}
			}
			else if (strcmp(tag, "GLBS") == 0) {
				int count = MdxReadInt(in);;
				int end = (int)in.tellg() + count;

				while (in.tellg() < end) {
					GLBS newGlbs;
					newGlbs.MdxRead(in);
					glbs.push_back(newGlbs);
				}
				assert((int)in.tellg() == end);
			}
			else if (strcmp(tag, "MTLS") == 0) {
				int count = MdxReadInt(in);;
				int end = (int)in.tellg() + count;

				while (in.tellg() < end) {
					MTL newMtl;
					newMtl.MdxRead(in);
					mtl.push_back(newMtl);
				}
				assert((int)in.tellg() == end);
			}
			else if (strcmp(tag, "TEXS") == 0) {
				int count;
				count = MdxReadInt(in) / 268;
				for (int i = 0; i < count; i++) {
					TEX newTex;
					newTex.MdxRead(in);
					tex.push_back(newTex);
				}
			}
			else if (strcmp(tag, "GEOA") == 0) {
				int count = MdxReadInt(in);
				int end = (int)in.tellg() + count;
				while (in.tellg() < end) {
					GEOA newGeoa;
					newGeoa.MdxRead(in);
					geoa.push_back(newGeoa);
				}
				assert((int)in.tellg() == end);
			}
			else if (strcmp(tag, "GEOS") == 0) {
				int count = MdxReadInt(in);
				int end = (int)in.tellg() + count;
				while (in.tellg() < end) {
					GEOS newGeos;
					newGeos.MdxRead(in);
					geos.push_back(newGeos);
				}
				assert((int)in.tellg() == end);
			}
			else if (strcmp(tag, "BONE") == 0) {
				MDXREAD(BONE, bone)
			}
			else if (strcmp(tag, "HELP") == 0) {
				int count = MdxReadInt(in);
				int end = (int)in.tellg() + count;
				while (in.tellg() < end) {
					HELP newHelp;
					newHelp.MdxRead(in);
					help.push_back(newHelp);
				}
				assert((int)in.tellg() == end);
			}
			else if (strcmp(tag, "ATCH") == 0) {
				int count = MdxReadInt(in);
				int end = (int)in.tellg() + count;
				while (in.tellg() < end) {
					ATCH newAtch;
					newAtch.MdxRead(in);
					atch.push_back(newAtch);
				}
				assert((int)in.tellg() == end);
			}
			else if (strcmp(tag, "PIVT") == 0) {
				int pivtCount = MdxReadInt(in) / 12;
				for (int i = 0; i < pivtCount; i++) {
					PIVT newPivt;
					newPivt.MdxRead(in);
					pivt.push_back(newPivt);
				}
			}
			else if (strcmp(tag, "TXAN") == 0) {
				int count = MdxReadInt(in);
				int end = (int)in.tellg() + count;
				while (in.tellg() < end) {
					TXAN newTxan;
					newTxan.MdxRead(in);
					txan.push_back(newTxan);
				}
				assert((int)in.tellg() == end);
			}
			else if (strcmp(tag, "CLID") == 0) {
				int count = MdxReadInt(in);
				int end = (int)in.tellg() + count;
				while (in.tellg() < end) {
					CLID newClid;
					newClid.MdxRead(in);
					clid.push_back(newClid);
				}
				assert((int)in.tellg() == end);
			}
			else if (strcmp(tag, "PRE2") == 0) {
				int count = MdxReadInt(in);
				int end = (int)in.tellg() + count;
				while (in.tellg() < end) {
					PRE2 newPre2;
					newPre2.MdxRead(in);
					pre2.push_back(newPre2);
				}
				assert((int)in.tellg() == end);
			}
			else if (strcmp(tag, "RIBB") == 0) {
				int count = MdxReadInt(in);
				int end = (int)in.tellg() + count;
				while (in.tellg() < end) {
					RIBB newRibb;
					newRibb.MdxRead(in);
					ribb.push_back(newRibb);
				}
				assert((int)in.tellg() == end);
			}
			else if (strcmp(tag, "CAMS") == 0) {
				int count = MdxReadInt(in);
				int end = (int)in.tellg() + count;
				while (in.tellg() < end) {
					CAMS newCams;
					newCams.MdxRead(in);
					cams.push_back(newCams);
				}
				assert((int)in.tellg() == end);
			}
			else if (strcmp(tag, "LITE") == 0) {
				MDXREAD(LITE, lite)
			}
			else if (strcmp(tag, "EVTS") == 0) {
				MDXREAD(EVTS, evts)
			}
			else if (strcmp(tag, "PREM") == 0) {
				MDXREAD(PREM, prem)
			}
			else {
				assert(false);
				int len;
				MdxReadInt(in, &len);
				in.seekg(len, ios::cur);
			}

		}
	}

	MdlError err_v;
	/**************************\
   |       MDL Reading          |
	\**************************/
	void MdlRead(ifstream &in)
	{
		clear();

		MdlLine line_v("");
		MdlLine* line = &line_v;

		MdlError* err = &err_v;

		line->lineCounter = 0;
		do {
			line->init(in);

			// Version
			if (line->type == LINE_OPENER && line->chunk == "Version")
				vers.MdlRead(in, line, err);
			// Model
			else if (line->type == LINE_OPENER_LABELED && line->chunk == "Model")
				modl.MdlRead(in, line, err);
			// TextureAnims
			else if (line->type == LINE_OPENER && line->chunk == "TextureAnims") {
				line->init(in);
				while (line->type != LINE_CLOSER && line->type != LINE_EOF && !err->critical)
				{
					if (line->type == LINE_OPENER && line->chunk == "TVertexAnim") {
						TXAN newTxan;
						newTxan.MdlRead(in, line, err);
						txan.push_back(newTxan);
					}
					else {
						err->add(line->lineText, line->lineCounter, "Expected TVertexAnim {");
					}
					line->init(in);
				}
			}
			// Sequences
			else if (line->type == LINE_OPENER && line->chunk.compare("Sequences") == 0) {
				line->init(in);
				while (line->type != LINE_CLOSER && line->type != LINE_EOF && !err->critical)
				{
					if (line->type == LINE_OPENER_LABELED && strmatch(line->chunk, "Anim")) {
						SEQ newSeq; newSeq.MdlRead(in, line, err);
						seq.push_back(newSeq);
					}
					else {
						err->add(line->lineText, line->lineCounter, "Expected Anim \"name\" {");
					}
					line->init(in);
				}
			}
			// GlobalSequences
			else if (line->type == LINE_OPENER && line->chunk.compare("GlobalSequences") == 0) {
				line->init(in);
				while (line->type != LINE_CLOSER && line->type != LINE_EOF && !err->critical)
				{
					if (line->type == LINE_LABELED_DATA && line->i_match && strmatch(line->label, "duration")) {
						GLBS newGlbs; newGlbs.MdlRead(in, line, err);
						glbs.push_back(newGlbs);
					}
					else {
						err->add(line->lineText, line->lineCounter, "Expected ###,");
					}
					line->init(in);
				}
			}
			// Materials
			else if (line->type == LINE_OPENER && line->chunk.compare("Materials") == 0) {
				line->init(in);
				while (line->type != LINE_CLOSER && line->type != LINE_EOF && !err->critical)
				{
					if (line->type == LINE_OPENER && strmatch(line->chunk, "Material")) {
						MTL newMtl; newMtl.MdlRead(in, line, err);
						mtl.push_back(newMtl);
					}
					else {
						err->add(line->lineText, line->lineCounter, "Expected Material {");
					}
					line->init(in);
				}
			}
			// Textures
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "textures")) {
				line->init(in);
				while (line->type != LINE_CLOSER && line->type != LINE_EOF && !err->critical)
				{
					if (line->type == LINE_OPENER && strmatch(line->chunk, "bitmap")) {
						TEX newTex; newTex.MdlRead(in, line, err);
						tex.push_back(newTex);
					}
					else {
						err->add(line->lineText, line->lineCounter, "Expected Bitmap {");
					}
					line->init(in);
				}
			}
			// Geosets
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "geoset")) {
				GEOS newGeos;
				newGeos.MdlRead(in, line, err);
				geos.push_back(newGeos);
			}
			// Geoset anims
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "geosetanim")) {
				GEOA newGeoa;
				newGeoa.MdlRead(in, line, err);
				geoa.push_back(newGeoa);
			}
			// Bones
			else if (line->type == LINE_OPENER_LABELED && strmatch(line->chunk, "bone")) {
				BONE newBone;
				newBone.MdlRead(in, line, err);
				bone.push_back(newBone);
			}
			// Helpers
			else if (line->type == LINE_OPENER_LABELED && strmatch(line->chunk, "helper")) {
				HELP newHelp;
				newHelp.MdlRead(in, line, err);
				help.push_back(newHelp);
			}
			// Attachments
			else if (line->type == LINE_OPENER_LABELED && strmatch(line->chunk, "attachment")) {
				ATCH newAtch;
				newAtch.MdlRead(in, line, err, atch.size());
				atch.push_back(newAtch);
			}
			// Lights
			else if (line->type == LINE_OPENER_LABELED && strmatch(line->chunk, "light")) {
				LITE newLite;
				newLite.MdlRead(in, line, err);
				lite.push_back(newLite);
			}
			// PivotPoints
			else if (line->type == LINE_OPENER && strmatch(line->chunk, "pivotpoints")) {
				line->init(in);
				while (line->type != LINE_CLOSER && line->type != LINE_EOF && !err->critical)
				{
					if (line->type == LINE_BRACED_DATA && line->f_listMatch && line->f_listData.size() == 3) {
						PIVT newPivt; newPivt.MdlRead(in, line, err);
						pivt.push_back(newPivt);
					}
					else {
						err->add(line->lineText, line->lineCounter, "Expected a pivotpoint");
					}
					line->init(in);
				}
			}
			// EventObjects
			else if (line->type == LINE_OPENER_LABELED && strmatch(line->chunk, "eventobject")) {
				EVTS newEvts;
				newEvts.MdlRead(in, line, err);
				evts.push_back(newEvts);
			}
			// CollisionShape
			else if (line->type == LINE_OPENER_LABELED && strmatch(line->chunk, "collisionshape")) {
				CLID newClid;
				newClid.MdlRead(in, line, err);
				clid.push_back(newClid);
			}
			// ParticleEmitter2s
			else if (line->type == LINE_OPENER_LABELED && strmatch(line->chunk, "particleemitter2")) {
				PRE2 newPre2;
				newPre2.MdlRead(in, line, err);
				pre2.push_back(newPre2);
			}
			// ParticleEmitters
			else if (line->type == LINE_OPENER_LABELED && strmatch(line->chunk, "particleemitter")) {
				PREM newPrem;
				newPrem.MdlRead(in, line, err);
				prem.push_back(newPrem);
			}
			// RibbonEmitters
			else if (line->type == LINE_OPENER_LABELED && strmatch(line->chunk, "ribbonemitter")) {
				//RIBB newRibb;
				//newRibb.MdlRead(in, line, err);
				//ribb.push_back(newRibb);
				MDLREAD(RIBB, ribb);
			}
			// Cameras
			else if (line->type == LINE_OPENER_LABELED && strmatch(line->chunk, "camera")) {
				//RIBB newRibb;
				//newRibb.MdlRead(in, line, err);
				//ribb.push_back(newRibb);
				MDLREAD(CAMS, cams);
			}
			else if (line->type == LINE_EOF) {

			}
			else
			{
				err->add(line->lineText, line->lineCounter, "Unknown chunk");
			}
		} while (line->type != LINE_EOF && !err->critical);

		// Error report
		printf("%d errors\n", err->errorLine.size());
		for (int i = 0; i < err->errorLine.size(); i++)
		{
			cout << "  Line " << err->errorLineNum[i] << ": " << err->errorLine[i].c_str() << endl;
			cout << "    " << err->errorRemark[i] << endl;
		}
	}

	void clear()
	{
		modl = MODL();
		vers = VERS();
		tex.clear();
		mtl.clear();
		geos.clear();
		bone.clear();
		pivt.clear();
		seq.clear();
		txan.clear();
		help.clear();
		lite.clear();
		evts.clear();
		pre2.clear();
		cams.clear();
		lite.clear();
		prem.clear();
	}

#ifdef TRANSFORM

	OBJ **transformOrder; // holds pointers to all the objects
	int objCount;
	bool transformOrderBuilt;

	void TransformNone()
	{
		for (int i = 0; i < geos.size(); i++)
		{
			for (int j = 0; j < geos[i].vert.size(); j++)
			{
				geos[i].vert[j].tx = geos[i].vert[j].x;
				geos[i].vert[j].ty = geos[i].vert[j].y;
				geos[i].vert[j].tz = geos[i].vert[j].z;
			}
		}
	}

	void Transform(int frame
#ifdef DEBUG
		, ofstream &out
#endif
	)
	{

		int i, j;
		float parentMatrix[16];
		float parentRotMatrix[16];
		for (i = 0; i < objCount; i++)
		{

			if (transformOrder[i]->parent != -1)
			{
				OBJ* par;
				for (j = 0; j < objCount; j++)
				{
					if (transformOrder[j]->objectID == transformOrder[i]->parent)
					{
						par = transformOrder[j];
					}
				}


				for (j = 0; j < 16; j++) {
					parentMatrix[j] = par->mat[j];
					parentRotMatrix[j] = par->rotMat[j];
				}
			}
			else
			{
				for (j = 0; j < 16; j++)
					parentMatrix[j] = 0;
				parentMatrix[0] = parentMatrix[5] = parentMatrix[10] = parentMatrix[15] = 1;
				for (j = 0; j < 16; j++) parentRotMatrix[j] = parentMatrix[j];
			}

			PIVT p = pivt[transformOrder[i]->objectID];
#ifdef DEBUG
			out << "Using pivot point for object id " << transformOrder[i]->objectID << " ( " << p.x << "," << p.y << "," << p.z << ")" << endl;
#endif

			transformOrder[i]->Transform(frame, parentMatrix, parentRotMatrix, pivt[transformOrder[i]->objectID]
#ifdef DEBUG
				, out
#endif
			);
		}
		for (i = 0; i < geos.size(); i++)
		{
			geos[i].Transform(frame, bone
#ifdef DEBUG
				, out
#endif
			);
		}
	}


	void BuildTransformOrder(
#ifdef DEBUG
		ofstream &out
#endif
	)
	{
		if (transformOrderBuilt)
		{
			delete[] transformOrder;
		}

		int i, j;
		OBJ ** allObjects;
		objCount = bone.size() + help.size();
		transformOrder = new OBJ*[objCount];
		allObjects = new OBJ*[objCount];
		bool *isListed = new bool[objCount];
		for (i = 0; i < objCount; i++) isListed[i] = false;

		int currObj = 0;

		for (i = 0; i < bone.size(); i++)
			allObjects[currObj++] = &bone[i].obj;
		for (i = 0; i < help.size(); i++)
			allObjects[currObj++] = &help[i];

		// Start transformOrder out with just the parentless objects
		currObj = 0;
		for (i = 0; i < objCount; i++)
		{
			if (allObjects[i]->parent == -1) {
				transformOrder[currObj++] = allObjects[i];
				isListed[i] = true;
#ifdef DEBUG
				out << "Adding " << i << " as parentless.\n";
#endif
			}
		}
#ifdef DEBUG
		out << "currObj=" << currObj << ", objCount=" << objCount << endl;
#endif
		int searchLow = 0;
		int searchHigh = currObj;
		while (currObj < objCount)
		{

			for (i = 0; i < objCount; i++) // The one to assign
				if (!isListed[i])
				{
					//out << "Searching for a parent for " << i << " from " << searchLow << " to " << searchHigh << endl;
					for (j = searchLow; j < searchHigh; j++)
						if (allObjects[i]->parent == transformOrder[j]->objectID)
						{ // Found one to assign it to
							//out << "Assigning " << i << " to " << j << endl;
							transformOrder[currObj++] = allObjects[i];
							isListed[i] = true;
						}
				}

			searchLow = searchHigh;
			searchHigh = currObj;
		}

		delete[] isListed;
		delete[] allObjects;

		transformOrderBuilt = true;
	}

	~MDLX()
	{
		if (transformOrderBuilt)
		{
			delete[] transformOrder;
		}

		//delete[] transformOrder;
	}
	MDLX()
	{
		transformOrderBuilt = false;
	}
#endif
};

#endif