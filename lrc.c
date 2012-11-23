#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

struct entry {
	float 	timeval; 	/* seconds */
	char 	*lyric;

	struct entry *prev;
	struct entry *next;
};

int compar(const void *a, const void *b)
{
	struct entry *entry1 = *(struct entry **)a;
	struct entry *entry2 = *(struct entry **)b;

	if (entry1->timeval > entry2->timeval)
		return 1;
	if (entry1->timeval < entry2->timeval)
		return -1;
	return 0;
}

int 
main(void)
{
	int 	i;
	int 	mm;
	float 	ss;

	FILE 	*fp;
	char 	*line = NULL;
        size_t 	linecap = 0;
        ssize_t linelen;

	struct entry *head = NULL;
	struct entry *entry;
	size_t len = 0;

	fp = fopen("beyond.lrc", "rb");
	if (fp == NULL) {
		fprintf(stderr, "fopen error\n");
		return 1;
	}

	len = 0;
	while ((linelen = getline(&line, &linecap, fp)) > 0) {
		char *raw  = line;
		char *buff = malloc(linelen);
		int assigned;

		bzero(buff, linelen);
		while ((assigned = sscanf(raw, "[%d:%f]%[^\n]s\n", &mm, &ss, buff)) > 0) {
			if (head == NULL) {
				head = malloc(sizeof(struct entry));
				bzero(head, sizeof(struct entry));
				entry = head;
			} else {
				entry->next = malloc(sizeof(struct entry));
				bzero(entry->next, sizeof(struct entry));
				entry->next->prev = entry;
				entry = entry->next;
			}
			len += 1;
			entry->timeval = mm * 60.0f + ss;

			/* strip out empty lyric */
			if (assigned != 3)
				break;
			raw = buff;
		}
		struct entry *back;
		for (back = entry; back != NULL && back->lyric == NULL; back = back->prev)
			back->lyric = strdup(buff);
		free(buff);
	}
	free(line);

	/* dump double link list to vector array */
	struct entry **lyrics = malloc(sizeof(struct entry *) * len);
	for (i = 0, entry = head; entry != NULL; entry = entry->next, i++)
		lyrics[i] = entry;

	/* sort lyrics by time see compar function */
	qsort(lyrics, len, sizeof(struct entry *), compar); 

	/* output */
	for (i = 0; i < len; i++)
		printf("time: %f lyric: %s\n", lyrics[i]->timeval, lyrics[i]->lyric);

	/* cleanup */
	for (i = 0; i < len; i++) {
		free(lyrics[i]->lyric);
		free(lyrics[i]);
	}
	free(lyrics);
}
