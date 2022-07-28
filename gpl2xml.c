#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

static const char *enemy_names[][2] = {
	{ "em0100", "Goblin" },
	{ "em0101", "Hobgoblin" },
	{ "em0102", "Grimgoblin" },
	{ "em0103", "Greater Goblin" },
	{ "em0200", "Wolf" },
	{ "em0201", "Direwolf" },
	{ "em0202", "Hellhound" },
	{ "em0203", "Warg" },
	{ "em0204", "Garm" },
	{ "em0400", "Saurian" },
	{ "em0401", "Sulfur Saurian" },
	{ "em0402", "Geo Saurian" },
	{ "em0403", "Saurian Sage" },
	{ "em0404", "Pyre Saurian" },
	{ "em0405", "Giant Saurian" },
	{ "em0406", "Giant Sulfur Saurian" },
	{ "em0407", "Giant Geo Saurian" },
	{ "em0408", "Giant Saurian Sage" },
	{ "em0500", "Undead Male" },
	{ "em0501", "Undead Female" },
	{ "em0502", "Stout Undead" },
	{ "em0503", "Undead Warrior" },
	{ "em0504", "Giant Undead" },
	{ "em0505", "Poison Undead" },
	{ "em0506", "Banshee" },
	{ "em0507", "Eliminator" },
	{ "em0600", "Harpy" },
	{ "em0601", "Snow Harpy" },
	{ "em0602", "Succubus" },
	{ "em0603", "Gargoyle" },
	{ "em0604", "Stigoi" },
	{ "em0605", "Siren" },
	{ "em0700", "Phantom" },
	{ "em0701", "Phantasm" },
	{ "em0702", "Specter" },
	{ "em0703", "Wraith" },
	{ "em0900", "Ogre" },
	{ "em0901", "Elder Ogre" },
	{ "em1000", "Bandit" },
	{ "em2000", "Skeleton" },
	{ "em2001", "Skeleton Knight" },
	{ "em2002", "Skeleton Lord" },
	{ "em2003", "Skeleton Warrior" },
	{ "em2004", "Skeleton Brute" },
	{ "em2005", "Gold Knight" },
	{ "em2006", "Silver Knight" },
	{ "em2007", "Living Armor" },
	{ "em2100", "Skeleton Mage" },
	{ "em2101", "Skeleton Sorcerer" },
	{ "em5000", "Cyclops" },
	{ "em5001", "Gorecyclops" },
	{ "em5100", "Golem" },
	{ "em5101", "Metal Golem" },
	{ "em5200", "Chimera" },
	{ "em5201", "Gorechimera" },
	{ "em5300", "Hydra" },
	{ "em5301", "Archydra" },
	{ "em5400", "Griffin" },
	{ "em5401", "Cockatrice" },
	{ "em5500", "Evil Eye" },
	{ "em5501", "Vile Eye" },
	{ "em5502", "Gazer" },
	{ "em5700", "Wight" },
	{ "em5800", "The Dragon" },
	{ "em5801", "Ur-Dragon" },
	{ "em5900", "Drake" },
	{ "em5901", "Wyrm" },
	{ "em5902", "Wyvern" },
	{ "em5903", "Firedrake" },
	{ "em5904", "Frostwyrm" },
	{ "em5905", "Thunderwyvern" },
	{ "em5906", "Cursed Dragon" },
	{ "em6000", "Wight" },
	{ "em6001", "Lich" },
	{ "em6002", "Dark Bishop" },
	{ "em6003", "Death" },
	{ "em7000", "Daimon" },
	{ NULL, NULL }
};

static const char *get_enemy_name(const char *string)
{
	int i = 0;

	while (enemy_names[i][0])
	{
		if (strcmp(enemy_names[i][0], string) == 0)
		{
			return enemy_names[i][1];
		}

		i++;
	}

	return NULL;
}

static int read_short(FILE *fp)
{
	short i = 0;
	fread(&i, 2, 1, fp);
	return i;
}

static int read_int(FILE *fp)
{
	int i = 0;
	fread(&i, 4, 1, fp);
	return i;
}

static float read_float(FILE *fp)
{
	float f = 0;
	fread(&f, 4, 1, fp);
	return f;
}

static char *read_string(FILE *fp)
{
	static char buf[256];
	char c;
	int i = 0;
	
	while ((c = fgetc(fp)) >= ' ' && i < 255)
	{
		buf[i++] = c;
	}

	buf[i] = 0;
	return buf;
}

static void indent(FILE *fp, int ind)
{
	for (int i = 0; i < ind; i++)
	{
		fputc('\t', fp);
	}
}

static int print_byte(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	char i = fgetc(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<i8 name=\"%s\" value=\"%d\"/>\n", name, i);
	return i;
}

