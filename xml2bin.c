#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#ifdef _WIN32
#define strcasecmp stricmp
#endif

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
#define XML_EQ    4
#define XML_VALD  5
#define XML_VALS  6
#define XML_SPC   7
#define XML_END   8
#define XML_COM   9
#define XML_COM1  10
#define XML_COM2  11

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
				xml_buffer[xml_index] = 0;
				xml_attr = xml_buffer + xml_index;
				xml_value = xml_attr; 
				return XML_OK;
			case '<':
			case '=':
			case '"':
			case '\'':
				return XML_ERROR;
			case '/':
				if (xml_index == 0)
				{
					xml_buffer[0] = '/';
					xml_index = 1;
					break;
				}
				if (xml_buffer[0] == '/')
					return XML_ERROR;
				xml_buffer[xml_index] = 0;
				if (++xml_index >= BUF_SIZE)
					return XML_ERROR;
				xml_attr = xml_buffer + xml_index;
				xml_attr[0] = 0;
				xml_value = xml_attr; 
				xml_status = XML_END;
				return XML_OK;
			case '-':
				if (xml_index == 2 && xml_buffer[0] == '!' && xml_buffer[1] == '-')
				{
					xml_status = XML_COM;
					break;
				}
				xml_buffer[xml_index] = '-';
				if (++xml_index >= BUF_SIZE)
					return XML_ERROR;
				break;
			default:
				if (c > ' ')
				{
					xml_buffer[xml_index] = c;
					if (++xml_index >= BUF_SIZE)
						return XML_ERROR;
					break;
				}
				xml_buffer[xml_index] = 0;
				if (++xml_index >= BUF_SIZE)
					return XML_ERROR;
				xml_attr = xml_buffer + xml_index;
				xml_attr[0] = 0;
				xml_value = xml_attr; 
				xml_status = XML_TAG;
				return XML_OK;
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
			case '\'':
				return XML_ERROR;
			case '/':
				if (xml_buffer[0] == '/')
					return XML_ERROR;
				xml_status = XML_END;
				break;
			case '?':
				if (xml_buffer[0] != '?')
					return XML_ERROR;
				xml_status = XML_END;
				break;
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
			case '\'':
			case '/':
				return XML_ERROR;
			case '=':
				xml_buffer[xml_index] = 0;
				xml_status = XML_EQ;
				if (++xml_index >= BUF_SIZE)
					return XML_ERROR;
				break;
			default:
				if (c <= ' ')
				{
					xml_status = XML_TAG;
					break;
				}
				xml_buffer[xml_index] = c;
				if (++xml_index >= BUF_SIZE)
					return XML_ERROR;
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
				xml_status = XML_VALD;
				break;
			case '\'':
				xml_value = xml_buffer + xml_index;
				xml_status = XML_VALS;
				break;
			default:
				if (c > ' ') return XML_ERROR;
			}
			break;

		case XML_VALD:
			if (c < ' ') return XML_ERROR;
			if (c == '"')
			{
				xml_buffer[xml_index] = 0;
				xml_status = XML_SPC;
				return XML_OK;
			}
			xml_buffer[xml_index] = c;
			if (++xml_index >= BUF_SIZE)
				return XML_ERROR;
			break;

		case XML_VALS:
			if (c < ' ') return XML_ERROR;
			if (c == '\'')
			{
				xml_buffer[xml_index] = 0;
				xml_status = XML_SPC;
				return XML_OK;
			}
			xml_buffer[xml_index] = c;
			if (++xml_index >= BUF_SIZE)
				return XML_ERROR;
			break;

		case XML_SPC:
			switch (c)
			{
			case '>':
				xml_status = XML_TEXT;
				break;
			case '/':
				if (xml_buffer[0] == '/')
					return XML_ERROR;
				xml_status = XML_END;
				break;
			case '?':
				if (xml_buffer[0] != '?')
					return XML_ERROR;
				xml_status = XML_END;
				break;

			default:
				if (c > ' ') return XML_ERROR;
				xml_status = XML_TAG;
			}
			break;

		case XML_END:
			if (c == '>')
			{
				xml_status = XML_TEXT;
				break;
			}
			if (c > ' ') return XML_ERROR;
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

