#define XFS_MAGIC 5457496

class XFS
{
private:
	enum
	{
		TYPE_UNDEFINED,
		TYPE_CLASS,
		TYPE_CLASSREF,
		TYPE_BOOL,
		TYPE_U8,
		TYPE_U16,
		TYPE_U32,
		TYPE_U64,
		TYPE_S8,
		TYPE_S16,
		TYPE_S32,
		TYPE_S64,
		TYPE_F32,
		TYPE_F64,
		TYPE_STRING,
		TYPE_COLOR,
		TYPE_POINT,
		TYPE_SIZE,
		TYPE_RECT,
		TYPE_MATRIX,
		TYPE_VECTOR3,
		TYPE_VECTOR4,
		TYPE_QUATERNION,
		TYPE_PROPERTY,
		TYPE_EVENT,
		TYPE_GROUP,
		TYPE_PAGE_BEGIN,
		TYPE_PAGE_END,
		TYPE_EVENT32,
		TYPE_ARRAY,
		TYPE_PROPERTYLIST,
		TYPE_GROUP_END,
		TYPE_CSTRING,
		TYPE_TIME,
		TYPE_FLOAT2,
		TYPE_FLOAT3,
		TYPE_FLOAT4,
		TYPE_FLOAT3x3,
		TYPE_FLOAT4x3,
		TYPE_FLOAT4x4,
		TYPE_EASECURVE,
		TYPE_LINE,
		TYPE_LINESEGMENT,
		TYPE_RAY,
		TYPE_PLANE,
		TYPE_SPHERE,
		TYPE_CAPSULE,
		TYPE_AABB,
		TYPE_OBB,
		TYPE_CYLINDER,
		TYPE_TRIANGLE,
		TYPE_CONE,
		TYPE_TORUS,
		TYPE_ELLIPSOID,
		TYPE_RANGE,
		TYPE_RANGEF,
		TYPE_RANGEU16,
		TYPE_HERMITECURVE,
		TYPE_ENUMLIST,
		TYPE_FLOAT3x4,
		TYPE_LINESEGMENT4,
		TYPE_AABB4,
		TYPE_OSCILLATOR,
		TYPE_VARIABLE,
		TYPE_VECTOR2,
		TYPE_MATRIX33,
		TYPE_RECT3D_XZ,
		TYPE_RECT3D,
		TYPE_RECT3D_COLLISION,
		TYPE_PLANE_XZ,
		TYPE_RAY_Y,
		TYPE_POINTF,
		TYPE_SIZEF,
		TYPE_RECTF,
		TYPE_END
	};

	struct ClassMap {
		uint32_t hash;
		const char *name;
	};

	struct FieldMap {
		const char *name;
		const char *orig;
	};

	struct FieldMapList {
		uint32_t hash;
		const FieldMap *map;
	};

	static char const * const TypeNames[];
	static const ClassMap ClassNames[];
	static const FieldMapList FieldNames[];
	static const FieldMap FieldMap1330451113[];
	static const FieldMap FieldMap1605172203[];
	static const FieldMap FieldMap1627774278[];
	static const FieldMap FieldMap1888096271[];
	static const FieldMap FieldMap1978005645[];
	static const FieldMap FieldMap1579093917[];
	static const FieldMap FieldMap1300620249[];
	static const FieldMap FieldMap524638941[];
	static const FieldMap FieldMap76243545[];
	static const FieldMap FieldMap86208413[];
	static const FieldMap FieldMap1244844019[];
	static const FieldMap FieldMap764189656[];
	static const FieldMap FieldMap38370575[];
	static const FieldMap FieldMap633112065[];
	static const FieldMap FieldMap1824796761[];
	static const FieldMap FieldMap1993026462[];
	static const FieldMap FieldMap721796412[];
	static const FieldMap FieldMap131868259[];
	static const FieldMap FieldMap2053699689[];
	static const FieldMap FieldMap929317226[];
	static const FieldMap FieldMap1183544139[];
	static const FieldMap FieldMap645026908[];
	static const FieldMap FieldMap454865959[];
	static const FieldMap FieldMap1159273634[];
	static const FieldMap FieldMap690919773[];
	static const FieldMap FieldMap413936459[];
	static const FieldMap FieldMap88772364[];
	static const FieldMap FieldMap1170710895[];
	static const FieldMap FieldMap397909348[];
	static const FieldMap FieldMap836365466[];
	static const FieldMap FieldMap658115316[];
	static const FieldMap FieldMap654098115[];
	static const FieldMap FieldMap454147563[];
	static const FieldMap FieldMap1382752683[];
	static const FieldMap FieldMap1329873997[];
	static const FieldMap FieldMap1288888543[];
	static const FieldMap FieldMap284185119[];
	static const FieldMap FieldMap1828787693[];
	static const FieldMap FieldMap796117654[];
	static const FieldMap FieldMap1914873364[];
	static const FieldMap FieldMap499962494[];
	static const FieldMap FieldMap2119213033[];

	struct HEADER
	{
		uint32_t magic;
		uint16_t version;
		uint16_t type;
		uint32_t maxClassID;
		uint32_t objDataNum;
		uint32_t objDataSize;
	};

