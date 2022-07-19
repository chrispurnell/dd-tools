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
	int i = 0;

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

static int print_byte(FILE *fp, char **data, char *name)
{
	char i = **data;
	*data += 1;
	fprintf(fp, "%s:i8 = %d\n", name, i);
	return i;
}

static int print_short(FILE *fp, char **data, char *name)
{
	short i;
	memcpy(&i, *data, 2);
	*data += 2;
	fprintf(fp, "%s:i16 = %d\n", name, i);
	return i;
}

static int print_int(FILE *fp, char **data, char *name)
{
	int i;
	memcpy(&i, *data, 4);
	*data += 4;
	fprintf(fp, "%s:i32 = %d\n", name, i);
	return i;
}

static float print_float(FILE *fp, char **data, char *name)
{
	char str[64];
	float f;
	memcpy(&f, *data, 4);
	*data += 4;

	for (int i = 1; i < 48; i++)
	{
		snprintf(str, 64, "%.*f", i, f);
		if ((float)atof(str) == f)
			break;
	}

	fprintf(fp, "%s:f32 = %s\n", name, str);
	return f;
}

static char *print_string(FILE *fp, char **data, char *name)
{
	char *s = *data;
	*data += strlen(s) + 1;
	fprintf(fp, "%s:str = %s\n", name, s);
	return s;
}

static char *print_model(FILE *fp, char **data, char *name)
{
	char *s = *data;
	*data += strlen(s) + 1;
	fprintf(fp, "%s:str = %s%s\n", name, s, get_enemy_name(s));
	return s;
}

static void print_array(FILE *fp, char **data, char *name, int n)
{
	for (int i = 0; i < n; i++)
	{
		short v;
		memcpy(&v, *data, 2);
		*data += 2;
		fprintf(fp, "%s[%u]:i16 = %d\n", name, i, v);
	}
}

static void print_common(FILE *fp, char **data)
{
	print_byte  (fp, data, "u01");
	print_float (fp, data, "u02");
	print_byte  (fp, data, "u03");
	print_float (fp, data, "u04");
	print_byte  (fp, data, "u05");
	print_float (fp, data, "u06");
	print_byte  (fp, data, "u07");
	print_float (fp, data, "u08");
	print_byte  (fp, data, "u09");
	print_float (fp, data, "u10");

	print_int   (fp, data, "u11");
	print_int   (fp, data, "u12");
	print_byte  (fp, data, "u13");
	print_byte  (fp, data, "u14");
	print_byte  (fp, data, "u15");
	print_int   (fp, data, "u16");

	print_model (fp, data, "Model");

	print_int   (fp, data, "u18");
	print_float (fp, data, "PositionX");
	print_float (fp, data, "PositionY");
	print_float (fp, data, "PositionZ");
	print_float (fp, data, "RotationX");
	print_float (fp, data, "RotationY");
	print_float (fp, data, "RotationZ");
	print_float (fp, data, "ScaleX");
	print_float (fp, data, "ScaleY");
	print_float (fp, data, "ScaleZ");
	print_float (fp, data, "u28");
	print_byte  (fp, data, "u29");
}

static void print_enemy(FILE *fp, char **data)
{
	print_int   (fp, data, "e01");
	print_byte  (fp, data, "e02");
	print_int   (fp, data, "e03");
	print_byte  (fp, data, "e04");
	print_byte  (fp, data, "e05");
	print_int   (fp, data, "e06");
	print_int   (fp, data, "e07");
	print_int   (fp, data, "e08");
	print_byte  (fp, data, "e09");
	print_byte  (fp, data, "e10");
	print_byte  (fp, data, "e11");
	print_int   (fp, data, "e12");
	print_string(fp, data, "FSM");
	print_byte  (fp, data, "e14");
	print_float (fp, data, "e15");
	print_int   (fp, data, "e16");

	print_common(fp, data);
}

