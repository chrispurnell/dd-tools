#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <inttypes.h>

static int read_short(FILE *fp)
{
	int16_t i = 0;
	fread(&i, 2, 1, fp);
	return i;
}

static int read_int(FILE *fp)
{
	int32_t i = 0;
	fread(&i, 4, 1, fp);
	return i;
}

static int64_t read_long(FILE *fp)
{
	int64_t i = 0;
	fread(&i, 8, 1, fp);
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

static int print_long(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	int64_t i = read_long(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<i64 name=\"%s\" value=\"%" PRId64 "\"/>\n", name, i);
	return i;
}

static void print_byte_array(FILE *ifp, FILE *ofp, int ind, const char *name, int count)
{
	indent(ofp, ind);
	fprintf(ofp, "<array name=\"%s\">\n", name);
	int in = ind + 1;

	for (int i = 0; i < count; i++)
	{
		char c = fgetc(ifp);
		indent(ofp, in);
		fprintf(ofp, "<i8 index=\"%d\" value=\"%d\"/>\n", i, c);
	}

	indent(ofp, ind);
	fprintf(ofp, "</array>\n");
}

static void print_int_array(FILE *ifp, FILE *ofp, int ind, const char *name, int count)
{
	indent(ofp, ind);
	fprintf(ofp, "<array name=\"%s\">\n", name);
	int in = ind + 1;

	for (int i = 0; i < count; i++)
	{
		indent(ofp, in);
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

static void print_flag_array(FILE *ifp, FILE *ofp, int ind, const char *name, int count)
{
	indent(ofp, ind);
	fprintf(ofp, "<array name=\"%s\">\n", name);

	int in = ind + 1;

	for (int i = 0; i < count; i++)
	{
		indent(ofp, in);
		fprintf(ofp, "<class>\n");

		print_byte(ifp, ofp, in + 1, "FlagType");
		print_byte(ifp, ofp, in + 1, "QuestNo");
		print_short(ifp, ofp, in + 1, "FlagNo");

		indent(ofp, in);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, ind);
	fprintf(ofp, "</array>\n");
}

static int eap2xml(FILE *ifp, char *file)
{
	if (read_int(ifp) != 1599095109)
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
	fprintf(ofp, "<eap magic=\"0x5F504145\">\n");
	print_short(ifp, ofp, 1, "version");
	int num = print_short(ifp, ofp, 1, "paramNum");
	int snum = print_short(ifp, ofp, 1, "studyDisableNum");
	print_byte_array(ifp, ofp, 1, "unitTargetNum", 30);

	indent(ofp, 1);
	fprintf(ofp, "<array name=\"param\">\n");
	for (int i = 0; i < num; i++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class>\n");

		print_int(ifp, ofp, 3, "ActionStatus");
		print_int(ifp, ofp, 3, "BaseStatus");
		print_long(ifp, ofp, 3, "StatusBadGood");
		print_long(ifp, ofp, 3, "NotStatusBadGood");
		print_int(ifp, ofp, 3, "EtcStatus");
		print_int(ifp, ofp, 3, "PositionBit");
		print_int(ifp, ofp, 3, "PositionID");
		print_int(ifp, ofp, 3, "CmcAction");
		print_int(ifp, ofp, 3, "CharacterInfoType");
		print_int(ifp, ofp, 3, "NotCharacterInfoType");
		print_int(ifp, ofp, 3, "CmcOrderTag");

		indent(ofp, 3);
		fprintf(ofp, "<class name=\"ActionParam\">\n");

		print_int_array(ifp, ofp, 4, "StudyID", 8);
		print_int_array(ifp, ofp, 4, "NoStudyID", 8);
		print_int(ifp, ofp, 4, "Goal");
		print_int(ifp, ofp, 4, "Weight");
		print_int(ifp, ofp, 4, "Rate");
		print_int(ifp, ofp, 4, "ElementAttr");
		print_int(ifp, ofp, 4, "NoElementAttr");
		print_int(ifp, ofp, 4, "AtkAttr");
		print_int(ifp, ofp, 4, "NoAtkAttr");
		print_int(ifp, ofp, 4, "UseAttr");
		print_int(ifp, ofp, 4, "NoUseAttr");
		print_int(ifp, ofp, 4, "Angle");
		print_int(ifp, ofp, 4, "CameraID");
		print_int(ifp, ofp, 4, "MessageID");
		print_int(ifp, ofp, 4, "MessageType");
		print_int(ifp, ofp, 4, "GetStudyID");
		print_int(ifp, ofp, 4, "StaminaCheckType");
		print_int(ifp, ofp, 4, "JobBit");
		print_int(ifp, ofp, 4, "ParamFlag");
		print_int_array(ifp, ofp, 4, "FreeS32", 4);
		print_float_array(ifp, ofp, 4, "FreeF32", 4);
		print_flag_array(ifp, ofp, 4, "SetOnFlag", 2);
		print_flag_array(ifp, ofp, 4, "SetffFlag", 2);

		indent(ofp, 3);
		fprintf(ofp, "</class>\n");

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}
	indent(ofp, 1);
	fprintf(ofp, "</array>\n");

	indent(ofp, 1);
	fprintf(ofp, "<array name=\"mpStudyDisableAttrAdrs\">\n");
	for (int i = 0; i < snum; i++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class>\n");

		print_int(ifp, ofp, 3, "ActionStatus");
		print_int(ifp, ofp, 3, "BaseStatus");
		print_int(ifp, ofp, 3, "StudyLvMin");
		print_int(ifp, ofp, 3, "StudyLvMax");
		print_int(ifp, ofp, 3, "DisableElement");
		print_int(ifp, ofp, 3, "DisableAtkAttr");
		print_int(ifp, ofp, 3, "DisableUse");

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}
	indent(ofp, 1);
	fprintf(ofp, "</array>\n");

	fprintf(ofp, "</eap>\n");
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

		if (eap2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