static int print_short(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	int i = read_short(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<i16 name=\"%s\" value=\"%d\"/>\n", name, i);
	return i;
}

static int print_int(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	int i = read_int(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<i32 name=\"%s\" value=\"%d\"/>\n", name, i);
	return i;
}

static int print_hex(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	int i = read_int(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<i32 name=\"%s\" value=\"0x%X\"/>\n", name, i);
	return i;
}

static float print_float(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	char str[64];
	float f = read_float(ifp);

	for (int i = 1; i < 48; i++)
	{
		snprintf(str, 64, "%.*f", i, f);
		if ((float)atof(str) == f)
			break;
	}

	indent(ofp, ind);
	fprintf(ofp, "<f32 name=\"%s\" value=\"%s\"/>\n", name, str);
	return f;
}

static char *print_string(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	char *str = read_string(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<string name=\"%s\" value=\"%s\"/>", name, str);
	const char *ename = get_enemy_name(str);
	if (ename) fprintf(ofp, "<!-- %s -->", ename);
	fputc('\n', ofp);
	return str;
}

static void print_array(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	int count = read_int(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<array name=\"%s\" count=\"%d\">\n", name, count);

	for (int i = 0; i < count; i++)
	{
		indent(ofp, ind + 1);
		fprintf(ofp, "<i32 index=\"%d\" value=\"0x%X\"/>\n", i, read_int(ifp));
	}

	indent(ofp, ind);
	fprintf(ofp, "</array>\n");
}

static void print_vector(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	indent(ofp, ind);
	fprintf(ofp, "<vector name=\"%s\">\n", name);
	int in = ind + 1;
	print_float(ifp, ofp, in, "x");
	print_float(ifp, ofp, in, "y");
	print_float(ifp, ofp, in, "z");
	print_float(ifp, ofp, in, "w");
	indent(ofp, ind);
	fprintf(ofp, "</vector>\n");
}

static int print_bits(FILE *ofp, int ind, int bits, const char *name, int value)
{
	indent(ofp, ind);
	fprintf(ofp, "<b%u name=\"%s\" value=\"%d\">\n", bits, name, value);
}

static int print_shape(FILE *ifp, FILE *ofp, int ind)
{
	indent(ofp, ind);
	fprintf(ofp, "<class>\n");
	int in = ind + 1;

	print_string(ifp, ofp, in, "mName");
	print_float (ifp, ofp, in, "mCheckAngle");
	print_float (ifp, ofp, in, "mCheckRange");
	print_float (ifp, ofp, in, "mCheckToward");
	print_byte  (ifp, ofp, in, "mAngleFlag");
	print_byte  (ifp, ofp, in, "mTowardFlag");

	int type = print_int(ifp, ofp, in, "type");
	print_float (ifp, ofp, in, "mDecay");
	print_byte  (ifp, ofp, in, "mIsNativeData");

	switch (type)
	{
	case 1:
		print_float (ifp, ofp, in, "mHeight");
                print_float (ifp, ofp, in, "mBottom");

		print_vector(ifp, ofp, in, "mVertex[0]");
		print_vector(ifp, ofp, in, "mVertex[1]");
		print_vector(ifp, ofp, in, "mVertex[2]");
		print_vector(ifp, ofp, in, "mVertex[3]");
		print_vector(ifp, ofp, in, "mConcaveCrossPos");
                print_byte  (ifp, ofp, in, "mFlgConvex");
                print_int   (ifp, ofp, in, "mConcaveStatus");
                break;

        case 2:
		print_vector(ifp, ofp, in, "mVertex");
                break;

        case 3:
		print_vector(ifp, ofp, in, "Position0");
		print_vector(ifp, ofp, in, "Position1");
                print_float (ifp, ofp, in, "Radius");
                print_hex   (ifp, ofp, in, "pad1");
                print_hex   (ifp, ofp, in, "pad2");
                print_hex   (ifp, ofp, in, "pad3");
                break;

	default:
		return 1;
	}

	indent(ofp, ind);
	fprintf(ofp, "</class>\n");
	return 0;
}

