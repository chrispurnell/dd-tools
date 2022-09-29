#define INI_MAX_LINE 256

class INI
{
public:
	char *section;
	char *name;
	char *value;

private:
	int pos;
	char buffer[INI_MAX_LINE];

	char *strip(char *str)
	{
		unsigned char c;

		while ((c = *str) != 0)
		{
			if (c > 32) break;
			str++;
		}

		char *s = str;
		char *e = str;

		while ((c = *s) != 0)
		{
			s++;
			if (c > 32) e = s;
		}

		*e = 0;
		return str;
	}

public:
	INI()
	{
		pos = -1;
	}

	bool parse(FILE *fp)
	{
		char *s;

		section = NULL;
		name = NULL;
		value = NULL;

		if (pos < 0)
		{
			if (!fgets(buffer, INI_MAX_LINE, fp))
				return false;
			pos = 0;
		}

		char *line = buffer + pos;

		s = strchr(line, ';');
		if (s)
		{
			*s = 0;
			pos = s + 1 - buffer;
		}
		else
		{
			pos = -1;
		}

		if (line[0] == '[' && (s = strchr(line, ']')))
		{
			*s = 0;
			section = strip(line + 1);
			return true;
		}

		s = strchr(line, '=');
		if (s)
		{
			*s = 0;
			name = strip(line);
			value = strip(s + 1);
		}

		return true;
	}
};
