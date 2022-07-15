#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

static const char *enemy_names[][2] = {
	{ "em0100", " (Goblin)" },
	{ "em0101", " (Hobgoblin)" },
	{ "em0102", " (Grimgoblin)" },
	{ "em0103", " (Greater Goblin)" },
	{ "em0200", " (Wolf)" },
	{ "em0201", " (Direwolf)" },
	{ "em0202", " (Hellhound)" },
	{ "em0203", " (Warg)" },
	{ "em0204", " (Garm)" },
	{ "em0400", " (Saurian)" },
	{ "em0401", " (Sulfur Saurian)" },
	{ "em0402", " (Geo Saurian)" },
	{ "em0403", " (Saurian Sage)" },
	{ "em0404", " (Pyre Saurian)" },
	{ "em0405", " (Giant Saurian)" },
	{ "em0406", " (Giant Sulfur Saurian)" },
	{ "em0407", " (Giant Geo Saurian)" },
	{ "em0408", " (Giant Saurian Sage)" },
	{ "em0500", " (Undead Male)" },
	{ "em0501", " (Undead Female)" },
	{ "em0502", " (Stout Undead)" },
	{ "em0503", " (Undead Warrior)" },
	{ "em0504", " (Giant Undead)" },
	{ "em0505", " (Poison Undead)" },
	{ "em0506", " (Banshee)" },
	{ "em0507", " (Eliminator)" },
	{ "em0600", " (Harpy)" },
	{ "em0601", " (Snow Harpy)" },
	{ "em0602", " (Succubus)" },
	{ "em0603", " (Gargoyle)" },
	{ "em0604", " (Stigoi)" },
	{ "em0605", " (Siren)" },
	{ "em0700", " (Phantom)" },
	{ "em0701", " (Phantasm)" },
	{ "em0702", " (Specter)" },
	{ "em0703", " (Wraith)" },
	{ "em0900", " (Ogre)" },
	{ "em0901", " (Elder Ogre)" },
	{ "em1000", " (Bandit)" },
	{ "em2000", " (Skeleton)" },
	{ "em2001", " (Skeleton Knight)" },
	{ "em2002", " (Skeleton Lord)" },
	{ "em2003", " (Skeleton Warrior)" },
	{ "em2004", " (Skeleton Brute)" },
	{ "em2005", " (Gold Knight)" },
	{ "em2006", " (Silver Knight)" },
	{ "em2007", " (Living Armor)" },
	{ "em2100", " (Skeleton Mage)" },
	{ "em2101", " (Skeleton Sorcerer)" },
	{ "em5000", " (Cyclops)" },
	{ "em5001", " (Gorecyclops)" },
	{ "em5100", " (Golem)" },
	{ "em5101", " (Metal Golem)" },
	{ "em5200", " (Chimera)" },
	{ "em5201", " (Gorechimera)" },
	{ "em5300", " (Hydra)" },
	{ "em5301", " (Archydra)" },
	{ "em5400", " (Griffin)" },
	{ "em5401", " (Cockatrice)" },
	{ "em5500", " (Evil Eye)" },
	{ "em5501", " (Vile Eye)" },
	{ "em5502", " (Gazer)" },
	{ "em5700", " (Wight)" },
	{ "em5800", " (The Dragon)" },
	{ "em5801", " (Ur-Dragon)" },
	{ "em5900", " (Drake)" },
	{ "em5901", " (Wyrm)" },
	{ "em5902", " (Wyvern)" },
	{ "em5903", " (Firedrake)" },
	{ "em5904", " (Frostwyrm)" },
	{ "em5905", " (Thunderwyvern)" },
	{ "em5906", " (Cursed Dragon)" },
	{ "em6000", " (Wight)" },
	{ "em6001", " (Lich)" },
	{ "em6002", " (Dark Bishop)" },
	{ "em6003", " (Death)" },
	{ "em7000", " (Daimon)" },
	{ NULL, "" }
};

