namespace xml
{
	enum {
		XML_ERROR = 0,
		XML_START = 1,
		XML_ATTR = 2,
		XML_END = 3,
		XML_EOF = -1,
	};

	enum {
		BUF_SIZE = 256
	};

	class parser
	{
	public:
		char tag[BUF_SIZE];
		char attr[BUF_SIZE];
		char value[BUF_SIZE];

	private:
		FILE *fp;
		unsigned status;
		unsigned index;

		enum {
			TEXT,
			NAME,
			TAG,
			ATTR,
			EQ,
			VALD,
			VALS,
			SPACE,
			END,
			COM,
			COM1,
			COM2
		};
		
	public:
		parser(FILE *f)
		{
			fp = f;
			status = TEXT;
		}

		int parse()
		{
			int c;

			while ((c = fgetc(fp)) >= 0)
			{
				switch (status)
				{
				case TEXT:
					if (c == '<')
					{
						index = 0;
						status = NAME;
					}
					break;

				case NAME:
					switch (c)
					{
					case '>':
						status = TEXT;
						tag[index] = 0;
						if (tag[0] == '/')
							return XML_END;
						if (tag[0] != '?')
							return XML_START;
						break;
					case '<':
					case '=':
					case '"':
					case '\'':
						return XML_ERROR;
					case '/':
						if (index == 0)
						{
							tag[0] = '/';
							index = 1;
							break;
						}
						if (tag[0] == '/')
							return XML_ERROR;
						tag[index] = 0;
						status = END;
						return XML_START;
					case '-':
						if (index == 2 && tag[0] == '!' && tag[1] == '-')
						{
							status = COM;
							break;
						}
						tag[index] = '-';
						if (++index >= BUF_SIZE)
							return XML_ERROR;
						break;
					default:
						if (c > ' ')
						{
							tag[index] = c;
							if (++index >= BUF_SIZE)
								return XML_ERROR;
							break;
						}
						tag[index] = 0;
						if (tag[0] == '/')
						{
							status = END;
							break;
						}
						status = TAG;
						return XML_START;
					}
					break;

				case TAG:
					switch (c)
					{
					case '>':
						status = TEXT;
						break;
					case '<':
					case '=':
					case '"':
					case '\'':
						return XML_ERROR;
					case '/':
						if (tag[0] == '/')
							return XML_ERROR;
						status = END;
						break;
					case '?':
						if (tag[0] != '?')
							return XML_ERROR;
						status = END;
						break;
					default:
						if (c <= ' ') break;
						attr[0] = c;
						index = 1;
						status = ATTR;
					}
					break;

				case ATTR:
					switch (c)
					{
					case '>':
						status = TEXT;
						break;
					case '<':
					case '"':
					case '\'':
					case '/':
						return XML_ERROR;
					case '=':
						attr[index] = 0;
						index = 0;
						status = EQ;
						break;
					default:
						if (c <= ' ')
						{
							status = TAG;
							break;
						}
						attr[index] = c;
						if (++index >= BUF_SIZE)
							return XML_ERROR;
					}
					break;

				case EQ:
					switch (c)
					{
					case '"':
						status = VALD;
						break;
					case '\'':
						status = VALS;
						break;
					default:
						return XML_ERROR;
					}
					break;

				case VALD:
					if (c < ' ') return XML_ERROR;
					if (c == '"')
					{
						value[index] = 0;
						status = SPACE;
						return XML_ATTR;
					}
					value[index] = c;
					if (++index >= BUF_SIZE)
						return XML_ERROR;
					break;

				case VALS:
					if (c < ' ') return XML_ERROR;
					if (c == '\'')
					{
						value[index] = 0;
						status = SPACE;
						return XML_ATTR;
					}
					value[index] = c;
					if (++index >= BUF_SIZE)
						return XML_ERROR;
					break;

				case SPACE:
					switch (c)
					{
					case '>':
						status = TEXT;
						break;
					case '/':
						if (tag[0] == '/')
							return XML_ERROR;
						status = END;
						break;
					case '?':
						if (tag[0] != '?')
							return XML_ERROR;
						status = END;
						break;
					default:
						if (c > ' ') return XML_ERROR;
						status = TAG;
					}
					break;

				case END:
					if (c == '>')
					{
						status = TEXT;
						if (tag[0] != '?')
							return XML_END;
						break;
					}
					if (c > ' ') return XML_ERROR;
					break;

				case COM:
					if (c == '-') status = COM1;
					break;

				case COM1:
					if (c == '-')
					{
						status = COM2;
						break;
					}
					status = COM;
					break;

				case COM2:
					if (c == '>')
					{
						status = TEXT;
						break;
					}
					status = COM;
					break;
				}
			}

			if (status != TEXT)
				return XML_ERROR;

			return XML_EOF;
		}
	};

