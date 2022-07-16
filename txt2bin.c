#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

const char *SPACE = " \t\n\v\f\r";

int txt2bin(FILE *ifp, char *file)
{
	FILE *fp = fopen(file, "wb");
	if (!fp)
	{
		perror(file);
		return 1;
	}

	char line[1024];
	int ret = 0;

	while (fgets(line, 1024, ifp))
	{
		char *s = line + strspn(line, SPACE);

		if (!*s) continue;
		if (*s == '[') continue;

		char *p = strchr(s, '=');
		if (!p)
		{
			printf("Error parsing file: %s.txt\n", file);
			ret = 1;
			break;
		}
		*(p++) = 0;
		p += strspn(p, SPACE);
		p[strcspn(p, SPACE)] = 0;

		s = strchr(s, ':');
		if (!s)
		{
			printf("Error parsing file: %s.txt\n", file);
			ret = 1;
			break;
		}
		s++;
		s[strcspn(s, SPACE)] = 0;

		if (strcmp(s, "i8") == 0)
		{
			fputc(atoi(p), fp);
			continue;
		}

		if (strcmp(s, "i16") == 0)
		{
			short i = atoi(p);
			fwrite(&i, 2, 1, fp);
			continue;
		}

		if (strcmp(s, "i32") == 0)
		{
			int i = atoi(p);
			fwrite(&i, 4, 1, fp);
			continue;
		}
		else if (strcmp(s, "f32") == 0)
		{
			float f = atof(p);
			fwrite(&f, 4, 1, fp);
			continue;
		}

		if (strcmp(s, "str") == 0)
		{
			fwrite(p, strlen(p) + 1, 1, fp);
			continue;
		}

		printf("Error: unknown type: %s\n", s);
		ret = 1;
		break;
	}

	fclose(fp);
	if (ret) remove(file);
	return ret;
}

int main(int argc, char **argv)
{
	char path[PATH_MAX];
	int ret = 0;

	for (int i = 1; i < argc; i++)
	{
		snprintf(path, PATH_MAX, "%s.txt", argv[i]);
		FILE *fp = fopen(path, "r");
		if (!fp)
		{
			perror(argv[i]);
			return 1;
                }

		ret |= txt2bin(fp, argv[i]);
		fclose(fp);
	}

	return ret;
}
