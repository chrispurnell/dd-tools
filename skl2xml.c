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

static int skl2xml(FILE *ifp, char *file)
{
	if (read_int(ifp) != 7105395)
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
	fprintf(ofp, "<skl magic=\"0x6C6B73\">\n");

	print_int(ifp, ofp, 1, "version");

	int n1 = print_int(ifp, ofp, 1, "mNormalSkillNum");

	indent(ofp, 1);
	fprintf(ofp, "<array name=\"mpNormalSkill\">\n");

	for (int i = 0; i < n1; i++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class index=\"%d\">\n", i);

		print_int(ifp, ofp, 3, "mSkillId");
		print_int(ifp, ofp, 3, "mJobPoint");
		print_int(ifp, ofp, 3, "mJobLevel");

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");

	int n2 = print_int(ifp, ofp, 1, "mCustomSkillNum");

	indent(ofp, 1);
	fprintf(ofp, "<array name=\"mpCustomSkill\">\n");

	for (int i = 0; i < n2; i++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class index=\"%d\">\n", i);

		print_int(ifp, ofp, 3, "mSkillId");
		print_int(ifp, ofp, 3, "mSkillLv");
		print_int(ifp, ofp, 3, "mJobPoint");
		print_int(ifp, ofp, 3, "mJobLevel");

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");

	int n3 = print_int(ifp, ofp, 2, "mAbilityNum");

	indent(ofp, 1);
	fprintf(ofp, "<array name=\"mpAbility\">\n");

	for (int i = 0; i < n3; i++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class index=\"%d\">\n", i);

		print_int(ifp, ofp, 3, "mAbilityId");
		print_int(ifp, ofp, 3, "mJobPoint");
		print_int(ifp, ofp, 3, "mJobLevel");

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");

	fprintf(ofp, "</skl>\n");
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

		if (skl2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
