#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

static const char *enemy_names[][2] = {
	{ "em0100", "Goblin" },
	{ "em0101", "Hobgoblin" },
	{ "em0102", "Grimgoblin" },
	{ "em0103", "Greater Goblin" },
	{ "em0200", "Wolf" },
	{ "em0201", "Direwolf" },
	{ "em0202", "Hellhound" },
	{ "em0203", "Warg" },
	{ "em0204", "Garm" },
	{ "em0400", "Saurian" },
	{ "em0401", "Sulfur Saurian" },
	{ "em0402", "Geo Saurian" },
	{ "em0403", "Saurian Sage" },
	{ "em0404", "Pyre Saurian" },
	{ "em0405", "Giant Saurian" },
	{ "em0406", "Giant Sulfur Saurian" },
	{ "em0407", "Giant Geo Saurian" },
	{ "em0408", "Giant Saurian Sage" },
	{ "em0500", "Undead Male" },
	{ "em0501", "Undead Female" },
	{ "em0502", "Stout Undead" },
	{ "em0503", "Undead Warrior" },
	{ "em0504", "Giant Undead" },
	{ "em0505", "Poison Undead" },
	{ "em0506", "Banshee" },
	{ "em0507", "Eliminator" },
	{ "em0600", "Harpy" },
	{ "em0601", "Snow Harpy" },
	{ "em0602", "Succubus" },
	{ "em0603", "Gargoyle" },
	{ "em0604", "Stigoi" },
	{ "em0605", "Siren" },
	{ "em0700", "Phantom" },
	{ "em0701", "Phantasm" },
	{ "em0702", "Specter" },
	{ "em0703", "Wraith" },
	{ "em0900", "Ogre" },
	{ "em0901", "Elder Ogre" },
	{ "em1000", "Bandit" },
	{ "em2000", "Skeleton" },
	{ "em2001", "Skeleton Knight" },
	{ "em2002", "Skeleton Lord" },
	{ "em2003", "Skeleton Warrior" },
	{ "em2004", "Skeleton Brute" },
	{ "em2005", "Gold Knight" },
	{ "em2006", "Silver Knight" },
	{ "em2007", "Living Armor" },
	{ "em2100", "Skeleton Mage" },
	{ "em2101", "Skeleton Sorcerer" },
	{ "em5000", "Cyclops" },
	{ "em5001", "Gorecyclops" },
	{ "em5100", "Golem" },
	{ "em5101", "Metal Golem" },
	{ "em5200", "Chimera" },
	{ "em5201", "Gorechimera" },
	{ "em5300", "Hydra" },
	{ "em5301", "Archydra" },
	{ "em5400", "Griffin" },
	{ "em5401", "Cockatrice" },
	{ "em5500", "Evil Eye" },
	{ "em5501", "Vile Eye" },
	{ "em5502", "Gazer" },
	{ "em5700", "Wight" },
	{ "em5800", "The Dragon" },
	{ "em5801", "Ur-Dragon" },
	{ "em5900", "Drake" },
	{ "em5901", "Wyrm" },
	{ "em5902", "Wyvern" },
	{ "em5903", "Firedrake" },
	{ "em5904", "Frostwyrm" },
	{ "em5905", "Thunderwyvern" },
	{ "em5906", "Cursed Dragon" },
	{ "em6000", "Wight" },
	{ "em6001", "Lich" },
	{ "em6002", "Dark Bishop" },
	{ "em6003", "Death" },
	{ "em7000", "Daimon" },
	{ NULL, NULL }
};

static const char *get_enemy_name(const char *string)
{
	int i = 0;

	while (enemy_names[i][0])
	{
		if (strcmp(enemy_names[i][0], string) == 0)
		{
			return enemy_names[i][1];
		}

		i++;
	}

	return NULL;
}

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
		if ((float)atof(str) == f)
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

static char *print_string(FILE *ifp, FILE *ofp, int ind, const char *name)
{
	char *str = read_string(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<string name=\"%s\" value=\"%s\"/>", name, str);
	const char *ename = get_enemy_name(str);
	if (ename) fprintf(ofp, "<!-- %s -->", ename);
	fputc('\n', ofp);
	return str;
}

