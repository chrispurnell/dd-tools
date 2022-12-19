#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "item_names.h"

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

static int print_int(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	int i = read_int(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<i32 name=\"%s\" value=\"%d\"/>\n", name, i);
	return i;
}

static int print_item(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	unsigned i = read_int(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<i32 name=\"%s\" value=\"%d\"/>", name, i);
	if (i < 1901) fprintf(ofp, "<!-- %s -->", item_names[i]);
	fputc('\n', ofp);
	return i;
}

static int imx2xml(FILE *ifp, char *file)
{
	if (read_int(ifp) != 7892329)
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
	fprintf(ofp, "<imx magic=\"0x786D69\">\n");
	print_int(ifp, ofp, 1, "version");

	int count = read_int(ifp);
	indent(ofp, 1);
	fprintf(ofp, "<array count=\"%d\">\n", count);

	for (int i = 0; i < count; i++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class>\n");

		print_item(ifp, ofp, 3, "mBaseItemNo0");
		print_item(ifp, ofp, 3, "mBaseItemNo1");
		print_item(ifp, ofp, 3, "mNewItemNo");
		print_int(ifp, ofp, 3, "mNewItemNum");
		
		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");
	fprintf(ofp, "</imx>\n");
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

		if (imx2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