	struct PARAMETER
	{
		char *name;
		uint32_t offset;
		uint8_t type;
		uint8_t flags;
		uint16_t size;
	};

	struct OBJECTDATA
	{
		uint32_t hash;
		uint32_t int1;
		uint32_t num;
		PARAMETER param[];
	};

	class File
	{
	public:
		FILE *fp;

		File(const char *name, const char *mode)
		{
			fp = fopen(name, mode);
			if (!fp) perror(name);
		}

		~File()
		{
			if (fp) fclose(fp);
		}

		operator FILE *() { return fp; }

		void write8(uint8_t v) { fputc(v, fp); }
		void write16(uint16_t v) { fwrite(&v, 2, 1, fp); }
		void write32(uint32_t v) { fwrite(&v, 4, 1, fp); }
		void write64(uint64_t v) { fwrite(&v, 8, 1, fp); }
		void writef(float v) { fwrite(&v, 4, 1, fp); }

		void write8a(xml::attribute *a) { write8(a ? a->toInt() : 0); }
		void write16a(xml::attribute *a) { write16(a ? a->toInt() : 0); }
		void write32a(xml::attribute *a) { write32(a ? a->toInt() : 0); }
#if UINT64_MAX <= ULONG_MAX
		void write64a(xml::attribute *a) { write64(a ? a->toInt() : 0); }
#else
		void write64a(xml::attribute *a) { write64(a ? a->toLInt() : 0); }
#endif
		void writefa(xml::attribute *a) { writef(a ? a->toFloat() : 0); }

		void remove(const char *name)
		{
			fclose(fp);
			remove(name);
			fp = NULL;
		}
	};

	class Buffer
	{
	private:
		char *data;
		unsigned pos;

	public:
		Buffer(FILE *fp, size_t len)
		{
			data = (char *)malloc(len);
			fread(data, len, 1, fp);
			pos = 0;
		}

		~Buffer()
		{
			free(data);
		}

		uint8_t u8()
		{
			return data[pos++];
		}

		int8_t s8()
		{
			return data[pos++];
		}

		uint16_t u16()
		{
			uint16_t i;
			memcpy(&i, data + pos, 2);
			pos += 2;
			return i;
		}

		int16_t s16()
		{
			int16_t i;
			memcpy(&i, data + pos, 2);
			pos += 2;
			return i;
		}

		uint32_t u32()
		{
			uint32_t i;
			memcpy(&i, data + pos, 4);
			pos += 4;
			return i;
		}

		int32_t s32()
		{
			int32_t i;
			memcpy(&i, data + pos, 4);
			pos += 4;
			return i;
		}

		uint64_t u64()
		{
			uint64_t i;
			memcpy(&i, data + pos, 8);
			pos += 8;
			return i;
		}

		int64_t s64()
		{
			int64_t i;
			memcpy(&i, data + pos, 8);
			pos += 8;
			return i;
		}

		float f32()
		{
			float f;
			memcpy(&f, data + pos, 4);
			pos += 4;
			return f;
		}

		char *str()
		{
			char *s = data + pos;
			pos += strlen(s) + 1;
			return s;
		}

		bool peek()
		{
			uint32_t i;
			memcpy(&i, data + pos, 4);
			if (i) return true;
			pos += 4;
			return false;
		}

		void skip(unsigned o) { pos += o; }
		void seek(unsigned o) { pos = o; }
		char *get(unsigned o) { return data + o; }
	};

	HEADER header;
	OBJECTDATA **objs;

	static const char *type2name(unsigned type)
	{
		if (type < TYPE_END) return TypeNames[type];
		return "custom";
	}

	static unsigned name2type(const char *name)
	{
		for (unsigned i = 0; i < TYPE_END; i++)
		{
			if (strcmp(TypeNames[i], name) == 0)
				return i;
		}
		return 0;
	}

	static const char *hash2name(uint32_t hash)
	{
		for (unsigned i = 0; ClassNames[i].name; i++)
		{
			if (ClassNames[i].hash == hash)
				return ClassNames[i].name;
		}
		return NULL;
	}

	static const char *fieldname(uint32_t hash, const char *name)
	{
		for (unsigned i = 0; FieldNames[i].map; i++)
		{
			if (FieldNames[i].hash == hash)
			{
				const FieldMap *map = FieldNames[i].map;

				for (unsigned j = 0; map[j].name; j++)
				{
					if (strcmp(map[j].name, name) == 0)
						return map[j].orig;
				}
			}
		}
		return name;
	}

	XFS()
	{
		objs = NULL;
	}

	~XFS()
	{
		if (objs)
		{
			for (unsigned i = 0; i < header.objDataNum; i++)
			{
				OBJECTDATA *o = objs[i];
				if (o)
				{
					for (unsigned j = 0; j < o->num; j++)
					{
						char *n = o->param[j].name;
						if (n) free(n);
					}

					free(o);
				}
			}
			
			free(objs);
		}
	}

	bool build_xml(Buffer *, xml::node *, unsigned);
	bool write_xml(File *, xml::node *);

public:
	static void xfs2txt(const char *);
	static void txt2xfs(const char *);
};
