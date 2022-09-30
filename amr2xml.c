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
		if (strtof(str, NULL) == f)
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

static int amr2xml(FILE *ifp, char *file)
{
	if (read_int(ifp) != 7499105)
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
	fprintf(ofp, "<amr magic=\"0x726D61\">\n");

	print_int(ifp, ofp, 1, "version");
	int i1 = print_int(ifp, ofp, 1, "mArrayInfoNum");
	int i2 = print_int(ifp, ofp, 1, "mArrayBlendNum");

	indent(ofp, 1);
	fprintf(ofp, "<array name=\"mpArrayInfo\">\n");

	for (int i = 0; i < i1; i++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class>\n");

		print_int(ifp, ofp, 3, "mModelId");
		print_int(ifp, ofp, 3, "mType");
		print_string(ifp, ofp, 3, "mFname");

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");

	indent(ofp, 1);
	fprintf(ofp, "<array name=\"mpArrayBlend\">\n");

	for (int i = 0; i < i2; i++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class>\n");

		print_int(ifp, ofp, 3, "mModelId");
		print_float(ifp, ofp, 3, "mRate");

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");

	fprintf(ofp, "</amr>\n");
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

		if (amr2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