static const char *get_enemy_name(const char *string)
{
	unsigned i = 0;

	while (enemy_names[i][0])
	{
		if (strcmp(enemy_names[i][0], string) == 0)
		{
			return enemy_names[i][1];
		}

		i++;
	}

	return "";
}

static int print_byte(FILE *fp, char *data, unsigned *pos, char *name)
{
	char i = data[*pos];
	*pos += 1;
	fprintf(fp, "%s:i8 = %d\n", name, i);
	return i;
}

static int print_short(FILE *fp, char *data, unsigned *pos, char *name)
{
	short i;
	memcpy(&i, data + *pos, 2);
	*pos += 2;
	fprintf(fp, "%s:i16 = %d\n", name, i);
	return i;
}

static int print_int(FILE *fp, char *data, unsigned *pos, char *name)
{
	int i;
	memcpy(&i, data + *pos, 4);
	*pos += 4;
	fprintf(fp, "%s:i32 = %d\n", name, i);
	return i;
}

static float print_float(FILE *fp, char *data, unsigned *pos, char *name)
{
	char str[64];
	float f;
	memcpy(&f, data + *pos, 4);
	*pos += 4;

	for (unsigned i = 1; i < 42; i++)
	{
		snprintf(str, 64, "%.*f", i, f);
		if ((float)atof(str) == f)
			break;
	}

	fprintf(fp, "%s:f32 = %s\n", name, str);
	return f;
}

static char *print_string(FILE *fp, char *data, unsigned *pos, char *name)
{
	char *s = data + *pos;
	*pos += strlen(s) + 1;
	fprintf(fp, "%s:str = %s\n", name, s);
	return s;
}

static char *print_model(FILE *fp, char *data, unsigned *pos, char *name)
{
	char *s = data + *pos;
	*pos += strlen(s) + 1;
	fprintf(fp, "%s:str = %s%s\n", name, s, get_enemy_name(s));
	return s;
}

static int print_index(FILE *fp, char *data, unsigned *pos)
{
	int i;
	memcpy(&i, data + *pos, 4);
	*pos += 4;
	fprintf(fp, "\n[ENTRY%04u]\n", i);
	return i;
}

static void print_array(FILE *fp, char *data, unsigned *pos, char *name, unsigned n)
{
	for (unsigned i = 0; i < n; i++)
	{
		short v;
		memcpy(&v, data + *pos, 2);
		*pos += 2;
		fprintf(fp, "%s[%u]:i16 = %d\n", name, i, v);
	}
}

static void print_common(FILE *fp, char *data, unsigned *pos)
{
	print_byte  (fp, data, pos, "u01");
	print_float (fp, data, pos, "u02");
	print_byte  (fp, data, pos, "u03");
	print_float (fp, data, pos, "u04");
	print_byte  (fp, data, pos, "u05");
	print_float (fp, data, pos, "u06");
	print_byte  (fp, data, pos, "u07");
	print_float (fp, data, pos, "u08");
	print_byte  (fp, data, pos, "u09");
	print_float (fp, data, pos, "u10");

	print_int   (fp, data, pos, "u11");
	print_int   (fp, data, pos, "u12");
	print_byte  (fp, data, pos, "u11");
	print_byte  (fp, data, pos, "u12");
	print_byte  (fp, data, pos, "u11");
	print_int   (fp, data, pos, "u12");

	print_model (fp, data, pos, "Model");

	print_int   (fp, data, pos, "u14");
	print_float (fp, data, pos, "PositionX");
	print_float (fp, data, pos, "PositionY");
	print_float (fp, data, pos, "PositionZ");
	print_float (fp, data, pos, "RotationX");
	print_float (fp, data, pos, "RotationY");
	print_float (fp, data, pos, "RotationZ");
	print_float (fp, data, pos, "ScaleX");
	print_float (fp, data, pos, "ScaleY");
	print_float (fp, data, pos, "ScaleZ");
	print_float (fp, data, pos, "u24");
	print_byte  (fp, data, pos, "u25");
}