static int match_bits(char *str)
{
	if (str[0] == '/') str++;
	if (str[0] != 'b') return 0;
	unsigned c1 = str[1] - '0';
	if (c1 > 9) return 0;
	unsigned c2 = str[2];
	if (c2 == 0) return c1;
	c2 -= '0';
	if (c2 > 9) return 0;
	if (str[3]) return 0;
	return c1 * 10 + c2;
}

int main(int argc, char **argv)
{
	char path[PATH_MAX];
	int ret = 0;

	for (int i = 1; i < argc; i++)
	{
		FILE *ifp, *ofp;
		char *s = strrchr(argv[i], '.');

		if (s && strcasecmp(s, ".xml") == 0)
		{
			ifp = fopen(argv[i], "rb");
			if (!ifp)
			{
				perror(path);
				ret = 1;
				continue;
			}

			int n = s - argv[i];
			snprintf(path, PATH_MAX, "%.*s", n, argv[i]);
			ofp = fopen(path, "wb");
			if (!ofp)
			{
				perror(argv[i]);
				fclose(ifp);
				ret = 1;
				continue;
			}
		}
		else
		{
			snprintf(path, PATH_MAX, "%s.xml", argv[i]);
			ifp = fopen(path, "rb");
			if (!ifp)
			{
				perror(path);
				ret = 1;
				continue;
			}

			ofp = fopen(argv[i], "wb");
			if (!ofp)
			{
				perror(argv[i]);
				fclose(ifp);
				ret = 1;
				continue;
			}
		}

		int bits = 0;
		unsigned value = 0;

		xml_init();
		int err;
		while ((err = xml_parse(ifp)) == XML_OK)
		{
			int b = match_bits(xml_tag);
			if (b)
			{
				if (strcmp(xml_attr, "value") == 0)
				{
					unsigned u = strtoul(xml_value, NULL, 0);
					u &= (1 << b) - 1;
					value |= u << bits;
					bits += b;

					while (bits >= 8)
					{
						fputc(value, ofp);
						bits -= 8;
						value >>= 8;
					}
				}
			}
			else
			{
				if (bits)
				{
					fputc(value, ofp);
					bits = 0;
					value = 0;
				}

				if (strcmp(xml_attr, "value") == 0)
				{
					if (strcmp(xml_tag, "i8") == 0)
					{
						uint8_t i8 = strtoul(xml_value, NULL, 0);
						fputc(i8, ofp);
					}
					else if (strcmp(xml_tag, "i16") == 0)
					{
						uint16_t i16 = strtoul(xml_value, NULL, 0);
						fwrite(&i16, 2, 1, ofp);
					}
					else if (strcmp(xml_tag, "i32") == 0)
					{
						uint32_t i32 = strtoul(xml_value, NULL, 0);
						fwrite(&i32, 4, 1, ofp);
					}
					else if (strcmp(xml_tag, "i64") == 0)
					{
						uint64_t i64 = strtoull(xml_value, NULL, 0);
						fwrite(&i64, 8, 1, ofp);
					}
					else if (strcmp(xml_tag, "f32") == 0)
					{
						float f = strtof(xml_value, NULL);
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
						uint32_t i32 = strtoul(xml_value, NULL, 0);
						fwrite(&i32, 4, 1, ofp);
					}
				}
				else if (strcmp(xml_attr, "magic") == 0)
				{
					uint32_t i32 = strtoul(xml_value, NULL, 0);
					fwrite(&i32, 4, 1, ofp);
				}
			}
		}

		if (bits || err != XML_EOF)
		{
			printf("Error parsing file %s\n", path);
			ret = 1;
		}

		fclose(ofp);
		fclose(ifp);
	}

	return ret;
}
