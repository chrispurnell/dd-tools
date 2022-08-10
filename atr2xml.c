#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

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

static int print_int(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	int i = read_int(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<i32 name=\"%s\" value=\"%d\"/>\n", name, i);
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
	fprintf(ofp, "<string name=\"%s\" value=\"%s\"/>\n", name, str);
	return str;
}

static int atr2xml(FILE *ifp, char *file)
{
	if (read_int(ifp) != 7500897)
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
	fprintf(ofp, "<atr magic=\"0x727461\">\n");

	print_int(ifp, ofp, 1, "version");
	int i1 = print_int(ifp, ofp, 1, "mArrayNum");
	print_int(ifp, ofp, 1, "allocate");

	indent(ofp, 1);
	fprintf(ofp, "<array name=\"mpArray\">\n");

	for (int i = 0; i < i1; i++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class>\n");

		indent(ofp, 3);
		fprintf(ofp, "<u32 name=\"mArmorId\">\n");
		int n = read_int(ifp);
		indent(ofp, 4);
                fprintf(ofp, "<b24 value=\"%d\"/>\n", (n << 8) >> 8);
                indent(ofp, 4);
                fprintf(ofp, "<b8 value=\"%d\"/>\n", n >> 24);
                indent(ofp, 3);
                fprintf(ofp, "</i32>\n");

		print_int(ifp, ofp, 3, "mSoundPri");
		print_int(ifp, ofp, 3, "mSoundType");
		
		int i2 = read_int(ifp);

		indent(ofp, 3);
		fprintf(ofp, "<array name=\"mpModelList\" count=\"%d\">\n", i2);

		for (int j = 0; j < i2; j++)
		{
			print_int(ifp, ofp, 4, "mModelId");
		}

		indent(ofp, 3);
		fprintf(ofp, "</array>\n");

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");

	fprintf(ofp, "</atr>\n");
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

		if (atr2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
