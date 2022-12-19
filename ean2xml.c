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

static void indent(FILE *fp, int ind)
{
	for (int i = 0; i < ind; i++)
	{
		fputc('\t', fp);
	}
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
		if (strtof(str, NULL) == f)
			break;
	}

	indent(ofp, ind);
	fprintf(ofp, "<f32 name=\"%s\" value=\"%s\"/>\n", name, str);
	return f;
}

static int ean2xml(FILE *ifp, char *file)
{
	if (read_int(ifp) != 5128517)
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
	fprintf(ofp, "<ean magic=\"0x4E4145\">\n");
	print_hex(ifp, ofp, 1, "version");
	int size = print_int(ifp, ofp, 1, "size");
	int count = print_int(ifp, ofp, 1, "count");

	long end = ftell(ifp) + size;

	indent(ofp, 1);
	fprintf(ofp, "<array>\n");

	for (int i = 0; i < count; i++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class>\n");

		print_int(ifp, ofp, 3, "a1");
		print_short(ifp, ofp, 3, "a2");
		print_short(ifp, ofp, 3, "a3");
		print_int(ifp, ofp, 3, "a4");
		print_int(ifp, ofp, 3, "a5");
		print_int(ifp, ofp, 3, "a6");
		print_int(ifp, ofp, 3, "a7");
		print_short(ifp, ofp, 3, "a8");
		print_short(ifp, ofp, 3, "a9");
		print_short(ifp, ofp, 3, "a10");
		print_short(ifp, ofp, 3, "a11");

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");

	indent(ofp, 1);
	fprintf(ofp, "<array>\n");

	while (ftell(ifp) < end)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class>\n");

		print_short(ifp, ofp, 3, "b1");
		print_short(ifp, ofp, 3, "b2");
		print_short(ifp, ofp, 3, "b3");
		print_short(ifp, ofp, 3, "b4");
		print_float(ifp, ofp, 3, "b5");
		print_float(ifp, ofp, 3, "b6");
		print_float(ifp, ofp, 3, "b7");
		print_float(ifp, ofp, 3, "b8");

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");

	fprintf(ofp, "</ean>\n");
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

		if (ean2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
