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

static int read_short(FILE *fp)
{
	short i = 0;
	fread(&i, 2, 1, fp);
	return i;
}

static char *read_string(FILE *fp)
{
	static char buf[256];
	int c;
	int i = 0;
	
	while ((c = fgetc(fp)) > 0 && i < 255)
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

static char *print_string(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	char *str = read_string(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<string name=\"%s\" value=\"%s\"/>\n", name, str);
	return str;
}

static int nnl2xml(FILE *ifp, char *file)
{
	if (read_int(ifp) != 7106158)
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
	fprintf(ofp, "<nnl magic=\"0x6C6E6E\">\n");

	print_int(ifp, ofp, 1, "version");
	int n = read_int(ifp);

	indent(ofp, 1);
	fprintf(ofp, "<array count=\"%d\">\n", n);

	for (int i = 0; i < n; i++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class index=\"%d\">\n", i);

		print_short(ifp, ofp, 3, "mNo");
		print_short(ifp, ofp, 3, "mStageNo");
		print_short(ifp, ofp, 3, "mInitmFriendPoint");
		print_short(ifp, ofp, 3, "mModelType");
		print_short(ifp, ofp, 3, "mIndex");
		print_short(ifp, ofp, 3, "mStrayId");
		print_short(ifp, ofp, 3, "mShopType");
		print_short(ifp, ofp, 3, "mSeType");
		print_byte(ifp, ofp, 3, "mFlag");
		print_byte(ifp, ofp, 3, "mChild");
		print_byte(ifp, ofp, 3, "mTP");
		print_byte(ifp, ofp, 3, "mTPType");
		print_byte(ifp, ofp, 3, "mLikeItem");
		print_byte(ifp, ofp, 3, "mCivilian");
		print_string(ifp, ofp, 3, "mName");
		print_string(ifp, ofp, 3, "mNameJ");

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");

	fprintf(ofp, "</nnl>\n");
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

		if (nnl2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