static void print_array(FILE *ifp, FILE *ofp, int ind, char *name)
{
	int n = read_int(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<array name=\"%s\" count=\"%d\">\n", name, n);

	for (int i = 0; i < n; i++)
	{
		indent(ofp, ind + 1);
		fprintf(ofp, "<i16 index=\"%d\" value=\"%d\"/>\n", i, read_short(ifp));
	}

	indent(ofp, ind);
	fprintf(ofp, "</array>\n");
}

static void print_common(FILE *ifp, FILE *ofp, int ind)
{
	print_int   (ifp, ofp, ind, "mAreaHitNo");
	print_byte  (ifp, ofp, ind, "mIsSetHp");
	print_float (ifp, ofp, ind, "mHpRatio");
	print_byte  (ifp, ofp, ind, "mIsSetAtkPow");
	print_float (ifp, ofp, ind, "mAtkPowRatio");
	print_byte  (ifp, ofp, ind, "mIsSetDefence");
	print_float (ifp, ofp, ind, "mDefenceRatio");
	print_byte  (ifp, ofp, ind, "mIsSetMagicPow");
	print_float (ifp, ofp, ind, "mMagicPowRatio");
	print_byte  (ifp, ofp, ind, "mIsSetMagicDef");
	print_float (ifp, ofp, ind, "mMagicDefRatio");
	print_int   (ifp, ofp, ind, "mAIKnowledgeFlag");
	print_int   (ifp, ofp, ind, "mAIKind");
	print_byte  (ifp, ofp, ind, "mIsAINoTarget");
	print_byte  (ifp, ofp, ind, "mIsUseItemEveryone");
	print_byte  (ifp, ofp, ind, "mIsDragonBallDrop");

	print_int   (ifp, ofp, ind, "mSetID");
	print_string(ifp, ofp, ind, "mName");
	print_int   (ifp, ofp, ind, "mOrder");
	print_vector(ifp, ofp, ind, "mPosition");
	print_vector(ifp, ofp, ind, "mAngle");
	print_vector(ifp, ofp, ind, "mScale");
	print_float (ifp, ofp, ind, "mDrawDistance");
	print_byte  (ifp, ofp, ind, "mIsOnSplitAreaIgnore");
}

static void print_enemy(FILE *ifp, FILE *ofp, int ind)
{
	print_int   (ifp, ofp, ind, "mLifePointGroup");
	print_byte  (ifp, ofp, ind, "mUseFirstSetThinkTbl");
	print_int   (ifp, ofp, ind, "mStartTableNo");
	print_byte  (ifp, ofp, ind, "mBossFlag");
	print_byte  (ifp, ofp, ind, "mBgmBossFlag");
	print_int   (ifp, ofp, ind, "mEmItemFlag");
	print_int   (ifp, ofp, ind, "mEmItemTable");
	print_int   (ifp, ofp, ind, "mDieSet");
	print_byte  (ifp, ofp, ind, "mCorpseQuickDiscreate");
	print_byte  (ifp, ofp, ind, "mIsDieEraseTimeExtend");
	print_byte  (ifp, ofp, ind, "mRandomSetIgnore");
	print_int   (ifp, ofp, ind, "mExperienceOW");
	print_string(ifp, ofp, ind, "mFsmPath");
	print_byte  (ifp, ofp, ind, "\343\202\273\343\203\263\343\202\265\343\203\274\345\215\212\345\276\204\345\200\215\347\216\207\343\202\222\344\275\277\347\224\250\343\201\231\343\202\213");
	print_float (ifp, ofp, ind, "\343\202\273\343\203\263\343\202\265\343\203\274\345\215\212\345\276\204\345\200\215\347\216\207");

	print_common(ifp, ofp, ind);
}

