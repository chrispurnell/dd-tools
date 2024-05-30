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

static void print_vector(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	indent(ofp, ind);
	fprintf(ofp, "<vector name=\"%s\">\n", name);
	int in = ind + 1;
	print_float(ifp, ofp, in, "x");
	print_float(ifp, ofp, in, "y");
	print_float(ifp, ofp, in, "z");
	indent(ofp, ind);
	fprintf(ofp, "</vector>\n");
}

static int hed2xml(FILE *ifp, char *file)
{
	char path[PATH_MAX];
	snprintf(path, PATH_MAX, "%s.xml", file);
	FILE *ofp = fopen(path, "w");
	if (!ofp)
	{
		perror(path);
		return 1;
	}

	fprintf(ofp, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
	fprintf(ofp, "<hed>\n");
	
	print_int(ifp, ofp, 1, "version");

	print_float(ifp, ofp, 1, "mHeightScale");
	print_float(ifp, ofp, 1, "mShoulderScale");
	print_float(ifp, ofp, 1, "mWaistScale");
	print_float(ifp, ofp, 1, "mLegScale");
	print_float(ifp, ofp, 1, "mAnkleScale");
	print_float(ifp, ofp, 1, "mArmScale");
	print_float(ifp, ofp, 1, "mArmUpperScale");
	print_float(ifp, ofp, 1, "mArmLowerScale");
	print_float(ifp, ofp, 1, "mWeaponNullScale");
	print_int(ifp, ofp, 1, "mLimitOff");

	int n = read_int(ifp);
	indent(ofp, 1);
	fprintf(ofp, "<array name=\"mpMarkerArray\" count=\"%d\">\n", n);

	for (int i = 0; i < n; i++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class index=\"%d\">\n", i);

		print_vector(ifp, ofp, 3, "mScale");
		print_vector(ifp, ofp, 3, "mTrans");

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");

	fprintf(ofp, "</hed>\n");
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

		if (hed2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