static void print_enemy(FILE *fp, char *data, unsigned *pos)
{
	print_int   (fp, data, pos, "e01");
	print_byte  (fp, data, pos, "e02");
	print_int   (fp, data, pos, "e03");
	print_byte  (fp, data, pos, "e04");
	print_byte  (fp, data, pos, "e05");
	print_int   (fp, data, pos, "e06");
	print_int   (fp, data, pos, "e07");
	print_int   (fp, data, pos, "e08");
	print_byte  (fp, data, pos, "e09");
	print_byte  (fp, data, pos, "e10");
	print_byte  (fp, data, pos, "e11");
	print_int   (fp, data, pos, "e12");
	print_string(fp, data, pos, "e13");
	print_byte  (fp, data, pos, "e14");
	print_float (fp, data, pos, "e15");
	print_int   (fp, data, pos, "e16");

	print_common(fp, data, pos);
}

static void print_target(FILE *fp, char *data, unsigned *pos)
{
	int n = print_int(fp, data, pos, "t01");
	if (n == 0) return;
	char *str = print_string(fp, data, pos, "t02");

	if (strcmp(str, "cAISensorTargetStageAction") == 0)
	{
		print_int   (fp, data, pos, "t03a");
		print_int   (fp, data, pos, "t04a");
		print_int   (fp, data, pos, "t05a");
		print_float (fp, data, pos, "t06a");
		print_int   (fp, data, pos, "t07a");
		print_byte  (fp, data, pos, "t08a");
		print_short (fp, data, pos, "t09a");
		print_byte  (fp, data, pos, "t10a");
		n = print_int(fp, data, pos, "t11a");
		print_array (fp, data, pos, "t12a", n);
		n = print_int(fp, data, pos, "t13a");
		print_array (fp, data, pos, "t14a", n);
		print_int   (fp, data, pos, "t15a");
		print_int   (fp, data, pos, "t16a");
		print_int   (fp, data, pos, "t17a");
		print_float (fp, data, pos, "t18a");
		print_float (fp, data, pos, "t19a");
		print_float (fp, data, pos, "t20a");
		print_float (fp, data, pos, "t21a");
		print_int   (fp, data, pos, "t22a");
		print_float (fp, data, pos, "t23a");
		print_float (fp, data, pos, "t24a");
		print_int   (fp, data, pos, "t25a");
		print_int   (fp, data, pos, "t26a");
		print_int   (fp, data, pos, "t27a");
		print_int   (fp, data, pos, "t28a");
	}
	else if (strcmp(str, "cAISensorTargetGeneralPoint") == 0)
	{
		print_int   (fp, data, pos, "t03b");
		print_int   (fp, data, pos, "t04b");
		print_int   (fp, data, pos, "t05b");
		print_byte  (fp, data, pos, "t06b");
		print_int   (fp, data, pos, "t07b");
		print_int   (fp, data, pos, "t08b");
		print_int   (fp, data, pos, "t09b");
		print_float (fp, data, pos, "t10b");
		print_float (fp, data, pos, "t11b");
		print_float (fp, data, pos, "t12b");
		print_int   (fp, data, pos, "t13b");
		print_int   (fp, data, pos, "t14b");
		print_int   (fp, data, pos, "t15b");
		print_float (fp, data, pos, "t16b");
		print_float (fp, data, pos, "t17b");
		print_float (fp, data, pos, "t18b");
		print_int   (fp, data, pos, "t19b");
		print_int   (fp, data, pos, "t20b");
		print_float (fp, data, pos, "t21b");
		print_float (fp, data, pos, "t22b");
		print_int   (fp, data, pos, "t23b");
		print_int   (fp, data, pos, "t24b");
		print_int   (fp, data, pos, "t25b");
		print_int   (fp, data, pos, "t26b");
	}
	else if (strcmp(str, "cAISensorTargetNpc") == 0)
	{
		print_byte  (fp, data, pos, "t03c");
		print_int   (fp, data, pos, "t04c");
		print_int   (fp, data, pos, "t05c");
		print_float (fp, data, pos, "t06c");
		print_byte  (fp, data, pos, "t07c");
		print_byte  (fp, data, pos, "t08c");
		print_byte  (fp, data, pos, "t09c");
		print_int   (fp, data, pos, "t10c");
		print_int   (fp, data, pos, "t11c");
		print_byte  (fp, data, pos, "t12c");
		print_int   (fp, data, pos, "t13c");
		print_int   (fp, data, pos, "t14c");
		print_float (fp, data, pos, "t15c");
		print_int   (fp, data, pos, "t16c");
		print_int   (fp, data, pos, "t17c");
		print_int   (fp, data, pos, "t18c");
		print_int   (fp, data, pos, "t19c");
		print_float (fp, data, pos, "t20c");
		print_float (fp, data, pos, "t21c");
		print_float (fp, data, pos, "t22c");
		print_float (fp, data, pos, "t23c");
		print_float (fp, data, pos, "t24c");
		print_float (fp, data, pos, "t25c");
		print_float (fp, data, pos, "t26c");
		print_int   (fp, data, pos, "t27c");
		print_int   (fp, data, pos, "t28c");
		print_int   (fp, data, pos, "t29c");
		print_int   (fp, data, pos, "t30c");
	}
}

