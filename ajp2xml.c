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

static int print_int(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	int i = read_int(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<i32 name=\"%s\" value=\"%d\"/>\n", name, i);
	return i;
}

static void print_float_array(FILE *ifp, FILE *ofp, int ind, const char *name, int count)
{
	char str[64];
	indent(ofp, ind);
	fprintf(ofp, "<array name=\"%s\">\n", name);
	int in = ind + 1;

	for (int i = 0; i < count; i++)
	{
		float2str(str, read_float(ifp));
		indent(ofp, in);
		fprintf(ofp, "<f32 index=\"%d\" value=\"%s\"/>\n", i, str);
	}

	indent(ofp, ind);
	fprintf(ofp, "</array>\n");
}

static int ajp2xml(FILE *ifp, char *file)
{
	if (read_int(ifp) != 7367265)
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
	fprintf(ofp, "<ajp magic=\"0x706A61\">\n");

	print_int(ifp, ofp, 1, "version");
	int n = print_int(ifp, ofp, 1, "mArrayNum");
	print_float_array(ifp, ofp, 1, "mpArray", n);

	fprintf(ofp, "</ajp>\n");
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

		if (ajp2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