static void print_target(FILE *ifp, FILE *ofp, int ind)
{
	if (!print_int(ifp, ofp, ind, "mpTarget"))
		return;

	char *str = read_string(ifp);
	indent(ofp, ind);
	fprintf(ofp, "<class type=\"%s\">\n", str);
	int in = ind + 1;

	if (strcmp(str, "cAISensorTargetStageAction") == 0)
	{
		print_int   (ifp, ofp, in, "mStageActionType");
		print_int   (ifp, ofp, in, "mPawnID");
		print_int   (ifp, ofp, in, "mOnOffKind");
		print_float (ifp, ofp, in, "mResetTime");
		print_int   (ifp, ofp, in, "mWeight");
		print_byte  (ifp, ofp, in, "CHK_TYPE");
		print_short (ifp, ofp, in, "ITEM_NO");
		print_byte  (ifp, ofp, in, "ITEM_NUM");
		print_array (ifp, ofp, in, "mActParamIdx");
		print_array (ifp, ofp, in, "mActParamQuestNo");
	}
	else if (strcmp(str, "cAISensorTargetGeneralPoint") == 0)
	{
		print_int   (ifp, ofp, in, "mGroup");
		print_int   (ifp, ofp, in, "mId");
		print_int   (ifp, ofp, in, "mActType");
		print_byte  (ifp, ofp, in, "mIsAllDay");
		print_int   (ifp, ofp, in, "mSTime");
		print_int   (ifp, ofp, in, "mETime");
		print_int   (ifp, ofp, in, "mShape");
		print_vector(ifp, ofp, in, "mSize");
	}
	else if (strcmp(str, "cAISensorTargetNpc") == 0)
	{
		print_byte  (ifp, ofp, in, "mEnable");
		print_int   (ifp, ofp, in, "mActType");
		print_int   (ifp, ofp, in, "mMotType");
		print_float (ifp, ofp, in, "mActFrame");
		print_byte  (ifp, ofp, in, "mNearFlag");
		print_byte  (ifp, ofp, in, "mRotFlag");
		print_byte  (ifp, ofp, in, "mIsAllDay");
		print_int   (ifp, ofp, in, "mStartTime");
		print_int   (ifp, ofp, in, "mEndTime");
		print_byte  (ifp, ofp, in, "mFix");
		print_int   (ifp, ofp, in, "mMessQuestNo");
		print_int   (ifp, ofp, in, "mMessId");
		print_float (ifp, ofp, in, "mMessDispSec");
		print_int   (ifp, ofp, in, "mNpcId");
	}

	print_int   (ifp, ofp, in, "mTypeBit");
	print_int   (ifp, ofp, in, "mSndLv");
	print_int   (ifp, ofp, in, "mStatusFlag");
	print_vector(ifp, ofp, in, "mPos");
	print_vector(ifp, ofp, in, "mDir");
	print_float (ifp, ofp, in, "mRange");
	print_int   (ifp, ofp, in, "mObjectID");
	print_int   (ifp, ofp, in, "mAttr");
	print_float (ifp, ofp, in, "mSphereRange");
	print_int   (ifp, ofp, in, "mJntNo");

	indent(ofp, ind);
	fprintf(ofp, "</class>\n");
}

static void print_npc(FILE *ifp, FILE *ofp, int ind)
{
	print_int   (ifp, ofp, ind, "mNpcPriority");
	print_string(ifp, ofp, ind, "mFSMPath");
	print_int   (ifp, ofp, ind, "mNpcId");
	print_byte  (ifp, ofp, ind, "mScrAdjustOff");
	print_byte  (ifp, ofp, ind, "mObjAdjustOff");
	print_int   (ifp, ofp, ind, "mPriority");
	print_byte  (ifp, ofp, ind, "mGoodsOff");
	print_int   (ifp, ofp, ind, "mSimpleModelType");
	print_int   (ifp, ofp, ind, "mPartsVariationNo");
	print_int   (ifp, ofp, ind, "mClothType");
	print_byte  (ifp, ofp, ind, "mUseMouseJoint");
	print_int   (ifp, ofp, ind, "mHumanEnemyKind");
	print_int   (ifp, ofp, ind, "mHumanEnemyID");
	print_int   (ifp, ofp, ind, "mRank");
	print_int   (ifp, ofp, ind, "mMultiNpcKind");
	print_byte  (ifp, ofp, ind, "mUse24Schedule");
	print_byte  (ifp, ofp, ind, "mIsHumanEnemyLeader");
	print_int   (ifp, ofp, ind, "mExperienceOW");

	print_target(ifp, ofp, ind);

	print_byte  (ifp, ofp, ind, "\345\244\226\351\203\250\346\214\207\345\256\232");
	print_int   (ifp, ofp, ind, "\345\276\205\346\251\237\350\241\214\345\213\225\343\202\277\343\202\244\343\203\227");
	print_int   (ifp, ofp, ind, "\345\206\205\351\203\250\343\203\225\343\203\251\343\202\260");
	print_int   (ifp, ofp, ind, "\345\276\205\346\251\237\350\241\214\345\213\225\347\225\252\345\217\267");
	print_float (ifp, ofp, ind, "\345\237\272\346\234\254\345\276\205\346\251\237\346\231\202\351\226\223");
	print_float (ifp, ofp, ind, "\343\203\251\343\203\263\343\203\200\343\203\240\345\276\205\346\251\237\346\231\202\351\226\223");
	print_int   (ifp, ofp, ind, "\343\201\227\343\201\220\343\201\225\350\241\214\345\213\225\347\225\252\345\217\267");
	print_float (ifp, ofp, ind, "\345\237\272\346\234\254\343\201\227\343\201\220\343\201\225\346\231\202\351\226\223");
	print_float (ifp, ofp, ind, "\343\203\251\343\203\263\343\203\200\343\203\240\343\201\227\343\201\220\343\201\225\346\231\202\351\226\223");

	print_common(ifp, ofp, ind);
}