static void print_npc(FILE *fp, char *data, unsigned *pos)
{
	print_int   (fp, data, pos, "n01");
	print_string(fp, data, pos, "n02");

	print_int   (fp, data, pos, "n03");
	print_byte  (fp, data, pos, "n04");
	print_byte  (fp, data, pos, "n05");
	print_int   (fp, data, pos, "n06");
	print_byte  (fp, data, pos, "n07");
	print_int   (fp, data, pos, "n08");
	print_int   (fp, data, pos, "n09");
	print_int   (fp, data, pos, "n10");
	print_byte  (fp, data, pos, "n11");
	print_int   (fp, data, pos, "n12");
	print_int   (fp, data, pos, "n13");
	print_int   (fp, data, pos, "n14");
	print_int   (fp, data, pos, "n15");
	print_byte  (fp, data, pos, "n16");
	print_byte  (fp, data, pos, "n17");
	print_int   (fp, data, pos, "n18");

	print_target(fp, data, pos);

	print_byte  (fp, data, pos, "n20");
	print_int   (fp, data, pos, "n21");
	print_int   (fp, data, pos, "n22");
	print_int   (fp, data, pos, "n23");
	print_float (fp, data, pos, "n24");
	print_float (fp, data, pos, "n25");
	print_int   (fp, data, pos, "n26");
	print_float (fp, data, pos, "n27");
	print_float (fp, data, pos, "n28");
	print_int   (fp, data, pos, "n29");

	print_common(fp, data, pos);
}

static void print_placeable(FILE *fp, char *data, unsigned *pos)
{
	print_short (fp, data, pos, "p01");
	print_short (fp, data, pos, "p02");
	print_short (fp, data, pos, "p03");
	print_short (fp, data, pos, "p04");
	print_int   (fp, data, pos, "p05");
	print_int   (fp, data, pos, "p06");
	print_int   (fp, data, pos, "p07");
	print_int   (fp, data, pos, "p08");
	print_int   (fp, data, pos, "p09");
	print_int   (fp, data, pos, "p10");
	print_int   (fp, data, pos, "p11");
	print_common(fp, data, pos);
}