static void print_target(FILE *fp, char **data)
{
	int n = print_int(fp, data, "t01");
	if (n == 0) return;
	char *str = print_string(fp, data, "t02");

	if (strcmp(str, "cAISensorTargetStageAction") == 0)
	{
		print_int   (fp, data, "t03a");
		print_int   (fp, data, "t04a");
		print_int   (fp, data, "t05a");
		print_float (fp, data, "t06a");
		print_int   (fp, data, "t07a");
		print_byte  (fp, data, "t08a");
		print_short (fp, data, "t09a");
		print_byte  (fp, data, "t10a");
		n = print_int(fp, data, "t11a");
		print_array (fp, data, "t12a", n);
		n = print_int(fp, data, "t13a");
		print_array (fp, data, "t14a", n);
		print_int   (fp, data, "t15a");
		print_int   (fp, data, "t16a");
		print_int   (fp, data, "t17a");
		print_float (fp, data, "t18a");
		print_float (fp, data, "t19a");
		print_float (fp, data, "t20a");
		print_float (fp, data, "t21a");
		print_int   (fp, data, "t22a");
		print_float (fp, data, "t23a");
		print_float (fp, data, "t24a");
		print_int   (fp, data, "t25a");
		print_int   (fp, data, "t26a");
		print_int   (fp, data, "t27a");
		print_int   (fp, data, "t28a");
	}
	else if (strcmp(str, "cAISensorTargetGeneralPoint") == 0)
	{
		print_int   (fp, data, "t03b");
		print_int   (fp, data, "t04b");
		print_int   (fp, data, "t05b");
		print_byte  (fp, data, "t06b");
		print_int   (fp, data, "t07b");
		print_int   (fp, data, "t08b");
		print_int   (fp, data, "t09b");
		print_float (fp, data, "t10b");
		print_float (fp, data, "t11b");
		print_float (fp, data, "t12b");
		print_int   (fp, data, "t13b");
		print_int   (fp, data, "t14b");
		print_int   (fp, data, "t15b");
		print_float (fp, data, "t16b");
		print_float (fp, data, "t17b");
		print_float (fp, data, "t18b");
		print_int   (fp, data, "t19b");
		print_int   (fp, data, "t20b");
		print_float (fp, data, "t21b");
		print_float (fp, data, "t22b");
		print_int   (fp, data, "t23b");
		print_int   (fp, data, "t24b");
		print_int   (fp, data, "t25b");
		print_int   (fp, data, "t26b");
	}
	else if (strcmp(str, "cAISensorTargetNpc") == 0)
	{
		print_byte  (fp, data, "t03c");
		print_int   (fp, data, "t04c");
		print_int   (fp, data, "t05c");
		print_float (fp, data, "t06c");
		print_byte  (fp, data, "t07c");
		print_byte  (fp, data, "t08c");
		print_byte  (fp, data, "t09c");
		print_int   (fp, data, "t10c");
		print_int   (fp, data, "t11c");
		print_byte  (fp, data, "t12c");
		print_int   (fp, data, "t13c");
		print_int   (fp, data, "t14c");
		print_float (fp, data, "t15c");
		print_int   (fp, data, "t16c");
		print_int   (fp, data, "t17c");
		print_int   (fp, data, "t18c");
		print_int   (fp, data, "t19c");
		print_float (fp, data, "t20c");
		print_float (fp, data, "t21c");
		print_float (fp, data, "t22c");
		print_float (fp, data, "t23c");
		print_float (fp, data, "t24c");
		print_float (fp, data, "t25c");
		print_float (fp, data, "t26c");
		print_int   (fp, data, "t27c");
		print_int   (fp, data, "t28c");
		print_int   (fp, data, "t29c");
		print_int   (fp, data, "t30c");
	}
}

static void print_npc(FILE *fp, char **data)
{
	print_int   (fp, data, "n01");
	print_string(fp, data, "FSM");

	print_int   (fp, data, "n03");
	print_byte  (fp, data, "n04");
	print_byte  (fp, data, "n05");
	print_int   (fp, data, "n06");
	print_byte  (fp, data, "n07");
	print_int   (fp, data, "n08");
	print_int   (fp, data, "n09");
	print_int   (fp, data, "n10");
	print_byte  (fp, data, "n11");
	print_int   (fp, data, "n12");
	print_int   (fp, data, "n13");
	print_int   (fp, data, "n14");
	print_int   (fp, data, "n15");
	print_byte  (fp, data, "n16");
	print_byte  (fp, data, "n17");
	print_int   (fp, data, "n18");

	print_target(fp, data);

	print_byte  (fp, data, "n20");
	print_int   (fp, data, "n21");
	print_int   (fp, data, "n22");
	print_int   (fp, data, "n23");
	print_float (fp, data, "n24");
	print_float (fp, data, "n25");
	print_int   (fp, data, "n26");
	print_float (fp, data, "n27");
	print_float (fp, data, "n28");
	print_int   (fp, data, "n29");

	print_common(fp, data);
}

