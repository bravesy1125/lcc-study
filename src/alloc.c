/*
	分配编译器的内存。LCC的内存管理非常简单，下面会详细注释。
	主要是记住这三个函数的名字就可以了，因为实在没有什么特别需要研究的东西。
*/

#include "c.h"
struct block
{
	struct block *next;
	char *limit;
	char *avail;
};
union align
{
	long l;
	char *p;
	double d;
	int (*f)(void);
};
union header
{
	struct block b;
	union align a;
};

static struct block
	first[] = {{NULL}, {NULL}, {NULL}},
	*arena[] = {&first[0], &first[1], &first[2]};
static struct block *freeblocks;

void *allocate(unsigned long n, unsigned a)
{
	struct block *ap;

	assert(a < NELEMS(arena));
	assert(n > 0);
	ap = arena[a];
	n = roundup(n, sizeof(union align));
	while (n > ap->limit - ap->avail)
	{
		if ((ap->next = freeblocks) != NULL)
		{
			freeblocks = freeblocks->next;
			ap = ap->next;
		}
		else
		{
			unsigned m = sizeof(union header) + n + roundup(10 * 1024, sizeof(union align));
			ap->next = malloc(m);
			ap = ap->next;
			if (ap == NULL)
			{
				error("insufficient memory\n");
				exit(1);
			}
			ap->limit = (char *)ap + m;
		}
		ap->avail = (char *)((union header *)ap + 1);
		ap->next = NULL;
		arena[a] = ap;
	}
	ap->avail += n;
	return ap->avail - n;
}

void *newarray(unsigned long m, unsigned long n, unsigned a)
{ //基本上和allocate没区别，就是多做一个乘法。不过我觉得应该记住这个名字，以后用到的时候知道在这里，而且很简单！
	return allocate(m * n, a);
}

void deallocate(unsigned a)
{
	assert(a < NELEMS(arena));
	arena[a]->next = freeblocks;
	freeblocks = first[a].next;
	first[a].next = NULL;
	arena[a] = &first[a];
}
