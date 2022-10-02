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

#include "xml.h"

#include "xfs.h"
#include "xfs_types.h"
#include "xfs_classes.h"
#include "xfs_fields.h"

bool XFS::build_xml(Buffer *buf, xml::node *node, unsigned index)
{
	if (index > header.objDataNum)
	{
		printf("Error: Invalid object during XFS conversion.\n");
		return false;
	}

	OBJECTDATA *o = objs[index];

	const char *name = hash2name(o->hash);
	if (name)
		node->add_attr("type", name);
	else
		node->add_attr("type", o->hash);

	for (unsigned i = 0; i < o->num; i++)
	{
		uint32_t count = buf->u32();
		PARAMETER *p = o->param + i;
		const char *fname = fieldname(o->hash, p->name);
		const char *tname = type2name(p->type);
		uint8_t is_array = p->flags & 0x20;

		xml::node *np = node;

		if (is_array)
		{
			np = np->add_child("array");
			np->add_attr("name", fname);
			np->add_attr("type", tname);
		}
		else if (count != 1)
		{
			printf("Error: bad count for object\n");
			return false;
		}

		for (unsigned j = 0; j < count; j++)
		{
			xml::node *n = np->add_child(tname);
			if (!is_array) n->add_attr("name", fname);

			switch (p->type)
			{
			case TYPE_CLASS:
			case TYPE_CLASSREF:
			{
				uint16_t type = buf->u16();
				buf->u16();

				if (type & 1)
				{
					buf->u32();

					if (!build_xml(buf, n, type >> 1))
						return false;
				}

				break;
			}
			case TYPE_BOOL:
				n->add_attr("value",  buf->u8() ? "true" : "false");
				break;
			case TYPE_U8:
				n->add_attr("value", buf->u8());
				break;
			case TYPE_U16:
				n->add_attr("value", buf->u16());
				break;
			case TYPE_U32:
				n->add_attr("value", buf->u32());
				break;
			case TYPE_S8:
				n->add_attr("value", buf->s8());
				break;
			case TYPE_S16:
				n->add_attr("value", buf->s16());
				break;
			case TYPE_S32:
				n->add_attr("value", buf->s32());
				break;
			case TYPE_F32:
				n->add_attr("value", buf->f32());
				break;
			case TYPE_STRING:
			case TYPE_CSTRING:
				n->add_attr("value", buf->str());
				break;
			case TYPE_MATRIX:
				n->add_attr("m0", buf->f32());
				n->add_attr("m1", buf->f32());
				n->add_attr("m2", buf->f32());
				n->add_attr("m3", buf->f32());
				n->add_attr("m4", buf->f32());
				n->add_attr("m5", buf->f32());
				n->add_attr("m6", buf->f32());
				n->add_attr("m7", buf->f32());
				n->add_attr("m8", buf->f32());
				n->add_attr("m9", buf->f32());
				n->add_attr("m10", buf->f32());
				n->add_attr("m11", buf->f32());
				n->add_attr("m12", buf->f32());
				n->add_attr("m13", buf->f32());
				n->add_attr("m14", buf->f32());
				n->add_attr("m15", buf->f32());
				break;
			case TYPE_VECTOR3:
				n->add_attr("x", buf->f32());
				n->add_attr("y", buf->f32());
				n->add_attr("z", buf->f32());
				buf->skip(4);
				break;
			case TYPE_VECTOR4:
				n->add_attr("x", buf->f32());
				n->add_attr("y", buf->f32());
				n->add_attr("z", buf->f32());
				n->add_attr("w", buf->f32());
				break;
			case TYPE_FLOAT2:
				n->add_attr("x", buf->f32());
				n->add_attr("y", buf->f32());
				break;
			case TYPE_FLOAT3:
				n->add_attr("x", buf->f32());
				n->add_attr("y", buf->f32());
				n->add_attr("z", buf->f32());
				break;
			case TYPE_SPHERE:
				n->add_attr("x", buf->f32());
				n->add_attr("y", buf->f32());
				n->add_attr("z", buf->f32());
				n->add_attr("r", buf->f32());
				break;
			case TYPE_AABB:
				n->add_attr("x0", buf->f32());
				n->add_attr("y0", buf->f32());
				n->add_attr("z0", buf->f32());
				buf->skip(4);
				n->add_attr("x1", buf->f32());
				n->add_attr("y1", buf->f32());
				n->add_attr("z1", buf->f32());
				buf->skip(4);
				break;
			case TYPE_CYLINDER:
				n->add_attr("x0", buf->f32());
				n->add_attr("y0", buf->f32());
				n->add_attr("z0", buf->f32());
				buf->skip(4);
				n->add_attr("x1", buf->f32());
				n->add_attr("y1", buf->f32());
				n->add_attr("z1", buf->f32());
				buf->skip(4);
				n->add_attr("r", buf->f32());
				buf->skip(4);
				n->add_attr("s", buf->f32());
				buf->skip(4);
				break;
			case TYPE_RANGEF:
				n->add_attr("s", buf->f32());
				n->add_attr("r", buf->f32());
				break;
			case TYPE_RANGEU16:
				n->add_attr("s", buf->u16());
				n->add_attr("r", buf->u16());
				break;
			case TYPE_HERMITECURVE:
				n->add_attr("x0", buf->f32());
				n->add_attr("x1", buf->f32());
				n->add_attr("x2", buf->f32());
				n->add_attr("x3", buf->f32());
				n->add_attr("x4", buf->f32());
				n->add_attr("x5", buf->f32());
				n->add_attr("x6", buf->f32());
				n->add_attr("x7", buf->f32());
				n->add_attr("y0", buf->f32());
				n->add_attr("y1", buf->f32());
				n->add_attr("y2", buf->f32());
				n->add_attr("y3", buf->f32());
				n->add_attr("y4", buf->f32());
				n->add_attr("y5", buf->f32());
				n->add_attr("y6", buf->f32());
				n->add_attr("y7", buf->f32());
				break;
			default:
				printf("Error: Unsupported XFS parameter type: %s\n", tname);
				return false;
			}
		}
	}

	return true;
}