static void print_placeable(FILE *fp, char **data)
{
	print_short (fp, data, "p01");
	print_short (fp, data, "p02");
	print_short (fp, data, "p03");
	print_short (fp, data, "p04");
	print_int   (fp, data, "p05");
	print_int   (fp, data, "p06");
	print_int   (fp, data, "p07");
	print_int   (fp, data, "p08");
	print_int   (fp, data, "p09");
	print_int   (fp, data, "p10");
	print_int   (fp, data, "p11");
	print_common(fp, data);
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

	print_string(fp, &data, "magic");
	print_int   (fp, &data, "version");
	int count = print_int(fp, &data, "count");

	for (int i = 0; i < count; i++)
	{
		fprintf(fp, "\n[ENTRY%04u]\n", i);

		print_int(fp, &data, "Index");
		int type = print_int(fp, &data, "Type");

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
			print_enemy (fp, &data);
			break;
		
		case 4:
			print_int   (fp, &data, "e01a");
			print_byte  (fp, &data, "e02a");
			print_byte  (fp, &data, "e03a");
			print_byte  (fp, &data, "e04a");
			print_enemy (fp, &data);
			break;

		case 5:
		case 6:
			print_int   (fp, &data, "e01b");
			print_byte  (fp, &data, "e02b");
			print_byte  (fp, &data, "e03b");
			print_enemy (fp, &data);
			break;

		case 7:
		case 8:
		case 9:
		case 56:
		case 57:
			print_byte  (fp, &data, "e01c");
			print_byte  (fp, &data, "e02c");
			print_byte  (fp, &data, "e03c");
			print_byte  (fp, &data, "e04c");
			print_byte  (fp, &data, "e05c");
			print_float (fp, &data, "e04c");
			print_enemy (fp, &data);
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
			print_byte  (fp, &data, "e01d");
			print_byte  (fp, &data, "e02d");
			print_enemy (fp, &data);
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
			print_int   (fp, &data, "e01e");
			print_enemy (fp, &data);
			break;

		case 17:
		case 26:
			print_int   (fp, &data, "e01f");
			print_int   (fp, &data, "e02f");
			print_enemy (fp, &data);
			break;

		case 34:
			print_int   (fp, &data, "e01g");
			print_byte  (fp, &data, "e02g");
			print_byte  (fp, &data, "e03g");
			print_byte  (fp, &data, "e04g");
			print_byte  (fp, &data, "e05g");
			print_enemy (fp, &data);
			break;

		case 19:
		case 69:
			print_byte  (fp, &data, "e01h");
			print_byte  (fp, &data, "e02h");
			print_byte  (fp, &data, "e03h");
			print_byte  (fp, &data, "e04h");
			print_byte  (fp, &data, "e05h");
			print_byte  (fp, &data, "e06h");
			print_byte  (fp, &data, "e07h");
			print_byte  (fp, &data, "e08h");
			print_byte  (fp, &data, "e09h");
			print_byte  (fp, &data, "e10h");
			print_byte  (fp, &data, "e11h");
			print_enemy (fp, &data);
			break;

		case 23:
			print_int   (fp, &data, "e01i");
			print_int   (fp, &data, "e02i");
			print_int   (fp, &data, "e03i");
			print_byte  (fp, &data, "e04i");
			print_byte  (fp, &data, "e05i");
			print_int   (fp, &data, "e06i");
			print_float (fp, &data, "e07i");
			print_float (fp, &data, "e08i");
			print_float (fp, &data, "e09i");
			print_enemy (fp, &data);
			break;

		case 27:
		case 33:
		case 67:
			print_byte  (fp, &data, "e01j");
			print_enemy (fp, &data);
			break;

		case 30:
		case 70:
			print_int   (fp, &data, "e01k");
			print_int   (fp, &data, "e02k");
			print_float (fp, &data, "e03k");
			print_float (fp, &data, "e04k");
			print_float (fp, &data, "e05k");
			print_enemy (fp, &data);
			break;

		case 35:
			print_byte  (fp, &data, "e01l");
			print_float (fp, &data, "e02l");
			print_enemy (fp, &data);
			break;

		case 45:
			print_float (fp, &data, "e01m");
			print_byte  (fp, &data, "e02m");
			print_enemy (fp, &data);
			break;

		case 66:
			print_int   (fp, &data, "e01n");
			print_byte  (fp, &data, "e02n");
			print_float (fp, &data, "e03n");
			print_float (fp, &data, "e04n");
			print_float (fp, &data, "e05n");
			print_float (fp, &data, "e06n");
			print_float (fp, &data, "e07n");
			print_float (fp, &data, "e08n");
			print_enemy (fp, &data);
			break;

		case 47:
			print_npc   (fp, &data);
			break;

		case 48:
			print_placeable(fp, &data);
			break;

		case 49:
			print_string(fp, &data, "FSM");
			print_placeable(fp, &data);
			break;

		case 50:
			print_int   (fp, &data, "p01b");
			print_short (fp, &data, "p02b");
			print_string(fp, &data, "FSM");
			print_placeable(fp, &data);
			break;

		case 51:
			print_float (fp, &data, "p01c");
			print_float (fp, &data, "p02c");
			print_placeable(fp, &data);
			break;

		case 52:
			print_int   (fp, &data, "p01d");
			print_short (fp, &data, "p02d");
			print_placeable(fp, &data);
			break;

		case 53:
			print_short (fp, &data, "p01e");
			print_short (fp, &data, "p02e");
			print_short (fp, &data, "p03e");
			print_float (fp, &data, "p04e");
			print_float (fp, &data, "p05e");
			print_int   (fp, &data, "p06e");
			print_short (fp, &data, "p07e");
			print_placeable(fp, &data);
			break;

		case 46:
			print_int   (fp, &data, "s01");
			print_int   (fp, &data, "s02");
			print_byte  (fp, &data, "s03");
			print_byte  (fp, &data, "s04");
			print_int   (fp, &data, "s05");
			print_common(fp, &data);
			break;

		case 54:
			print_target(fp, &data);
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
