#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "item_names.h"

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

static void print_item_array(FILE *ifp, FILE *ofp, int ind, const char *name, int count)
{
	indent(ofp, ind);
	fprintf(ofp, "<array name=\"%s\">\n", name);
	int in = ind + 1;

	for (int i = 0; i < count; i++)
	{
		indent(ofp, in);
		unsigned it = read_short(ifp);
		fprintf(ofp, "<i16 index=\"%d\" value=\"%d\"/>", i, it);
		if (it < 1901) fprintf(ofp, "<!-- %s -->", item_names[it]);
		fputc('\n', ofp);
	}

	indent(ofp, ind);
	fprintf(ofp, "</array>\n");
}

static void print_short_array(FILE *ifp, FILE *ofp, int ind, const char *name, int count)
{
	indent(ofp, ind);
	fprintf(ofp, "<array name=\"%s\">\n", name);
	int in = ind + 1;

	for (int i = 0; i < count; i++)
	{
		indent(ofp, in);
		fprintf(ofp, "<i16 index=\"%d\" value=\"%d\"/>\n", i, read_short(ifp));
	}

	indent(ofp, ind);
	fprintf(ofp, "</array>\n");
}

static int cit2xml(FILE *ifp, char *file)
{
	if (read_int(ifp) != 7629155)
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
	fprintf(ofp, "<cit magic=\"0x746963\">\n");

	print_int(ifp, ofp, 1, "version");

	int n = read_int(ifp);
	indent(ofp, 1);
	fprintf(ofp, "<array count=\"%d\">\n", n);

	for (int i = 0; i < n; i++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class index=\"%d\">\n", i);

		print_short(ifp, ofp, 3, "mLevel");
		print_item_array(ifp, ofp, 3, "mGetItemNo", 20);
		print_short_array(ifp, ofp, 3, "mGetItemRate", 20);
		
		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");

	fprintf(ofp, "</cit>\n");
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

		if (cit2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
