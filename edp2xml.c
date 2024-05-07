#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

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
	int c;
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

static void float2str(char *str, float f)
{
        for (int i = 1; i < 48; i++)
        {
                snprintf(str, 64, "%.*f", i, f);
                if (strtof(str, NULL) == f)
                        break;
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
	float2str(str, f);
	indent(ofp, ind);
	fprintf(ofp, "<f32 name=\"%s\" value=\"%s\"/>\n", name, str);
	return f;
}

static char *print_string(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	char *str = read_string(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<string name=\"%s\" value=\"%s\"/>\n", name, str);
	return str;
}

static void print_color(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	char str[64];
	for (int i = 0; i < 4; i++)
	{
		float2str(str, read_float(ifp));
		indent(ofp, ind);
		fprintf(ofp, "<f32 name=\"%s[%d]\" value=\"%s\"/>\n", name, i, str);
	}
}

static int edp2xml(FILE *ifp, char *file)
{
	if (read_int(ifp) != 7365733)
	{
		printf("Bad magic in file %s\n", file);
		return 1;
	}

	char path[PATH_MAX];
        snprintf(path, PATH_MAX, "%s.xml", file);
        FILE *ofp = fopen(path, "w");
        if (!ofp)
        {
                perror(path);
                return 1;
        }

	fprintf(ofp, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
        fprintf(ofp, "<edp magic=\"0x706465\">\n");

	print_int(ifp, ofp, 1, "version");

	print_short(ifp, ofp, 1, "mModelType");
	print_short(ifp, ofp, 1, "mNpcType");
	print_short(ifp, ofp, 1, "mJobType");
	print_short(ifp, ofp, 1, "mJobPl");
	print_float(ifp, ofp, 1, "mManWoman");
	print_float(ifp, ofp, 1, "mStoop");
	print_string(ifp, ofp, 1, "mprHumanEdit");
	print_string(ifp, ofp, 1, "mprBodyEdit");
	print_string(ifp, ofp, 1, "mprFaceEdit");
	print_hex(ifp, ofp, 1, "mMainWpnId");
	print_hex(ifp, ofp, 1, "mSubWpnId");
	print_hex(ifp, ofp, 1, "mQuiverId");
	print_int(ifp, ofp, 1, "mInnerId");
	print_hex(ifp, ofp, 1, "mWearId[0]");
	print_hex(ifp, ofp, 1, "mWearId[1]");
	print_hex(ifp, ofp, 1, "mArmorId[0]");	
	print_hex(ifp, ofp, 1, "mArmorId[1]");	
	print_hex(ifp, ofp, 1, "mArmorId[2]");	
	print_hex(ifp, ofp, 1, "mArmorId[3]");	
	print_hex(ifp, ofp, 1, "mArmorId[4]");
	print_hex(ifp, ofp, 1, "mHairId");
	print_hex(ifp, ofp, 1, "mBeardId");
	print_color(ifp, ofp, 1, "mColorEyeR");
	print_color(ifp, ofp, 1, "mColorEyeL");
	print_color(ifp, ofp, 1, "mColorHair");
	print_color(ifp, ofp, 1, "mColorBody");
	print_float(ifp, ofp, 1, "mWrinkleFace");
	print_float(ifp, ofp, 1, "mWrinkleBody");
	print_float(ifp, ofp, 1, "mEyebrowOfs[0]");
	print_float(ifp, ofp, 1, "mEyebrowOfs[1]");
	print_float(ifp, ofp, 1, "mEyebrowScl[0]");
	print_float(ifp, ofp, 1, "mEyebrowScl[1]");
	print_int(ifp, ofp, 1, "mEyebrowTex");
	print_int(ifp, ofp, 1, "mGoodsIdR");
	print_int(ifp, ofp, 1, "mGoodsIdL");
	print_byte(ifp, ofp, 1, "mChainFlag");
	print_byte(ifp, ofp, 1, "mHairSwingFlag");
	print_short(ifp, ofp, 1, "mAttackMotType");

	fprintf(ofp, "</edp>\n");
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

		if (edp2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
