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

static void print_int_array(FILE *ifp, FILE *ofp, int ind, const char *name, int count)
{
	indent(ofp, ind);
	fprintf(ofp, "<array name=\"%s\">\n", name);

	for (int i = 0; i < count; i++)
	{
		indent(ofp, ind + 1);
		fprintf(ofp, "<i32 index=\"%d\" value=\"%d\"/>\n", i, read_int(ifp));
	}

	indent(ofp, ind);
	fprintf(ofp, "</array>\n");
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

static int ite2xml(FILE *ifp, char *file)
{
	if (read_int(ifp) != 6648937)
	{
		printf("Bad magic in file %s\n", file);
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
	fprintf(ofp, "<ite magic=\"0x657469\">\n");
	print_int(ifp, ofp, 1, "version");

	int count = read_int(ifp);
	indent(ofp, 1);
	fprintf(ofp, "<array count=\"%d\">\n", count);

	for (int i = 0; i < count; i++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class index=\"%d\">\n", i);

		print_int_array(ifp, ofp, 3, "mUpParamLv1", 2);
		print_int_array(ifp, ofp, 3, "mUpParamLv2", 4);
		print_int_array(ifp, ofp, 3, "mUpParamLv3", 6);
		print_int_array(ifp, ofp, 3, "mUpParamLv4", 8);
		print_int_array(ifp, ofp, 3, "mUpParamLv5", 8);
		print_int_array(ifp, ofp, 3, "mUpParamLv6", 8);
		print_float_array(ifp, ofp, 3, "mUpRate1", 2);
		print_float_array(ifp, ofp, 3, "mUpRate2", 4);
		print_float_array(ifp, ofp, 3, "mUpRate3", 6);
		print_float_array(ifp, ofp, 3, "mUpRate4", 8);
		print_float_array(ifp, ofp, 3, "mUpRate5", 8);
		print_float_array(ifp, ofp, 3, "mUpRate6", 8);
		print_int_array(ifp, ofp, 3, "mIsDirectValue1", 2);
		print_int_array(ifp, ofp, 3, "mIsDirectValue2", 4);
		print_int_array(ifp, ofp, 3, "mIsDirectValue3", 6);
		print_int_array(ifp, ofp, 3, "mIsDirectValue4", 8);
		print_int_array(ifp, ofp, 3, "mIsDirectValue5", 8);
		print_int_array(ifp, ofp, 3, "mIsDirectValue6", 8);

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");
	fprintf(ofp, "</ite>\n");
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

		if (ite2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