static void print_placeable(FILE *ifp, FILE *ofp, int ind)
{
	print_short (ifp, ofp, ind, "mSetTableID");
	print_short (ifp, ofp, ind, "mSetTableIDNight");
	print_short (ifp, ofp, ind, "mSetItemNo");
	print_short (ifp, ofp, ind, "mSetItemNoNight");
	print_int   (ifp, ofp, ind, "mStartHour");
	print_int   (ifp, ofp, ind, "mEndHour");
	print_int   (ifp, ofp, ind, "mFree00");
	print_int   (ifp, ofp, ind, "mFree01");
	print_int   (ifp, ofp, ind, "mAngleType");
	print_int   (ifp, ofp, ind, "mFlag");

	print_common(ifp, ofp, ind);
}

static int lot2xml(FILE *ifp, char *file)
{
	if (read_int(ifp) != 7630700)
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
	fprintf(ofp, "<lot magic=\"0x746F6C\">\n");
	print_int(ifp, ofp, 1, "version");

	int count = read_int(ifp);
	indent(ofp, 1);
	fprintf(ofp, "<array count=\"%d\">\n", count);

	for (int i = 0; i < count; i++)
	{
		indent(ofp, 2);
		fprintf(ofp, "<class>\n");
		print_int(ifp, ofp, 3, "index");
		int type = print_int(ifp, ofp, 3, "type");

		switch (type)
		{
		case 3:
		case 21: // em0900
		case 29: // em5500
		case 31: // em5501
		case 36: // em8000
		case 37: // em8100
		case 38: // em8200
		case 39: // em8300
		case 40: // em8500
		case 41: // em8600
		case 42: // em8700
		case 43: // em8900
		case 44: // em9000
		case 68: // em9100
		case 71: // em7001
		case 73: // em5401
			print_enemy (ifp, ofp, 3);
			break;

		case 4: // em0100
			print_int   (ifp, ofp, 3, "mEquipType");
			print_byte  (ifp, ofp, 3, "mIsLeader");
			print_byte  (ifp, ofp, 3, "mIsWeaponHandStart");
			print_byte  (ifp, ofp, 3, "mIsBallistaLicense");
			print_enemy (ifp, ofp, 3);
			break;

		case 5: // em0101
		case 6: // em0102
			print_int   (ifp, ofp, 3, "mEquipType");
			print_byte  (ifp, ofp, 3, "mIsLeader");
			print_byte  (ifp, ofp, 3, "mIsWeaponHandStart");
			print_enemy (ifp, ofp, 3);
			break;

		case 7: // em0200
		case 8: // em0201
		case 9: // em0202
		case 56: // em0203
		case 57: // em0204
			print_int   (ifp, ofp, 3, "mWolfType");
			print_byte  (ifp, ofp, 3, "mIsRandamScale");
			print_float (ifp, ofp, 3, "mWolfScale");
			print_enemy (ifp, ofp, 3);
			break;

		case 10: // em0400
		case 11: // em0401
		case 12: // em0402
		case 13: // em0403
		case 58: // em0404
		case 59: // em0405
		case 60: // em0406
		case 61: // em0407
		case 62: // em0408
			print_byte  (ifp, ofp, 3, "mIsWallStart");
			print_byte  (ifp, ofp, 3, "mIsOptCamStart");
			print_enemy (ifp, ofp, 3);
			break;

		case 14: // em0500
		case 15: // em0501
		case 16: // em0502
		case 18: // em0504
		case 63: // em0505
		case 64: // em0506
		case 65: // em0507
		case 22: // em2000
			print_int   (ifp, ofp, 3, "mPartsVariationNo");
			print_enemy (ifp, ofp, 3);
			break;

		case 17: // em0503
			print_int   (ifp, ofp, 3, "mEquipType");
			print_int   (ifp, ofp, 3, "mPartsVariationNo");
			print_enemy (ifp, ofp, 3);
			break;

		case 19: // em0600
			print_byte  (ifp, ofp, 3, "mHoverMainMode");
			print_byte  (ifp, ofp, 3, "mbOmBreakCheck");
			print_int   (ifp, ofp, 3, "mOmBreakGroup");
			print_int   (ifp, ofp, 3, "mOmBreakID");
			print_byte  (ifp, ofp, 3, "mHoverActionStart");
			print_enemy (ifp, ofp, 3);
			break;

		case 20: // em0700
			print_int   (ifp, ofp, 3, "mPowerUpLv");
			print_enemy (ifp, ofp, 3);
			break;

		case 23: // em5000
			print_int   (ifp, ofp, 3, "mType");
			print_int   (ifp, ofp, 3, "mWeaponType");
			print_int   (ifp, ofp, 3, "mTuskType");
			print_byte  (ifp, ofp, 3, "mIsWest");
			print_byte  (ifp, ofp, 3, "mIsKakusya");
			print_int   (ifp, ofp, 3, "mKakusyaExp");
			print_float (ifp, ofp, 3, "mKakusyaRestHpRate");
			print_float (ifp, ofp, 3, "mKakusyaLegDP");
			print_float (ifp, ofp, 3, "mKakusyaLegBP");
			print_enemy (ifp, ofp, 3);
			break;

		case 24: // em5100
			print_int   (ifp, ofp, 3, "mInitMediumBreak");
			print_enemy (ifp, ofp, 3);
			break;

		case 25: // em5101
			print_int   (ifp, ofp, 3, "mGolemNo");
			print_enemy (ifp, ofp, 3);
			break;

		case 26: // em5101_00
			print_int   (ifp, ofp, 3, "mGolemNo");
			print_int   (ifp, ofp, 3, "mMediumRegionNo");
			print_enemy (ifp, ofp, 3);
			break;

		case 27: // em5200
			print_byte  (ifp, ofp, 3, "mIsReqDieCamera");
			print_enemy (ifp, ofp, 3);
			break;

		case 28: // em5300
			print_float (ifp, ofp, 3, "mStartWaitTime");
			print_enemy (ifp, ofp, 3);
			break;

		case 30: // em5500B
		case 70: // em5500C
			print_byte  (ifp, ofp, 3, "mTentacleMagicCanNum");
			print_int   (ifp, ofp, 3, "mTentacleWarpMaxNum");
			print_vector(ifp, ofp, 3, "mTentacleWarpExtent");
 			print_enemy (ifp, ofp, 3);
			break;

		case 32: // em5800
			print_int   (ifp, ofp, 3, "mSeqButtleSts");
			print_enemy (ifp, ofp, 3);
			break;

		case 33: // em5801
		case 67: // em7000
			print_byte  (ifp, ofp, 3, "mIsDieSet");
			print_enemy (ifp, ofp, 3);
			break;

		case 34: // em5900
			print_int   (ifp, ofp, 3, "mSetType");
			print_byte  (ifp, ofp, 3, "mNonFlySet");
			print_byte  (ifp, ofp, 3, "mOriginalTalk");
			print_byte  (ifp, ofp, 3, "\343\203\252\343\203\203\343\203\201\343\201\262\343\202\207\343\201\206\343\201\204");
			print_byte  (ifp, ofp, 3, "\346\227\213\345\233\236\351\243\233\350\241\214\343\201\227\343\201\252\343\201\204");
			print_enemy (ifp, ofp, 3);
			break;

		case 35: // em6000
			print_byte  (ifp, ofp, 3, "\346\255\273\347\245\236\347\211\271\345\210\245\350\250\255\347\275\256\343\201\213\357\274\237");
			print_float (ifp, ofp, 3, "\346\255\273\347\245\236\347\211\271\345\210\245\350\250\255\347\275\256\343\203\225\343\203\254\343\203\274\343\203\240");
			print_enemy (ifp, ofp, 3);
			break;

		case 45: // em9807
			print_float (ifp, ofp, 3, "mLifeTime");
			print_byte  (ifp, ofp, 3, "mIsSoundOff");
			print_enemy (ifp, ofp, 3);
			break;

		case 66: // em5001
			print_int   (ifp, ofp, 3, "mType");
			print_byte  (ifp, ofp, 3, "mIsWeaponInitRemove");
			print_vector(ifp, ofp, 3, "mWeaponInitPos");
			print_vector(ifp, ofp, 3, "mWeaponInitAngle");
			print_enemy (ifp, ofp, 3);
			break;

		case 69: // em0103
			print_int   (ifp, ofp, 3, "mEquipType");
			print_byte  (ifp, ofp, 3, "mIsLeader");
			print_byte  (ifp, ofp, 3, "mIsWeaponHandStart");
			print_byte  (ifp, ofp, 3, "mIsBallistaLicense");
			print_int   (ifp, ofp, 3, "mType");
			print_enemy (ifp, ofp, 3);
			break;

		case 47:
			print_npc(ifp, ofp, 3);
			break;

		case 48:
			print_placeable(ifp, ofp, 3);
			break;

		case 49:
			print_string(ifp, ofp, 3, "mFSMPath");
			print_placeable(ifp, ofp, 3);
			break;

		case 50:
			print_short (ifp, ofp, 3, "mSeGroupId");
			print_short (ifp, ofp, 3, "mSePriority");
			print_short (ifp, ofp, 3, "mMapIconType");
			print_string(ifp, ofp, 3, "mFSMPath");
			print_placeable(ifp, ofp, 3);
			break;

		case 51:
			print_float (ifp, ofp, 3, "mCheckAngle");
			print_float (ifp, ofp, 3, "mCheckRange");
			print_placeable(ifp, ofp, 3);
			break;

		case 52:
			print_short (ifp, ofp, 3, "mSeGroupId");
			print_short (ifp, ofp, 3, "mSePriority");
			print_short (ifp, ofp, 3, "mMapIconType");
			print_placeable(ifp, ofp, 3);
			break;

		case 53:
			print_short (ifp, ofp, 3, "mLockMsgNo");
			print_short (ifp, ofp, 3, "mOutMsgNo");
			print_short (ifp, ofp, 3, "mOpenMsgNo");
			print_float (ifp, ofp, 3, "mCheckAngle");
			print_float (ifp, ofp, 3, "mCheckRange");
			print_short (ifp, ofp, 3, "mSeGroupId");
			print_short (ifp, ofp, 3, "mSePriority");
			print_short (ifp, ofp, 3, "mMapIconType");
			print_placeable(ifp, ofp, 3);
			break;

		case 46:
			print_int   (ifp, ofp, 3, "mLightGroup");
			print_byte  (ifp, ofp, 3, "mOverwriteLightGroup");
			print_int   (ifp, ofp, 3, "mTransMode");
			print_byte  (ifp, ofp, 3, "mOverwriteTransMode");
			print_common(ifp, ofp, 3);
			break;

		case 54:
			print_target(ifp, ofp, 3);
			break;

		default:
			printf("Unsupported type %d in LOT file %s\n", type, file);
			fclose(ofp);
			return 1;
		}

		indent(ofp, 2);
		fprintf(ofp, "</class>\n");
	}

	indent(ofp, 1);
	fprintf(ofp, "</array>\n");
	fprintf(ofp, "</lot>\n");
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

		if (lot2xml(fp, argv[i]))
		{
			ret = 1;
		}

		fclose(fp);
	}

	return ret;
}