	class attribute
	{
	private:
		char *m_name;
		char *m_value;
		attribute *m_next;

	public:
		attribute(const char *name, const char *value)
		{
			m_name = strdup(name);
			m_value = strdup(value);
			m_next = NULL;
		}

		~attribute()
		{
			free(m_name);
			free(m_value);
			if (m_next) delete m_next;
		}

		const char *name() { return m_name; }
		const char *value() { return m_value; }
		attribute *next() { return m_next; }

		static void new_attr( const char *name, const char *value, attribute **ptr)
		{
			while (*ptr) ptr = &(*ptr)->m_next;
			*ptr = new attribute(name, value);
		}

		long toInt() { return strtoul(m_value, NULL, 0); }
		float toFloat() { return strtod(m_value, NULL); }
	};

	class node
	{
	private:
		char *m_name;
		node *m_next;
		node *m_child;
		attribute *m_attr;

		void indent(FILE *fp, unsigned n)
		{
			for (unsigned i = 0; i < n; i++)
			{
				fputc('\t', fp);
			}
		}

	public:
		node(const char *name)
		{
			m_name = strdup(name);
			m_next = NULL;
			m_child = NULL;
			m_attr = NULL;
		}

		~node()
		{
			free(m_name);
			if (m_child) delete m_child;
			if (m_next) delete m_next;
			if (m_attr) delete m_attr;
		}

		node *add_child(const char *name)
		{
			node **ptr = &m_child;
			while (*ptr) ptr = &(*ptr)->m_next;
			node *n = new node(name);
			*ptr = n;
			return n;
		}

		void add_attr(const char *name, const char *value)
		{
			attribute::new_attr(name, value, &m_attr);
		}

		void add_attr(const char *name, uint32_t value)
		{
			char str[16];
			snprintf(str, 16, "%u", value);
			attribute::new_attr(name, str, &m_attr);
		}

		void add_attr(const char *name, int32_t value)
		{
			char str[16];
			snprintf(str, 16, "%d", value);
			attribute::new_attr(name, str, &m_attr);
		}

		void add_attr(const char *name, float value)
		{
			char str[64];
			for (int i = 1; i < 48; i++)
			{
				snprintf(str, 64, "%.*f", i, value);
				if ((float)atof(str) == value)
					break;
			}
			attribute::new_attr(name, str, &m_attr);
		}

		attribute *find_attr(const char *name)
		{
			for (attribute *ptr = attr(); ptr; ptr = ptr->next())
			{
				if (strcmp(ptr->name(), name) == 0)
					return ptr;
			}
			
			return NULL;
		}

		const char *name() { return m_name; }
		node *next() { return m_next; }
		node *child() { return m_child; }
		attribute *attr() { return m_attr; }

		void print(FILE *fp, unsigned ind)
		{
			indent(fp, ind);
			fprintf(fp, "<%s", m_name);

			for (attribute *a = m_attr; a; a = a->next())
			{
				fprintf(fp, " %s=\"%s\"", a->name(), a->value());
			}

			if (!m_child)
			{
				fprintf(fp, "/>\n");
				return;
			}

			fprintf(fp, ">\n");

			unsigned in = ind + 1;

			for (node *n = m_child; n; n = n->next())
			{
				n->print(fp, in);
			}

			indent(fp, ind);
			fprintf(fp, "</%s>\n", m_name);			
		}

		bool parse(parser *p)
		{
			for (;;)
			{
				switch (p->parse())
				{
				case XML_START:
				{
					node *n = add_child(p->tag);
					if (!n->parse(p))
						return false;
					break;
				}
				case XML_ATTR:
					add_attr(p->attr, p->value);
					break;
				case XML_END:
				{
					char *s = p->tag;
					if (*s == '/') s++;
					if (strcmp(s, m_name))
						return false;
					return true;
				}
				default:
					return false;
				}
			}
		}
	};

	class document : public node
	{
	public:
		document(const char *name) : node(name) { }
		~document() { }

		void print(FILE *fp)
		{
			fprintf(fp, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
			node::print(fp, 0);
		}

		bool parse(FILE *fp)
		{
			parser p(fp);

			for (;;)
			{
				int rc = p.parse();
				switch (rc)
				{
				case XML_START:
					if (p.tag[0] == '?')
						break;
					if (strcmp(p.tag, name()))
						return false;
					if (!node::parse(&p))
						return false;
					break;

				case XML_EOF:
					return true;

				case XML_ERROR:
					return false;

				default:
					if (p.tag[0] == '?')
						break;
					return false;
				}
			}
		}
	};
}
