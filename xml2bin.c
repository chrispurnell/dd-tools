#include <string.h>
#include <stdio.h>
#include <limits.h>

#define BUF_SIZE 1024

static char xml_buffer[BUF_SIZE];
static int xml_status;
static unsigned xml_index;
static char *xml_tag;
static char *xml_attr;
static char *xml_value;

#define XML_TEXT  0
#define XML_NAME  1
#define XML_TAG   2
#define XML_ATTR  3
#define XML_PREEQ 4
#define XML_EQ    5
#define XML_VALUE 6
#define XML_COM   7
#define XML_COM1  8
#define XML_COM2  9

#define XML_OK    0
#define XML_ERROR 1
#define XML_EOF  -1

static void xml_init()
{
	xml_status = XML_TEXT;
	xml_index = 0;

	xml_tag = xml_buffer;
	xml_attr = xml_buffer;
	xml_value = xml_buffer;
}

static int xml_parse(FILE *fp)
{
	int c;

	while ((c = fgetc(fp)) >= 0)
	{
		switch (xml_status)
		{
		case XML_TEXT:
			if (c == '<')
			{
				xml_tag = xml_buffer;
				xml_index = 0;
				xml_status = XML_NAME;
			}
			break;

		case XML_NAME:
			switch (c)
			{
			case '>':
				xml_status = XML_TEXT;
				break;
			case '<':
			case '=':
			case '"':
				return XML_ERROR;
			case '-':
				if (xml_index == 2 && xml_buffer[0] == '!' && xml_buffer[1] == '-')
				{
					xml_status = XML_COM;
					break;
				}
				/* fallthrough */
			default:
				if (c > ' ')
				{
					xml_buffer[xml_index++] = c;
				}
				else
				{
					xml_buffer[xml_index++] = 0;
					xml_attr = xml_buffer + xml_index;
					xml_status = XML_TAG;
				}
				if (xml_index >= BUF_SIZE)
					return XML_ERROR;
			}
			break;

		case XML_TAG:
			switch (c)
			{
			case '>':
				xml_status = XML_TEXT;
				break;
			case '<':
			case '=':
			case '"':
				return XML_ERROR;
			default:
				if (c > ' ')
				{
					*xml_attr = c;
					xml_index = xml_attr - xml_buffer;
					xml_status = XML_ATTR;
					if (++xml_index >= BUF_SIZE)
						return XML_ERROR;
				}
			}
			break;

		case XML_ATTR:
			switch (c)
			{
			case '>':
				xml_status = XML_TEXT;
				break;
			case '<':
			case '"':
				return XML_ERROR;
			case '=':
				xml_buffer[xml_index] = 0;
				xml_status = XML_EQ;
				if (++xml_index >= BUF_SIZE)
					return XML_ERROR;
				break;
			default:
				if (c > ' ')
				{
					xml_buffer[xml_index] = c;
				}
				else
				{
					xml_buffer[xml_index] = 0;
					xml_status = XML_PREEQ;
				}
				if (++xml_index >= BUF_SIZE)
					return XML_ERROR;
			}
			break;

		case XML_PREEQ:
			switch (c)
			{
			case '>':
				xml_status = XML_TEXT;
				break;
			case '=':
				xml_status = XML_EQ;
				break;
			default:
				if (c > ' ') return XML_ERROR;
			}
			break;

		case XML_EQ:
			switch (c)
			{
			case '>':
				xml_status = XML_TEXT;
				break;
			case '"':
				xml_value = xml_buffer + xml_index;
				xml_status = XML_VALUE;
				break;
			default:
				if (c < ' ') break;
				return XML_ERROR;
			}
			break;

		case XML_VALUE:
			if (c < ' ') return XML_ERROR;
			if (c == '"')
			{
				xml_buffer[xml_index] = 0;
				xml_status = XML_TAG;
				return XML_OK;
			}
			xml_buffer[xml_index] = c;
			if (++xml_index >= BUF_SIZE)
				return XML_ERROR;
			break;

		case XML_COM:
			if (c == '-') xml_status = XML_COM1;
			break;

		case XML_COM1:
			if (c == '-')
			{
				xml_status = XML_COM2;
				break;
			}
			xml_status = XML_COM;
			break;
			
		case XML_COM2:
			if (c == '>')
			{
				xml_status = XML_TEXT;
				break;
			}
			xml_status = XML_COM;
			break;
		}
	}

	if (xml_status != XML_TEXT)
		return XML_ERROR;

	return XML_EOF;
}

int main(int argc, char **argv)
{
        char path[PATH_MAX];
        int ret = 0;

        for (int i = 1; i < argc; i++)
        {
		snprintf(path, PATH_MAX, "%s.xml", argv[i]);
                FILE *ifp = fopen(path, "rb");
                if (!ifp)
                {
                        perror(path);
                        ret = 1;
                        continue;
                }

		FILE *ofp = fopen(argv[i], "wb");
		if (!ofp)
		{
			perror(argv[i]);
			fclose(ifp);
			ret = 1;
			continue;
		}

		xml_init();
		int err;
		while ((err = xml_parse(ifp)) == XML_OK)
		{
			if (strcmp(xml_attr, "value") == 0)
			{
				if (strcmp(xml_tag, "i8") == 0)
				{
					int i = 0;
					sscanf(xml_value, "%i", &i);
					fputc(i, ofp);
				}
				else if (strcmp(xml_tag, "i16") == 0)
				{
					int i = 0;
					sscanf(xml_value, "%i", &i);
					fwrite(&i, 2, 1, ofp);
				}
				else if (strcmp(xml_tag, "i32") == 0)
				{
					int i = 0;
					sscanf(xml_value, "%i", &i);
					fwrite(&i, 4, 1, ofp);
				}
				else if (strcmp(xml_tag, "f32") == 0)
				{
					float f = 0;
					sscanf(xml_value, "%f", &f);
					fwrite(&f, 4, 1, ofp);
				}
				else if (strcmp(xml_tag, "string") == 0)
				{
					fwrite(xml_value, strlen(xml_value)+1, 1, ofp);
				}
			}
			else if (strcmp(xml_attr, "count") == 0)
			{
				if (strcmp(xml_tag, "array") == 0)
				{
					int i = 0;
					sscanf(xml_value, "%i", &i);
					fwrite(&i, 4, 1, ofp);
				}
			}
			else if (strcmp(xml_attr, "type") == 0)
			{
				if (strcmp(xml_tag, "class") == 0)
				{
					fwrite(xml_value, strlen(xml_value)+1, 1, ofp);
				}
			}
			else if (strcmp(xml_attr, "magic") == 0)
			{
				int i = 0;
				sscanf(xml_value, "%i", &i);
				fwrite(&i, 4, 1, ofp);
			}
		}

		if (err != XML_EOF)
		{
			printf("Error parsing file %s\n", path);
			ret = 1;
		}

		fclose(ofp);
                fclose(ifp);
        }

        return ret;
}
