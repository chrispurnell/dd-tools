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

static int aor2xml(FILE *ifp, char *file)
{
	if (read_int(ifp) != 7499617)
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
	fprintf(ofp, "<aor magic=\"0x726f61\">\n");

	print_int(ifp, ofp, 1, "version");
	int i1 = print_int(ifp, ofp, 1, "mArrayNum");
	print_int(ifp, ofp, 1, "allocate");

	indent(ofp, 1);
	fprintf(ofp, "<array name=\"mpArray\">\n");

	for (int i = 0; i < i1; i++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class index=\"%d\">\n", i);

		print_int(ifp, ofp, 3, "mModelId");

		int i2 = read_int(ifp);
		indent(ofp, 3);
		fprintf(ofp, "<array name=\"mpPartslList\" count=\"%d\">\n", i2);

		for (int j = 0; j < i2; j++)
		{
			indent(ofp, 4);
			fprintf(ofp, "<class index=\"%d\">\n", j);
			
			print_short(ifp, ofp, 5, "mChkModelId");
			print_short(ifp, ofp, 5, "mPartsNo");

			indent(ofp, 4);
			fprintf(ofp, "</class>\n");
		}

		indent(ofp, 3);
		fprintf(ofp, "</array>\n");

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");

	fprintf(ofp, "</aor>\n");
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

		if (aor2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
