#include "c.h"

static char rcsid[] = "$Id$";

static void printtoken(void);
int errcnt   = 0;
int errlimit = 20;
char kind[] = {
#define xx(a,b,c,d,e,f,g) f,
#define yy(a,b,c,d,e,f,g) f,
#include "token.h"
};
int wflag;		/* != 0 to suppress warning messages */

void test(int tok, char set[]) {//感觉逻辑上和expect并没有任何区别！！！！
	if (t == tok)
		t = gettok();
	else {
		expect(tok);
		skipto(tok, set);
		if (t == tok)
			t = gettok();
	}
}
void expect(int tok) {// 测试必须的token，如果不是则报错。其实就是语法错误。
	if (t == tok)
		t = gettok();
	else {
		error("syntax error; found");
		printtoken();
		fprint(stderr, " expecting '%k'\n", tok);
	}
}
void error(const char *fmt, ...) {// 打印错误信息。
	va_list ap;

	if (errcnt++ >= errlimit) {
		errcnt = -1;
		error("too many errors\n");
		exit(1);
	}
	va_start(ap, fmt);
	if (firstfile != file && firstfile && *firstfile)
		fprint(stderr, "%s: ", firstfile);
	fprint(stderr, "%w: ", &src);
	vfprint(stderr, NULL, fmt, ap);
	va_end(ap);
}

void skipto(int tok, char set[]) {// 尝试跳到set制定的token,其实没有太大的意义，可以算是一种试图修复错误的举动，但是我觉得既然是错误，显示给用户就是最好的解决方案。
	int n;
	char *s;

	assert(set);
	for (n = 0; t != EOI && t != tok; t = gettok()) {
		for (s = set; *s && kind[t] != *s; s++)
			;
		if (kind[t] == *s)
			break;
		if (n++ == 0)
			error("skipping");
		if (n <= 8)
			printtoken();
		else if (n == 9)
			fprint(stderr, " ...");
	}
	if (n > 8) {
		fprint(stderr, " up to");
		printtoken();
	}
	if (n > 0)
		fprint(stderr, "\n");
}
/* fatal - issue fatal error message and exit */
int fatal(const char *name, const char *fmt, int n) {// 这个函数也没什么实际的意义，基本上就是调用error().
	print("\n");
	errcnt = -1;
	error("compiler error in %s--", name);
	fprint(stderr, fmt, n);
	exit(EXIT_FAILURE);
	return 0;
}

/* printtoken - print current token preceeded by a space */
static void printtoken(void) {// 这个函数也没什么实际的意义，基本上就是调用error().
	switch (t) {
	case ID: fprint(stderr, " `%s'", token); break;
	case ICON:
		fprint(stderr, " `%s'", vtoa(tsym->type, tsym->u.c.v));
		break;
	case SCON: {
		int i, n;
		if (ischar(tsym->type->type)) {
			char *s = tsym->u.c.v.p;
			n = tsym->type->size;
			fprint(stderr, " \"");
			for (i = 0; i < 20 && i < n && *s; s++, i++)
				if (*s < ' ' || *s >= 0177)
					fprint(stderr, "\\%o", *s);
				else
					fprint(stderr, "%c", *s);
		} else {	/* wchar_t string */
			unsigned int *s = tsym->u.c.v.p;
			assert(tsym->type->type->size == widechar->size);
			n = tsym->type->size/widechar->size;
			fprint(stderr, " L\"");
			for (i = 0; i < 20 && i < n && *s; s++, i++)
				if (*s < ' ' || *s >= 0177)
					fprint(stderr, "\\x%x", *s);
				else
					fprint(stderr, "%c", *s);
		}
		if (i < n)
			fprint(stderr, " ...");
		else
			fprint(stderr, "\"");
		break;
		}
	case FCON:
		fprint(stderr, " `%S'", token, (char*)cp - token);
		break;
	case '`': case '\'': fprint(stderr, " \"%k\"", t); break;
	default: fprint(stderr, " `%k'", t);
	}
}

/* warning - issue warning error message */
void warning(const char *fmt, ...) {// 跟error基本上一样！！
	va_list ap;

	va_start(ap, fmt);
	if (wflag == 0) {
		errcnt--;
		error("warning: ");
		vfprint(stderr, NULL, fmt, ap);
	}
	va_end(ap);
}
