#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

inline void *operator new(size_t s) { return malloc(s); }
inline void operator delete(void *p) { free(p); }
inline void operator delete(void *p, size_t) { free(p); }

namespace {

#include "ini.h"
#include "xml.h"

#include "xfs.h"
#include "xfs_types.h"

unsigned hash_table[256] = {
	0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
	0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
	0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
	0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
	0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
	0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
	0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
	0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
	0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
	0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
	0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
	0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
	0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
	0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
	0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
	0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
	0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
	0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
	0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
	0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
	0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
	0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
	0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
	0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
	0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
	0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
	0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
	0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
	0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
	0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
	0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
	0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
	0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
	0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
	0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
	0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
	0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
	0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
	0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
	0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
	0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
	0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
	0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
	0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
	0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
	0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
	0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
	0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
	0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
	0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
	0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
	0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
	0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
	0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
	0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
	0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
	0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
	0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
	0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
	0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
	0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
	0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
	0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
	0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

unsigned calc_hash(const char *ptr)
{
	char *end = NULL;
	unsigned val = strtoul(ptr, &end, 0);
	if (!*end) return val;
	
	val = 0xFFFFFFFF;
	char c;
	while ((c = *ptr))
	{
		ptr++;
		val = (val >> 8) ^ hash_table[(val ^ c) & 255];
	}
	return val & 0x7FFFFFFF;
}

bool XFS::write_xml(File *fp, xml::node *node)
{
	int index = -1;

	xml::attribute *attr = node->find_attr("type");
	if (attr)
	{
		uint32_t hash = calc_hash(attr->value());
		for (unsigned i = 0; i < header.objDataNum; i++)
		{
			if (objs[i]->hash == hash)
			{
				index = i;
			}
		}
	}

	if (index < 0)
	{
		fp->write16(index << 1);
		fp->write16(0);
		return true;
	}

	fp->write16((index << 1) | 1);
	fp->write16(header.maxClassID++);

	long offset = ftell(fp->fp);
	fp->write32(0);

	OBJECTDATA *o = objs[index];
	xml::node *np = node->child();

	for (unsigned i = 0; i < o->num; i++)
	{
		PARAMETER *p = o->param + i;
		const char *tname = type2name(p->type);

		if (!np)
		{
			printf("Error: %s expected.\n", tname);
			return false;
		}

		unsigned count = 1;
		xml::node *n = np;

		if (p->flags & 0x20)
		{
			if (strcmp(np->name(), "array"))
			{
				printf("Error: array expected, found: %s\n", np->name());
				return false;
			}

			count = 0;

			for (n = np->child(); n; n = n->next())
			{
				count++;
			}

			n = np->child();
		}

		fp->write32(count);

		for (unsigned j = 0; j < count; j++)
		{
			if (strcmp(n->name(), tname))
			{
				printf("Error: %s expected, found: %s\n", tname, n->name());
				return false;
			}

			switch (p->type)
			{
			case TYPE_CLASS:
			case TYPE_CLASSREF:
				write_xml(fp, n);
				break;
			case TYPE_BOOL:
				attr = n->find_attr("value");
				fp->write8((attr && strcmp(attr->value(), "true") == 0) ? 1 : 0);
				break;
			case TYPE_U8:
			case TYPE_S8:
				fp->write8a(n->find_attr("value"));
				break;
			case TYPE_U16:
			case TYPE_S16:
				fp->write16a(n->find_attr("value"));
				break;
			case TYPE_U32:
			case TYPE_S32:
				fp->write32a(n->find_attr("value"));
				break;
			case TYPE_F32:
				fp->writefa(n->find_attr("value"));
				break;
			case TYPE_STRING:
			case TYPE_CSTRING:
				attr = n->find_attr("value");
				if (attr)
				{
					const char *str = attr->value();
					size_t len = strlen(str) + 1;
					fwrite(str, len, 1, fp->fp);
				}
				else
				{
					fp->write8(0);
				}
				break;
			case TYPE_MATRIX:
				fp->writefa(n->find_attr("m0"));
				fp->writefa(n->find_attr("m1"));
				fp->writefa(n->find_attr("m2"));
				fp->writefa(n->find_attr("m3"));
				fp->writefa(n->find_attr("m4"));
				fp->writefa(n->find_attr("m5"));
				fp->writefa(n->find_attr("m6"));
				fp->writefa(n->find_attr("m7"));
				fp->writefa(n->find_attr("m8"));
				fp->writefa(n->find_attr("m9"));
				fp->writefa(n->find_attr("m10"));
				fp->writefa(n->find_attr("m11"));
				fp->writefa(n->find_attr("m12"));
				fp->writefa(n->find_attr("m13"));
				fp->writefa(n->find_attr("m14"));
				fp->writefa(n->find_attr("m15"));
				break;
			case TYPE_VECTOR3:
				fp->writefa(n->find_attr("x"));
				fp->writefa(n->find_attr("y"));
				fp->writefa(n->find_attr("z"));
				fp->writefa(n->find_attr("pad"));
				break;
			case TYPE_VECTOR4:
				fp->writefa(n->find_attr("x"));
				fp->writefa(n->find_attr("y"));
				fp->writefa(n->find_attr("z"));
				fp->writefa(n->find_attr("w"));
				break;
			case TYPE_FLOAT2:
				fp->writefa(n->find_attr("x"));
				fp->writefa(n->find_attr("y"));
				break;
			case TYPE_FLOAT3:
				fp->writefa(n->find_attr("x"));
				fp->writefa(n->find_attr("y"));
				fp->writefa(n->find_attr("z"));
				break;
			case TYPE_SPHERE:
				fp->writefa(n->find_attr("x"));
				fp->writefa(n->find_attr("y"));
				fp->writefa(n->find_attr("z"));
				fp->writefa(n->find_attr("r"));
				break;
			case TYPE_AABB:
				fp->writefa(n->find_attr("x0"));
				fp->writefa(n->find_attr("y0"));
				fp->writefa(n->find_attr("z0"));
				fp->writefa(n->find_attr("pad0"));
				fp->writefa(n->find_attr("x1"));
				fp->writefa(n->find_attr("y1"));
				fp->writefa(n->find_attr("z1"));
				fp->writefa(n->find_attr("pad1"));
				break;
			case TYPE_CYLINDER:
				fp->writefa(n->find_attr("x0"));
				fp->writefa(n->find_attr("y0"));
				fp->writefa(n->find_attr("z0"));
				fp->writefa(n->find_attr("pad0"));
				fp->writefa(n->find_attr("x1"));
				fp->writefa(n->find_attr("y1"));
				fp->writefa(n->find_attr("z1"));
				fp->writefa(n->find_attr("pad1"));
				fp->writefa(n->find_attr("r"));
				fp->writefa(n->find_attr("pad2"));
				fp->writefa(n->find_attr("pad3"));
				fp->writefa(n->find_attr("pad4"));
				break;
			case TYPE_RANGEF:
				fp->writefa(n->find_attr("s"));
				fp->writefa(n->find_attr("r"));
				break;
			case TYPE_RANGEU16:
				fp->write16a(n->find_attr("s"));
				fp->write16a(n->find_attr("r"));
				break;
			case TYPE_HERMITECURVE:
				fp->writefa(n->find_attr("x0"));
				fp->writefa(n->find_attr("x1"));
				fp->writefa(n->find_attr("x2"));
				fp->writefa(n->find_attr("x3"));
				fp->writefa(n->find_attr("x4"));
				fp->writefa(n->find_attr("x5"));
				fp->writefa(n->find_attr("x6"));
				fp->writefa(n->find_attr("x7"));
				fp->writefa(n->find_attr("y0"));
				fp->writefa(n->find_attr("y1"));
				fp->writefa(n->find_attr("y2"));
				fp->writefa(n->find_attr("y3"));
				fp->writefa(n->find_attr("y4"));
				fp->writefa(n->find_attr("y5"));
				fp->writefa(n->find_attr("y6"));
				fp->writefa(n->find_attr("y7"));
				break;
			default:
				printf("Error: Unsupported XFS parameter type: %s\n", n->name());
				return false;
			}

			n = n->next();
		}

		np = np->next();
	}

	if (np)
	{
		printf("Error: Unexpected: %s\n", np->name());
		return false;
	}

	long end = ftell(fp->fp);

	fseek(fp->fp, offset, SEEK_SET);
	fp->write32(end - offset);
	fseek(fp->fp, end, SEEK_SET);

	return true;
}

void XFS::txt2xfs(const char *filename)
{
	char path[PATH_MAX];
	snprintf(path, PATH_MAX, "%s.txt", filename);

	XFS xfs;
	xfs.header.magic = XFS_MAGIC;

	{
		File file(path, "r");
		if (!file.fp) return;

		INI ini;

		xfs.header.objDataNum = 0;
		uint32_t count = 0;
		uint32_t hash = 0;
		uint32_t int1 = 0;
		uint32_t num = 0;
		OBJECTDATA *o = NULL;
		PARAMETER *p = NULL;
		
		while (ini.parse(file))
		{
			if (ini.section)
			{
				if (count > xfs.header.objDataNum) break;
				count++;
				o = NULL;
				p = NULL;
				hash = 0;
				int1 = 0;
				num = 0;
			}
			else if (ini.name)
			{
				if (strcmp(ini.name, "version") == 0)
				{
					xfs.header.version = strtoul(ini.value, NULL, 0);
				}
				else if (strcmp(ini.name, "type") == 0)
				{
					xfs.header.type = strtoul(ini.value, NULL, 0);
				}
				else if (strcmp(ini.name, "count") == 0)
				{
					if (!xfs.objs)
					{
						uint32_t n = strtoul(ini.value, NULL, 0);
						xfs.header.objDataNum = n;
						xfs.objs = (OBJECTDATA **)calloc(n, sizeof(OBJECTDATA *));
					}
				}
				else if (strcmp(ini.name, "Hash") == 0)
				{
					hash = strtoul(ini.value, NULL, 0);
					if (o) o->hash = hash;
				}
				else if (strcmp(ini.name, "int1") == 0)
				{
					int1 = strtoul(ini.value, NULL, 0);
					if (o) o->int1 = int1;
				}
				else if (strcmp(ini.name, "Num") == 0)
				{
					if (xfs.objs && count > 0 && !xfs.objs[count-1])
					{
						uint32_t n = strtoul(ini.value, NULL, 0);
						o = (OBJECTDATA *)calloc(1, sizeof(OBJECTDATA) + sizeof(PARAMETER) * n);
						xfs.objs[count-1] = o;
						o->hash = hash;
						o->int1 = int1;
						o->num = n;
					}
				}
				else if (strcmp(ini.name, "Name") == 0)
				{
					if (o) 
					{
						if (num >= o->num)
						{
							p = NULL;
							continue;
						}

						p = o->param + num;
						num++;
						char *name = strdup(ini.value);
						p->name = name;
					}
				}
				else if (strcmp(ini.name, "Type") == 0)
				{
					if (p) p->type = name2type(ini.value);
				}
				else if (strcmp(ini.name, "Flags") == 0)
				{
					if (p) p->flags = strtoul(ini.value, NULL, 0);
				}
				else if (strcmp(ini.name, "Size") == 0)
				{
					if (p) p->size = strtoul(ini.value, NULL, 0);
				}
			}
		}
	}

	snprintf(path, PATH_MAX, "%s.xml", filename);

	xml::document doc("class");

	{
		File file(path, "r");
		if (!file.fp) return;

		if (!doc.parse(file))
		{
			printf("Error parsing: %s\n", filename);
			return;
		}
	}

	{
		uint32_t size = xfs.header.objDataNum * 16;

		for (unsigned i = 0; i < xfs.header.objDataNum; i++)
		{
			size += xfs.objs[i]->num * 24;
		}

		for (unsigned i = 0; i < xfs.header.objDataNum; i++)
		{
			OBJECTDATA *o1 = xfs.objs[i];

			for (unsigned j = 0; j < o1->num; j++)
			{
				PARAMETER *p1 = o1->param + j;

				for (unsigned k = 0; k <= i; k++)
				{
					OBJECTDATA *o2 = xfs.objs[k];
			
					for (unsigned l = 0; l < o2->num; l++)
					{
						PARAMETER *p2 = o2->param + l;

						if (p2->offset == 0) break;

						if (strcmp(p1->name, p2->name) == 0)
						{
							p1->offset = p2->offset;
							goto done;
						}
					}
				}

				p1->offset = size;
				size += strlen(p1->name) + 1;
			done:;
			}
		}

		xfs.header.objDataSize = (size + 3) & -4;
	}

	{
		File file(filename, "wb");
		if (!file.fp) return;

		fwrite(&xfs.header, sizeof(HEADER), 1, file);

		uint32_t offset = xfs.header.objDataNum * 4;

		for (unsigned i = 0; i < xfs.header.objDataNum; i++)
		{
			file.write32(offset);

			OBJECTDATA *o = xfs.objs[i];
			offset += 12 + o->num * 24;
		}

		for (unsigned i = 0; i < xfs.header.objDataNum; i++)
		{
			OBJECTDATA *o = xfs.objs[i];

			file.write32(o->hash);
			file.write32(o->int1);
			file.write32(o->num);

			for (unsigned j = 0; j < o->num; j++)
			{
				PARAMETER *p = o->param + j;

				file.write32(p->offset);
				file.write8(p->type);
				file.write8(p->flags);
				file.write16(p->size);
				file.write32(0);
				file.write32(0);
				file.write32(0);
				file.write32(0);
			}
		}

		for (unsigned i = 0; i < xfs.header.objDataNum; i++)
		{
			OBJECTDATA *o = xfs.objs[i];

			for (unsigned j = 0; j < o->num; j++)
			{
				PARAMETER *p = o->param + j;

				if (p->offset == offset)
				{
					char *name = p->name;
					size_t len = strlen(name) + 1;
					fwrite(name, len, 1, file);
					offset += len;
				}
			}
		}

		offset &= 3;
		if (offset)
		{
			uint32_t tmp = 0;
			fwrite(&tmp, 4 - offset, 1, file);
		}

		xfs.header.maxClassID = 0;

		if (!xfs.write_xml(&file, &doc))
		{
			remove(filename);
			return;
		}

		fseek(file, 0, SEEK_SET);
		fwrite(&xfs.header, sizeof(HEADER), 1, file);		
	}
}

}

int main(int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		char *s = strrchr(argv[i], '.');
		if (s)
		{
			if (strcmp(s, ".txt") == 0 ||
			    strcmp(s, ".xml") == 0)
				*s = 0;
		}

		XFS::txt2xfs(argv[i]);
	}

	return 0;
}
