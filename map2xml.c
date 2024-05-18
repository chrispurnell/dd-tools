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

static int print_byte(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	char i = fgetc(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<i8 name=\"%s\" value=\"%d\"/>\n", name, i);
	return i;
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

static void print_param(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	indent(ofp, ind);
	fprintf(ofp, "<class name=\"%s\">\n", name);
	int in = ind + 1;
	print_int(ifp, ofp, in, "shlGroup");
	print_int(ifp, ofp, in, "shlNo");
	print_int(ifp, ofp, in, "setType");
	print_int(ifp, ofp, in, "shotJointNo");
	print_vector(ifp, ofp, in, "shotOffset");
	print_vector(ifp, ofp, in, "shlScale");
	print_int(ifp, ofp, in, "shlShotNum");
	print_float(ifp, ofp, in, "shlSetWait");
	indent(ofp, ind);
	fprintf(ofp, "</class>\n");
}


static int map2xml(FILE *ifp, char *file)
{
	if (read_int(ifp) != 7364973)
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
	fprintf(ofp, "<map magic=\"0x70616d\">\n");

	print_int(ifp, ofp, 1, "version");

	int n1 = read_int(ifp);
	indent(ofp, 1);
	fprintf(ofp, "<array name=\"mpArray\" count=\"%d\">\n", n1);

	for (int i1 = 0; i1 < n1; i1++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class index=\"%d\">\n", i1);

		print_int(ifp, ofp, 3, "efctType");
		print_int(ifp, ofp, 3, "manualType");
		print_int(ifp, ofp, 3, "seType");
		print_int(ifp, ofp, 3, "motNoChantBgn");
		print_float(ifp, ofp, 3, "blendChatBgn");
		print_int(ifp, ofp, 3, "motNoChantLoop");
		print_float(ifp, ofp, 3, "blendChantLoop");
		print_int(ifp, ofp, 3, "motNoShot");
		print_float(ifp, ofp, 3, "blendShot");
		print_int(ifp, ofp, 3, "motNoEnd");
		print_float(ifp, ofp, 3, "blendEnd");
		print_byte(ifp, ofp, 3, "is4Move");
		print_int(ifp, ofp, 3, "mot4MoveWait");
		print_int(ifp, ofp, 3, "mot4MoveWaitBgnFrame");
		print_float(ifp, ofp, 3, "mot4MoveBlend");
		print_int(ifp, ofp, 3, "motShlCtrlPlWait");
		print_int(ifp, ofp, 3, "motShlCtrlPlWaitBgnFrame");
		print_int(ifp, ofp, 3, "motShlCtrlPlMove");
		print_int(ifp, ofp, 3, "motShlCtrlPlBlend");
		print_int(ifp, ofp, 3, "motShlCtrlPlShot");
		print_int(ifp, ofp, 3, "motShlCtrlPlShotBlend");
		print_int(ifp, ofp, 3, "motNoShlHit");
		print_float(ifp, ofp, 3, "motBlendShlHit");
		print_int(ifp, ofp, 3, "motNoShlHitEnd");
		print_float(ifp, ofp, 3, "motBlendShlHitEnd");
		print_int(ifp, ofp, 3, "motNoShlHitEnd2");
		print_float(ifp, ofp, 3, "motBlendShlHitEnd2");
		print_int(ifp, ofp, 3, "motNoEx");
		print_float(ifp, ofp, 3, "motBlendEx");
		print_int(ifp, ofp, 3, "optionFlag");
		print_float(ifp, ofp, 3, "chantLoopFrame");
		print_float(ifp, ofp, 3, "shotLoopFrame");
		print_int(ifp, ofp, 3, "chantBallJoint");
		print_vector(ifp, ofp, 3, "chantBallOffset");

		int n2 = read_int(ifp);
		indent(ofp, 3);
		fprintf(ofp, "<array name=\"lvParams\" count=\"%d\">\n", n2);

		for (int i2 = 0; i2 < n2; i2++)
		{
			indent(ofp, 4);
			fprintf(ofp, "<class index=\"%d\">\n", i2);

			print_float(ifp, ofp, 5, "chantFrame");
			print_float(ifp, ofp, 5, "stamina");
			print_float(ifp, ofp, 5, "staminaLoop");
			print_float(ifp, ofp, 5, "motSpeed");
			print_int(ifp, ofp, 5, "lockOnNum");
			print_float(ifp, ofp, 5, "lockOnBgnDist");
			print_float(ifp, ofp, 5, "lockOnEndDist");
			print_int(ifp, ofp, 5, "multiSetType");
			print_float(ifp, ofp, 5, "multiLockShlWait");
			print_int(ifp, ofp, 5, "multiMotLoopFrame");
			print_int(ifp, ofp, 5, "multiMotLoopBlend");
			print_int(ifp, ofp, 5, "cameraNo");
			print_param(ifp, ofp, 5, "shl0");
			print_param(ifp, ofp, 5, "shl1");
			print_param(ifp, ofp, 5, "shl2");

			indent(ofp, 4);
			fprintf(ofp, "</class>\n");
		}
		indent(ofp, 3);
		fprintf(ofp, "</array>\n");

		print_int(ifp, ofp, 3, "exeType");
		print_int(ifp, ofp, 3, "shlCtrlType");
		print_int(ifp, ofp, 3, "lockOnFlag");
		print_int(ifp, ofp, 3, "efPosType");
		print_int(ifp, ofp, 3, "shotLoopEndFlag");
		print_int(ifp, ofp, 3, "cameraType");
		print_int(ifp, ofp, 3, "cameraLookType");
		print_int(ifp, ofp, 3, "cameraNoTps");
		print_float(ifp, ofp, 3, "chantCmpCantShotFrame");
		print_float(ifp, ofp, 3, "lvUpCantShotFrame");
		print_int(ifp, ofp, 3, "noticeType");
		print_float(ifp, ofp, 3, "noticeRadius");

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");

	fprintf(ofp, "</map>\n");
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

		if (map2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
