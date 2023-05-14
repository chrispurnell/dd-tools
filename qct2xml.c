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

static char *read_string(FILE *fp)
{
	static char buf[256];
	int c;
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

static char *print_string(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	char *str = read_string(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<string name=\"%s\" value=\"%s\"/>\n", name, str);
	return str;
}

static int qct2xml(FILE *ifp, char *file)
{
	if (read_int(ifp) != 7627633)
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
	fprintf(ofp, "<qct magic=\"0x746371\">\n");

	print_int(ifp, ofp, 1, "version");
	print_int(ifp, ofp, 1, "unknown");

	int n1 = read_int(ifp);
	indent(ofp, 1);
	fprintf(ofp, "<array name=\"mpArray\" count=\"%d\">\n", n1);

	for (int i1 = 0; i1 < n1; i1++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class index=\"%d\">\n", i1);

		int n2 = read_int(ifp);
		indent(ofp, 3);
		fprintf(ofp, "<array name=\"mpParamTbl\" count=\"%d\">\n", n2);

		for (int i2 = 0; i2 < n2; i2++)
		{
			indent(ofp, 4);
			fprintf(ofp, "<class index=\"%d\">\n", i2);

			int n3 = read_int(ifp);
			indent(ofp, 5);
			fprintf(ofp, "<array name=\"mpQuestTblJudgment\" count=\"%d\">\n", n3);

			for (int i3 = 0; i3 < n3; i3++)
			{
				indent(ofp, 6);
				fprintf(ofp, "<class index=\"%d\">\n", i3);

				print_int(ifp, ofp, 7, "mCommand");
				print_int(ifp, ofp, 7, "mParam00");
				print_int(ifp, ofp, 7, "mParam01");
				print_int(ifp, ofp, 7, "mParam02");
				print_int(ifp, ofp, 7, "mParam03");

				indent(ofp, 6);
				fprintf(ofp, "</class>\n");
			}

			indent(ofp, 5);
			fprintf(ofp, "</array>\n");

			n3 = read_int(ifp);
			indent(ofp, 5);
			fprintf(ofp, "<array name=\"mpQuestTblResult\" count=\"%d\">\n", n3);

			for (int i3 = 0; i3 < n3; i3++)
			{
				indent(ofp, 6);
				fprintf(ofp, "<class index=\"%d\">\n", i3);

				print_int(ifp, ofp, 7, "mCommand");
				print_int(ifp, ofp, 7, "mParam00");
				print_int(ifp, ofp, 7, "mParam01");
				print_int(ifp, ofp, 7, "mParam02");
				print_int(ifp, ofp, 7, "mParam03");
				print_int(ifp, ofp, 7, "mParam04");
				print_int(ifp, ofp, 7, "mParam05");
				print_int(ifp, ofp, 7, "mParam06");
				print_int(ifp, ofp, 7, "mParam07");
				print_int(ifp, ofp, 7, "mParam08");
				print_int(ifp, ofp, 7, "mParam09");

				indent(ofp, 6);
				fprintf(ofp, "</class>\n");
			}

			indent(ofp, 5);
			fprintf(ofp, "</array>\n");

			indent(ofp, 4);
			fprintf(ofp, "</class>\n");
		}

		indent(ofp, 3);
		fprintf(ofp, "</array>\n");

		print_string(ifp, ofp, 3, "mSheetName");

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");
		
	print_int(ifp, ofp, 1, "mUid");

	fprintf(ofp, "</qct>\n");
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

		if (qct2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
