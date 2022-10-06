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

static void print_bits(FILE *ofp, int ind, int bits, const char *name, int value)
{
	int i = value & ((1 << bits) - 1);
	indent(ofp, ind);
	fprintf(ofp, "<b%u name=\"%s\" value=\"%d\"/>\n", bits, name, i);
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

static void print_float_array(FILE *ifp, FILE *ofp, int ind, const char *name, int count)
{
	char str[64];
	indent(ofp, ind);
	fprintf(ofp, "<array name=\"%s\">\n", name);

	for (int i = 0; i < count; i++)
	{
		indent(ofp, ind + 1);
		float2str(str, read_float(ifp));
		fprintf(ofp, "<f32 index=\"%d\" value=\"%s\"/>\n", i, str);
	}

	indent(ofp, ind);
	fprintf(ofp, "</array>\n");
}

static char *print_string(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	char *str = read_string(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<string name=\"%s\" value=\"%s\"/>\n", name, str);
	return str;
}

static int gfd2xml(FILE *ifp, char *file)
{
	if (read_int(ifp) != 4474439)
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
	fprintf(ofp, "<gfd magic=\"0x444647\">\n");
	print_int(ifp, ofp, 1, "mVersion");
	print_int(ifp, ofp, 1, "mAttr");
	print_int(ifp, ofp, 1, "mSuffix");
	print_int(ifp, ofp, 1, "mFontType");
	print_int(ifp, ofp, 1, "mFontSize");
	print_int(ifp, ofp, 1, "mTextureNum");
	int mCharNum = print_int(ifp, ofp, 1, "mCharNum");
	int mDescentNum = print_int(ifp, ofp, 1, "mDescentNum");
	print_float(ifp, ofp, 1, "mMaxAscent");
	print_float(ifp, ofp, 1, "mMaxDescent");
	print_float_array(ifp, ofp, 1, "mpDescent", mDescentNum);
	print_int(ifp, ofp, 1, "strlen");
	print_string(ifp, ofp, 1, "mpTexturePath");

	indent(ofp, 1);
	fprintf(ofp, "<array name=\"mpChar\">\n");

	for (int i = 0; i < mCharNum; i++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class>\n");

		print_byte(ifp, ofp, 3, "texture");
		print_byte(ifp, ofp, 3, "decent");
		print_short(ifp, ofp, 3, "unknown");
		print_short(ifp, ofp, 3, "x");
		print_short(ifp, ofp, 3, "y");

		int n = read_int(ifp);
		print_bits(ofp, 3, 12, "width", n);
		print_bits(ofp, 3, 12, "height", n >> 12);
		print_bits(ofp, 3, 8, "pad", n >> 24);

		print_hex(ifp, ofp, 3, "codepoint");

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");

	fprintf(ofp, "</gfd>\n");
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

		if (gfd2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
