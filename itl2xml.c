#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "item_names.h"

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

static void print_bits(FILE *ofp, int ind, int bits, const char *name, int value)
{
	int i = value & ((1 << bits) - 1);
	indent(ofp, ind);
	fprintf(ofp, "<b%u name=\"%s\" value=\"%d\"/>\n", bits, name, i);
}

static int print_item(FILE *ofp, int ind, int bits, const char *name, int value)
{
	int s = (32 - bits);
	unsigned i = (value << s) >> s;
	indent(ofp, ind);
	fprintf(ofp, "<b%u name=\"%s\" value=\"%d\"/>", bits, name, i);
	if (i < 1901) fprintf(ofp, "<!-- %s -->", item_names[i]);
	fputc('\n', ofp);
	return i;
}

static int itl2xml(FILE *ifp, char *file)
{
	if (read_int(ifp) != 843863113)
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
	fprintf(ofp, "<itl magic=\"0x324C5449\">\n");
	print_int(ifp, ofp, 1, "version");
	int count = print_int(ifp, ofp, 1, "count");
	print_int(ifp, ofp, 1, "pad");
	indent(ofp, 1);
	fprintf(ofp, "<array>\n");

	for (int i = 0; i < count; i++)
	{
		int n;

		indent(ofp, 2);
		fprintf(ofp, "<class>\n");

		indent(ofp, 3);
		fprintf(ofp, "<i32>\n");
		n = read_int(ifp);
		print_bits(ofp, 4, 11, "mAttack", n);
		print_bits(ofp, 4, 11, "mMagicAttack", n >> 11);
		print_bits(ofp, 4, 10, "mElementAttack", n >> 22);
		indent(ofp, 3);
		fprintf(ofp, "</i32>\n");

		indent(ofp, 3);
		fprintf(ofp, "<i32>\n");
		n = read_int(ifp);
		print_bits(ofp, 4, 10, "mDefense", n);
		print_bits(ofp, 4, 10, "mMagicDefense", n >> 10);
		print_bits(ofp, 4, 7, "mCritialRate", n >> 20);
		indent(ofp, 3);
		fprintf(ofp, "</i32>\n");

		indent(ofp, 3);
		fprintf(ofp, "<i32>\n");
		n = read_int(ifp);
		print_bits(ofp, 4, 10, "mShrink", n);
		print_bits(ofp, 4, 10, "mBlow", n >> 10);
		print_bits(ofp, 4, 7, "mShieldStaminaReduceRate", n >> 20);
		indent(ofp, 3);
		fprintf(ofp, "</i32>\n");

		indent(ofp, 3);
		fprintf(ofp, "<i32>\n");
		n = read_int(ifp);
		print_bits(ofp, 4, 10, "mNokeGuard", n);
		print_bits(ofp, 4, 10, "mBlowGuard", n >> 10);
		print_bits(ofp, 4, 10, "mPoison", n >> 20);
		indent(ofp, 3);
		fprintf(ofp, "</i32>\n");

		indent(ofp, 3);
		fprintf(ofp, "<i32>\n");
		n = read_int(ifp);
		print_bits(ofp, 4, 10, "mSlow", n);
		print_bits(ofp, 4, 10, "mOil", n >> 10);
		print_bits(ofp, 4, 10, "mBlind", n >> 20);
		print_bits(ofp, 4, 1, "unk1", n >> 30);
		indent(ofp, 3);
		fprintf(ofp, "</i32>\n");

		print_byte(ifp, ofp, 3, "mSwordAttackRate");
		print_byte(ifp, ofp, 3, "mHitAttackRate");
		print_byte(ifp, ofp, 3, "mSwordDefenseRate");
		print_byte(ifp, ofp, 3, "mHitDefenseRate");

		print_byte(ifp, ofp, 3, "mFireDefenseRate");
		print_byte(ifp, ofp, 3, "mIceDefenseRate");
		print_byte(ifp, ofp, 3, "mThunderDefenseRate");
		print_byte(ifp, ofp, 3, "mSaintDefenseRate");

		print_byte(ifp, ofp, 3, "mDarkDefenseRate");
		print_byte(ifp, ofp, 3, "mFireCut");
		print_byte(ifp, ofp, 3, "mIceCut");
		print_byte(ifp, ofp, 3, "mThunderCut");

		print_byte(ifp, ofp, 3, "mSaintCut");
		print_byte(ifp, ofp, 3, "mDarkCut");
		print_byte(ifp, ofp, 3, "mBlowDefenseRate");
		print_byte(ifp, ofp, 3, "mShrinkDefenseRate");

		print_byte(ifp, ofp, 3, "pad1");
		print_byte(ifp, ofp, 3, "mPoisonCut");
		print_byte(ifp, ofp, 3, "mSlowCut");
		print_byte(ifp, ofp, 3, "mBlindCut");

		print_byte(ifp, ofp, 3, "mSleepCut");
		print_byte(ifp, ofp, 3, "mWetCut");
		print_byte(ifp, ofp, 3, "mOilCut");
		print_byte(ifp, ofp, 3, "mEnemyCut");

		print_byte(ifp, ofp, 3, "mSilenceCut");
		print_byte(ifp, ofp, 3, "mSealCut");
		print_byte(ifp, ofp, 3, "mCurseCut");
		print_byte(ifp, ofp, 3, "mStoneCut");

		print_byte(ifp, ofp, 3, "mAttackDownCut");
		print_byte(ifp, ofp, 3, "mDefenseDownCut");
		print_byte(ifp, ofp, 3, "mMagicAttackDownCut");
		print_byte(ifp, ofp, 3, "mMagicDefenseDownCut");

		indent(ofp, 3);
		n = read_int(ifp);
		fprintf(ofp, "<i32>\n");
		print_bits(ofp, 4, 16, "mLevelUpType", n);
		print_bits(ofp, 4, 12, "mEnableEquipJob", n >> 16);
		print_bits(ofp, 4, 4, "mEquipKind", n >> 28);
		indent(ofp, 3);
		fprintf(ofp, "</i32>\n");

		indent(ofp, 3);
		fprintf(ofp, "<i32>\n");
		n = read_int(ifp);
		print_bits(ofp, 4, 5, "mKind", n);
		print_bits(ofp, 4, 5, "mCategoryType", n >> 5);
		print_bits(ofp, 4, 2, "mCategoryKind", n >> 10);
		print_bits(ofp, 4, 3, "mUseType", n >> 12);
		print_bits(ofp, 4, 4, "mUseMot", n >> 15);
		print_bits(ofp, 4, 3, "mElementType", n >> 19);
		print_bits(ofp, 4, 10, "mSilence", n >> 22);
		indent(ofp, 3);
		fprintf(ofp, "</i32>\n");

		indent(ofp, 3);
		fprintf(ofp, "<i32>\n");
		n = read_int(ifp);
		print_item(ofp, 4, 13, "mItemNo", n);
		print_item(ofp, 4, 13, "mAlterItemNo", n >> 13);
		indent(ofp, 3);
		fprintf(ofp, "</i32>\n");

		indent(ofp, 3);
		fprintf(ofp, "<i32 name=\"mEquipModelNo\">\n");
		n = read_int(ifp);
		indent(ofp, 4);
		fprintf(ofp, "<b24 value=\"%d\"/>\n", (n << 8) >> 8);
		indent(ofp, 4);
		fprintf(ofp, "<b8 value=\"%d\"/>\n", n >> 24);
		indent(ofp, 3);
		fprintf(ofp, "</i32>\n");

		print_float(ifp, ofp, 3, "mWeight");
		print_int(ifp, ofp, 3, "mBuy");
		print_int(ifp, ofp, 3, "mSell");
		print_int(ifp, ofp, 3, "mPri");
		print_float(ifp, ofp, 3, "mHp");

		indent(ofp, 3);
		fprintf(ofp, "<i32>\n");
		n = read_int(ifp);
		print_bits(ofp, 4, 1, "mEnemy01BigDamage", n);
		print_bits(ofp, 4, 1, "mEnemy02BigDamage", n >> 1);
		print_bits(ofp, 4, 1, "mEnemy03BigDamage", n >> 2);
		print_bits(ofp, 4, 1, "mEnemy04BigDamage", n >> 3);
		print_bits(ofp, 4, 1, "mEnemy05BigDamage", n >> 4);
		print_bits(ofp, 4, 1, "mEnemy06BigDamage", n >> 5);
		print_bits(ofp, 4, 1, "mEnemy07BigDamage", n >> 6);
		print_bits(ofp, 4, 1, "mEnemy08BigDamage", n >> 7);
		print_bits(ofp, 4, 1, "mEnemy09BigDamage", n >> 8);
		print_bits(ofp, 4, 1, "mEnemy10BigDamage", n >> 9);
		print_bits(ofp, 4, 1, "mEnemy11BigDamage", n >> 10);
		print_bits(ofp, 4, 1, "mEnemy12BigDamage", n >> 11);
		print_bits(ofp, 4, 1, "mEnemy13BigDamage", n >> 12);
		print_bits(ofp, 4, 1, "mArrow", n >> 13);
		print_bits(ofp, 4, 1, "mMix", n >> 14);
		print_bits(ofp, 4, 1, "mVisor", n >> 15);
		print_bits(ofp, 4, 1, "mEnableThrow", n >> 16);
		print_bits(ofp, 4, 1, "mFake", n >> 17);
		print_bits(ofp, 4, 1, "mNoEffect", n >> 18);
		print_bits(ofp, 4, 1, "mRimShop", n >> 19);
		print_bits(ofp, 4, 1, "mEquipItem", n >> 20);
		print_bits(ofp, 4, 1, "mLantern", n >> 21);
		print_bits(ofp, 4, 1, "mInfinitUse", n >> 22);
		print_bits(ofp, 4, 1, "mHoldActionItem", n >> 23);
		print_bits(ofp, 4, 1, "mEquipActionItem", n >> 24);
		print_bits(ofp, 4, 1, "mGold", n >> 25);
		print_bits(ofp, 4, 1, "mRimPoint", n >> 26);
		print_bits(ofp, 4, 1, "mUnused", n >> 27);
		print_bits(ofp, 4, 1, "mKusariItem", n >> 28);
		print_bits(ofp, 4, 1, "unk2", n >> 29);
		print_bits(ofp, 4, 1, "unk3", n >> 30);
		print_bits(ofp, 4, 1, "unk4", n >> 31);
		indent(ofp, 3);
		fprintf(ofp, "</i32>\n");

		print_int(ifp, ofp, 3, "mSeType");

		print_short(ifp, ofp, 3, "mLife");
		print_short(ifp, ofp, 3, "mFriendPoint");

		print_short(ifp, ofp, 3, "mFrindCategory");
		print_short(ifp, ofp, 3, "mObtainingLv");

		print_byte(ifp, ofp, 3, "mStsAttackUp");
		print_byte(ifp, ofp, 3, "mStsMagicAttackUp");
		print_byte(ifp, ofp, 3, "mStsDefenseUp");
		print_byte(ifp, ofp, 3, "mStsMagicDefenseUp");

		print_byte(ifp, ofp, 3, "mStsFortune");
		print_byte(ifp, ofp, 3, "mStsEconomicFortune");
		print_byte(ifp, ofp, 3, "mTolerantDefense");
		print_byte(ifp, ofp, 3, "pad2");

		print_short(ifp, ofp, 3, "mAddHp");
		print_short(ifp, ofp, 3, "mAddAp");

		print_float(ifp, ofp, 3, "mUpHp");
		print_float(ifp, ofp, 3, "mUpAp"); 

		print_short(ifp, ofp, 3, "mOmId");
		print_byte(ifp, ofp, 3, "mOmColor");
		print_byte(ifp, ofp, 3, "pad3");

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");
	fprintf(ofp, "</itl>\n");
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

		if (itl2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