void XFS::xfs2txt(const char *filename)
{
	File fp(filename, "rb");
	if (!fp.fp) return;

	XFS xfs;

	fread(&xfs.header, sizeof(HEADER), 1, fp);

	if (xfs.header.magic != XFS_MAGIC)
	{
		printf("%s: bad magic number\n", filename);
		return;
	}

	{
		Buffer buf(fp, xfs.header.objDataSize);

		xfs.objs = (OBJECTDATA **)calloc(xfs.header.objDataNum, sizeof(OBJECTDATA *));
		
		for (unsigned i = 0; i < xfs.header.objDataNum; i++)
		{
			buf.seek(i * 4);
			buf.seek(buf.u32());

			uint32_t hash = buf.u32();
			uint32_t int1 = buf.u32();
			uint32_t num = buf.u32();

			OBJECTDATA *o = (OBJECTDATA *)malloc(sizeof(OBJECTDATA) + sizeof(PARAMETER) * num);

			o->hash = hash;
			o->int1 = int1;
			o->num = num;

			for (unsigned j = 0; j < num; j++)
			{
				PARAMETER *p = o->param + j;
				char *name = strdup(buf.get(buf.u32()));
				p->name = name;
				p->type = buf.u8();
				p->flags = buf.u8();
				p->size = buf.u16();
				buf.skip(16);
			}

			xfs.objs[i] = o;
		}
	}


	char path[PATH_MAX];
	snprintf(path, PATH_MAX, "%s.txt", filename);

	{
		File file(path, "w");
		if (!file.fp) return;

		fprintf(file, "XFS (DD)\n");
		fprintf(file, "version = %u\n", xfs.header.version);
		fprintf(file, "type = %u\n", xfs.header.type);
		fprintf(file, "count = %u\n", xfs.header.objDataNum);

		for (unsigned i = 0; i < xfs.header.objDataNum; i++)
		{
			fprintf(file, "\n");
			OBJECTDATA *o = xfs.objs[i];

			const char *name = hash2name(o->hash);
			if (name)
				fprintf(file, "[%s]\n", name);
			else
				fprintf(file, "[STRUCT%04u]\n", i);

			fprintf(file, "Hash = %u\n", o->hash);
			fprintf(file, "int1 = %u\n", o->int1);
			fprintf(file, "Num = %u\n", o->num);

			for (unsigned j = 0; j < o->num; j++)
			{
				PARAMETER *p = o->param + j;
				const char *tname = type2name(p->type);

				fprintf(file, "Name = %s; Type = %s; Flags = %u; Size = %u\n", p->name, tname, p->flags, p->size);
			}
		}
	}

	snprintf(path, PATH_MAX, "%s.xml", filename);

	uint16_t tmp[2];
	fread(tmp, 4, 1, fp);
	
	if (tmp[0] & 1)
	{
		uint32_t len;
		fread(&len, 4, 1, fp);

		Buffer buf(fp, len - 4);

		xml::document doc("class");
		doc.add_attr("name", "XFS");

		if (xfs.build_xml(&buf, &doc, 0))
		{
			File file(path, "w");
			if (!file.fp) return;
			doc.print(file);
		}
	}
}

}

int main(int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		XFS::xfs2txt(argv[i]);
	}

	return 0;
}