static int gpl2xml(FILE *ifp, char *file)
{
	if (read_int(ifp) != 7106663)
	{
		printf("Bad magic in file %s\n", file);
		return 1;
	}

	char path[PATH_MAX];
	snprintf(path, PATH_MAX, "%s.xml", file);
	FILE *ofp = fopen(path, "w");
	if(!ofp)
	{
		perror(path);
		return 1;
	}

	fprintf(ofp, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
	fprintf(ofp, "<gpl magic=\"0x6C7067\">\n");
	print_int(ifp, ofp, 1, "version");
	print_array(ifp, ofp, 1, "mGroupList");
	print_array(ifp, ofp, 1, "mSetBit");
	print_int(ifp, ofp, 1, "mDLCNo");
	int count = read_int(ifp);
	indent(ofp, 1);
	fprintf(ofp, "<array count=\"%d\">\n", count);

	for (int i = 0; i < count; i++)
	{
		int n, j;

		indent(ofp, 2);
		fprintf(ofp, "<class>\n");
		print_int(ifp, ofp, 3, "mGroupClass");

		n = read_int(ifp);
		print_bits(ofp, 3, 9, "mGroup", n & 0x1FF);
		print_bits(ofp, 3, 18, "mPriority", (n >> 9) & 0x3FFFF);
		print_bits(ofp, 3, 1, "mIsDisableSplit", (n >> 27) & 0x1);
		print_bits(ofp, 3, 4, "mDLCNo", (n >> 28) & 0xF);

		n = read_int(ifp);
		indent(ofp, 3);
		fprintf(ofp, "<array name=\"mUnitKindList\" count=\"%d\">\n", n);
		for (j = 0; j < n; j++)
		{
			indent(ofp, 4);
			fprintf(ofp, "<class>\n");
			print_string(ifp, ofp, 5, "name");
			print_byte  (ifp, ofp, 5, "isBelong");
			indent(ofp, 4);
			fprintf(ofp, "</class>\n");
		}
		indent(ofp, 3);
		fprintf(ofp, "</array>\n");

		n = read_int(ifp);
		indent(ofp, 3);
		fprintf(ofp, "<array name=\"mLayoutIDArray\" count=\"%d\">\n", n);
		for (j = 0; j < n; j++)
		{
			indent(ofp, 4);
			fprintf(ofp, "<class>\n");
			print_int(ifp, ofp, 5, "mLayoutID");
			print_int(ifp, ofp, 5, "mGroup");
			print_int(ifp, ofp, 5, "mSplitX");
			print_int(ifp, ofp, 5, "mSplitZ");
			indent(ofp, 4);
			fprintf(ofp, "</class>\n");
		}
		indent(ofp, 3);
		fprintf(ofp, "</array>\n");

		print_int  (ifp, ofp, 3, "Next\351\200\232\345\270\270");
		print_int  (ifp, ofp, 3, "Random Division");
		print_int  (ifp, ofp, 3, "Next\347\211\271\346\256\212");
		print_int  (ifp, ofp, 3, "\346\212\275\351\201\270\343\201\253\345\244\226\343\202\214\343\201\246\343\201\204\343\201\246\343\202\202\351\253\230\345\204\252\345\205\210");
		print_int  (ifp, ofp, 3, "\346\211\200\346\214\201\343\202\242\343\202\244\343\203\206\343\203\240");
		print_int  (ifp, ofp, 3, "Random Pattern");
		print_int  (ifp, ofp, 3, "\346\216\222\344\273\226\343\201\225\343\202\214\343\202\213\347\211\271\346\256\212\343\203\221\343\202\277\343\203\274\343\203\263\343\202\222\346\214\207\345\256\232");
		print_int  (ifp, ofp, 3, "\346\216\222\344\273\226\343\201\225\343\202\214\343\202\213\347\211\271\346\256\212\343\203\221\343\202\277\343\203\274\343\203\263");
		print_int  (ifp, ofp, 3, "\346\255\273\347\245\236\347\224\250\345\206\215\343\202\273\343\203\203\343\203\210\345\276\205\343\201\241");
		print_int  (ifp, ofp, 3, "\346\255\273\344\275\223\343\202\222\343\202\273\343\203\203\343\203\210\343\201\227\343\201\252\343\201\204");
		print_int  (ifp, ofp, 3, "\346\255\273\344\275\223\346\271\247\343\201\215\343\202\250\343\203\225\343\202\247\343\202\257\343\203\210\343\202\222\345\274\267\345\210\266\343\202\273\343\203\203\343\203\210");
		print_int  (ifp, ofp, 3, "\346\234\200\345\204\252\345\205\210\346\231\202\343\200\201\344\273\226\343\202\260\343\203\253\343\203\274\343\203\227\343\201\256\343\203\246\343\203\213\343\203\203\343\203\210\343\202\222\343\202\273\343\203\203\343\203\210\343\201\227\343\201\252\343\201\204");
		print_int  (ifp, ofp, 3, "mLoadCondition.mScenario");
		print_int  (ifp, ofp, 3, "mAppearBgn");
		print_int  (ifp, ofp, 3, "mAppearEnd");
		print_int  (ifp, ofp, 3, "mLoadCondition.mLotFlag");
		print_int  (ifp, ofp, 3, "mLoadCondition.mLotFlag2");
		print_short(ifp, ofp, 3, "mDataLotFlag.mFlagNo");
		print_short(ifp, ofp, 3, "mDataLotFlag.mFlagNo2");
		print_int  (ifp, ofp, 3, "mSetCondition.mTime");
		print_short(ifp, ofp, 3, "mDataSetHour.mSetHourBgn");
		print_short(ifp, ofp, 3, "mDataSetHour.mSetHourEnd");
		print_int  (ifp, ofp, 3, "mSetCondition.mTimeKA");
		print_int  (ifp, ofp, 3, "\346\211\200\346\214\201\343\202\242\343\202\244\343\203\206\343\203\240No.");
		print_int  (ifp, ofp, 3, "mSetCondition.mAreaHit");

		n = read_int(ifp);
		indent(ofp, 3);
		fprintf(ofp, "<array name=\"mAreaHitShapeList\" count=\"%d\">\n", n);
		for (j = 0; j < n; j++)
		{
			if (print_shape(ifp, ofp, 4))
			{
				printf("Error reading: %s\n", file);
				fclose(ofp);
				return 1;
			}
		}
		indent(ofp, 3);
		fprintf(ofp, "</array>\n");

		print_int(ifp, ofp, 3, "mSetCondition.mFsm");
		print_int(ifp, ofp, 3, "mSetCondition.mRequest");
		print_int(ifp, ofp, 3, "mSetCondition.mChArea");
		print_int(ifp, ofp, 3, "mSetCondition.mSimpleEv");
		print_int(ifp, ofp, 3, "mSetCountMax");
		print_int(ifp, ofp, 3, "mSetCondition.mIsEmGroupLink");
		print_int(ifp, ofp, 3, "mSetCondition.mLinkEmGroup");
		print_int(ifp, ofp, 3, "mDeleteCondition.mLotFlag");
		print_int(ifp, ofp, 3, "mDeleteCondition.mScenario");
		print_int(ifp, ofp, 3, "mDeleteCondition.mTime");
		print_int(ifp, ofp, 3, "mRspnCondition.mRspnType");
		print_int(ifp, ofp, 3, "mRspnCondition.mRspnDay");
		print_int(ifp, ofp, 3, "mRspnCondition.mRspnProb");
		print_int(ifp, ofp, 3, "mRspnCondition.mRspnProbAdd");
		print_int(ifp, ofp, 3, "mRspnCondition.mRspnForceRspn");

		n = read_int(ifp);
		indent(ofp, 3);
		fprintf(ofp, "<array name=\"mLifeAreaArray\" count=\"%d\">\n", n);
		for (j = 0; j < n; j++)
		{
			int m = read_int(ifp);
			indent(ofp, 4);
			fprintf(ofp, "<array count=\"%d\">\n", m);
			for (int k = 0; k < m; k++)
			{
				if (print_shape(ifp, ofp, 5))
				{
					printf("Error reading: %s\n", file);
					fclose(ofp);
					return 1;
				}
			}
			indent(ofp, 4);
			fprintf(ofp, "</array>\n");
		}
		indent(ofp, 3);
		fprintf(ofp, "</array>\n");

		print_int(ifp, ofp, 3, "mKillAreaType");

		n = read_int(ifp);
		indent(ofp, 3);
		fprintf(ofp, "<array name=\"mKillAreaList\" count=\"%d\">\n", n);
		for (j = 0; j < n; j++)
		{
			if (print_shape(ifp, ofp, 4))
			{
				printf("Error reading: %s\n", file);
				fclose(ofp);
				return 1;
			}
		}
		indent(ofp, 3);
		fprintf(ofp, "</array>\n");
		
		print_byte(ifp, ofp, 3, "\343\201\257\343\201\204\343\201\213\343\201\204\343\202\250\343\203\252\343\202\242\343\202\222\345\205\261\346\234\211\343\201\231\343\202\213");
		print_int (ifp, ofp, 3, "\345\205\261\346\234\211\343\201\257\343\201\204\343\201\213\343\201\204\343\202\250\343\203\252\343\202\242\343\202\260\343\203\253\343\203\274\343\203\227");
		print_byte(ifp, ofp, 3, "KILL\343\202\250\343\203\252\343\202\242\343\202\222\345\205\261\346\234\211\343\201\231\343\202\213");
		print_int (ifp, ofp, 3, "\345\205\261\346\234\211Kill\343\202\250\343\203\252\343\202\242\343\202\260\343\203\253\343\203\274\343\203\227");

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");
	fprintf(ofp, "</gpl>\n");
	fclose(ofp);
	return 0;
}

int main(int argc, char **argv)
{
	int ret = 0;

	for (int i = 1; i < argc; i++)
	{
		FILE *fp = fopen(argv[i], "rb");
		if (!fp)
		{
			perror(argv[i]);
			ret = 1;
			continue;
		}

		if (gpl2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