static int lot2txt(char *file, char *data)
{
	if (memcmp(data, "lot", 4))
	{
		printf("Error: Magic is not correct for LOT file.\n");
		return 1;
	}
	
	char path[PATH_MAX];
	snprintf(path, PATH_MAX, "%s.txt", file);
	FILE *fp = fopen(path, "w");
	if(!fp)
	{
		perror(path);
		return 1;
	}

	unsigned pos = 0;
	print_string(fp, data, &pos, "magic");
	print_int   (fp, data, &pos, "version");
	unsigned count = print_int(fp, data, &pos, "count");

	for (unsigned i = 0; i < count; i++)
	{
		print_index(fp, data, &pos);
		int type = print_int(fp, data, &pos, "Type");

		switch(type)
		{
		case 3:
		case 21:
		case 29:
		case 31:
		case 36:
		case 37:
		case 38:
		case 39:
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 68:
		case 71:
		case 73:
			print_enemy (fp, data, &pos);
			break;
		
		case 4:
			print_int   (fp, data, &pos, "e01a");
			print_byte  (fp, data, &pos, "e02a");
			print_byte  (fp, data, &pos, "e03a");
			print_byte  (fp, data, &pos, "e04a");
			print_enemy (fp, data, &pos);
			break;

		case 5:
		case 6:
			print_int   (fp, data, &pos, "e01b");
			print_byte  (fp, data, &pos, "e02b");
			print_byte  (fp, data, &pos, "e03b");
			print_enemy (fp, data, &pos);
			break;

		case 7:
		case 8:
		case 9:
		case 56:
		case 57:
			print_byte  (fp, data, &pos, "e01c");
			print_byte  (fp, data, &pos, "e02c");
			print_byte  (fp, data, &pos, "e03c");
			print_byte  (fp, data, &pos, "e04c");
			print_byte  (fp, data, &pos, "e05c");
			print_float (fp, data, &pos, "e04c");
			print_enemy (fp, data, &pos);
			break;

		case 10:
		case 11:
		case 12:
		case 13:
		case 58:
		case 59:
		case 60:
		case 61:
		case 62:
			print_byte  (fp, data, &pos, "e01d");
			print_byte  (fp, data, &pos, "e02d");
			print_enemy (fp, data, &pos);
			break;

		case 14:
		case 15:
		case 16:
		case 18:
		case 20:
		case 22:
		case 24:
		case 25:
		case 28:
		case 32:
		case 63:
		case 64:
		case 65:
			print_int   (fp, data, &pos, "e01e");
			print_enemy (fp, data, &pos);
			break;

		case 17:
		case 26:
			print_int   (fp, data, &pos, "e01f");
			print_int   (fp, data, &pos, "e02f");
			print_enemy (fp, data, &pos);
			break;

		case 34:
			print_int   (fp, data, &pos, "e01g");
			print_byte  (fp, data, &pos, "e02g");
			print_byte  (fp, data, &pos, "e03g");
			print_byte  (fp, data, &pos, "e04g");
			print_byte  (fp, data, &pos, "e05g");
			print_enemy (fp, data, &pos);
			break;

		case 19:
		case 69:
			print_byte  (fp, data, &pos, "e01h");
			print_byte  (fp, data, &pos, "e02h");
			print_byte  (fp, data, &pos, "e03h");
			print_byte  (fp, data, &pos, "e04h");
			print_byte  (fp, data, &pos, "e05h");
			print_byte  (fp, data, &pos, "e06h");
			print_byte  (fp, data, &pos, "e07h");
			print_byte  (fp, data, &pos, "e08h");
			print_byte  (fp, data, &pos, "e09h");
			print_byte  (fp, data, &pos, "e10h");
			print_byte  (fp, data, &pos, "e11h");
			print_enemy (fp, data, &pos);
			break;

		case 23:
			print_int   (fp, data, &pos, "e01i");
			print_int   (fp, data, &pos, "e02i");
			print_int   (fp, data, &pos, "e03i");
			print_byte  (fp, data, &pos, "e04i");
			print_byte  (fp, data, &pos, "e05i");
			print_int   (fp, data, &pos, "e06i");
			print_float (fp, data, &pos, "e07i");
			print_float (fp, data, &pos, "e08i");
			print_float (fp, data, &pos, "e09i");
			print_enemy (fp, data, &pos);
			break;

		case 27:
		case 33:
		case 67:
			print_byte  (fp, data, &pos, "e01j");
			print_enemy (fp, data, &pos);
			break;

		case 30:
		case 70:
			print_int   (fp, data, &pos, "e01k");
			print_int   (fp, data, &pos, "e02k");
			print_float (fp, data, &pos, "e03k");
			print_float (fp, data, &pos, "e04k");
			print_float (fp, data, &pos, "e05k");
			print_enemy (fp, data, &pos);
			break;

		case 35:
			print_byte  (fp, data, &pos, "e01l");
			print_float (fp, data, &pos, "e02l");
			print_enemy (fp, data, &pos);
			break;

		case 45:
			print_float (fp, data, &pos, "e01m");
			print_byte  (fp, data, &pos, "e02m");
			print_enemy (fp, data, &pos);
			break;

		case 66:
			print_int   (fp, data, &pos, "e01n");
			print_byte  (fp, data, &pos, "e02n");
			print_float (fp, data, &pos, "e03n");
			print_float (fp, data, &pos, "e04n");
			print_float (fp, data, &pos, "e05n");
			print_float (fp, data, &pos, "e06n");
			print_float (fp, data, &pos, "e07n");
			print_float (fp, data, &pos, "e08n");
			print_enemy (fp, data, &pos);
			break;

		case 47:
			print_npc   (fp, data, &pos);
			break;

		case 48:
			print_placeable(fp, data, &pos);
			break;

		case 49:
			print_string(fp, data, &pos, "p01a");
			print_placeable(fp, data, &pos);
			break;

		case 50:
			print_int   (fp, data, &pos, "p01b");
			print_short (fp, data, &pos, "p02b");
			print_string(fp, data, &pos, "p03b");
			print_placeable(fp, data, &pos);
			break;

		case 51:
			print_float (fp, data, &pos, "p01c");
			print_float (fp, data, &pos, "p02c");
			print_placeable(fp, data, &pos);
			break;

		case 52:
			print_int   (fp, data, &pos, "p01d");
			print_short (fp, data, &pos, "p02d");
			print_placeable(fp, data, &pos);
			break;

		case 53:
			print_short (fp, data, &pos, "p01e");
			print_short (fp, data, &pos, "p02e");
			print_short (fp, data, &pos, "p03e");
			print_float (fp, data, &pos, "p04e");
			print_float (fp, data, &pos, "p05e");
			print_int   (fp, data, &pos, "p06e");
			print_short (fp, data, &pos, "p07e");
			print_placeable(fp, data, &pos);
			break;

		case 46:
			print_int   (fp, data, &pos, "s01");
			print_int   (fp, data, &pos, "s02");
			print_byte  (fp, data, &pos, "s03");
			print_byte  (fp, data, &pos, "s04");
			print_int   (fp, data, &pos, "s05");
			print_int   (fp, data, &pos, "s06");
			print_int   (fp, data, &pos, "s07");
			print_int   (fp, data, &pos, "s08");
			print_int   (fp, data, &pos, "s09");
			print_int   (fp, data, &pos, "s10");
			print_int   (fp, data, &pos, "s11");
			print_int   (fp, data, &pos, "s12");
			print_int   (fp, data, &pos, "s13");
			print_int   (fp, data, &pos, "s14");
			print_int   (fp, data, &pos, "s15");
			print_string(fp, data, &pos, "s16");
			print_int   (fp, data, &pos, "s17");
			print_float (fp, data, &pos, "s18");
			print_float (fp, data, &pos, "s19");
			print_float (fp, data, &pos, "s20");
			print_float (fp, data, &pos, "s21");
			print_float (fp, data, &pos, "s22");
			print_float (fp, data, &pos, "s23");
			print_float (fp, data, &pos, "s24");
			print_float (fp, data, &pos, "s25");
			print_float (fp, data, &pos, "s26");
			print_float (fp, data, &pos, "s27");
			print_byte  (fp, data, &pos, "s28");
			break;

		case 54:
			print_target(fp, data, &pos);
			break;

		default:
			printf("Unsupported type %d with LOT file %s\n", type, file);
			fclose(fp);
			remove(path);
			return 1;
		}
	}

	fclose(fp);
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
			return 1;
		}

		fseek(fp, 0, SEEK_END);
		long len = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		char *data = malloc(len);
		fread(data, len, 1, fp);
		fclose(fp);

		ret |= lot2txt(argv[i], data);
		free(data);
	}

	return ret;
}
