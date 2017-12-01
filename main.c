#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

__attribute__ ((noreturn))
void panic(const char *msg) {
    fprintf(stderr, "PANIC: %s\n", msg);
    exit(0);
}

void expect(int cond, const char *msg) {
    assert(cond != 0);
    if (cond == 0) {
        panic(msg);
    }
}

int letter_index(char c) {
    expect(c >= 'a' && c <= 'z', "non-lowercase letter");
    return c - 'a';
}

struct Trie {
    // could store "complete" flag in lower bits of `links'
    struct Trie *links[26];
    uint8_t      compl[26];
};
typedef struct Trie Trie;

void Trie_init(Trie *trie) {
    memset(&trie->links[0], 0, sizeof(trie->links));
    memset(&trie->compl[0], 0, sizeof(trie->compl));
}

void Trie_insert(Trie *trie, const char *word) {
    int index;
    Trie *prev = NULL;
    for (; *word; ++word) {
        index = letter_index(*word);
        if (trie->links[index] == NULL) {
            trie->links[index] = malloc(sizeof(Trie));
            expect(trie->links[index] != NULL, "oom alloc trie link!");
        }
        prev = trie;
        trie = trie->links[index];
    }

    if (prev) {
        prev->compl[index] = 1;
    }
}

int Trie_contains(Trie *trie, const char *word) {
    int index;
    const Trie *prev = NULL;

    expect(*word, "null word");
    for (; *word; ++word) {
        index = letter_index(*word);
        if (trie->links[index] == NULL) {
            return 0;
        }
        prev = trie;
        trie = trie->links[index];
    }

    return prev && prev->compl[index];
}

void load_dictionary(Trie *trie, const char *dictionary) {
    FILE *f;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int i;
    int good;
    int num_words = 0;

    f = fopen(dictionary, "r");
    if (f == NULL) {
       exit(EXIT_FAILURE);
    }

    while ((read = getline(&line, &len, f)) != -1) {
        read -= 1;
        line[read] = 0; // remove newline character
        if (read < 3) {
            continue;
        }

        good = 1;
        for (i = 0; i < read; ++i) {
            if (line[i] >= 'a' && line[i] <= 'z') {
                continue;
            } else if (line[i] >= 'A' && line[i] <= 'Z') {
                line[i] += 'a' - 'A'; // convert to lowercase
            } else {
                good = 0;
                break;
            }
        }

        if (!good) {
            continue;
        }

        Trie_insert(trie, &line[0]);
        ++num_words;
    }

    free(line);
    fclose(f);

    printf("Read %d words.\n", num_words);
}

int main(int argc, char **argv) {
    // const char board[4][4] = {
    //     { 'T', 'L', 'B', 'I', },
    //     { 'E', 'D', 'E', 'S', },
    //     { 'P', 'A', 'N', 'G', },
    //     { 'P', 'T', 'H', 'S', },
    // };

    const char *dictionary = "words_alpha.txt";
    Trie trie;
    Trie_init(&trie);
    load_dictionary(&trie, dictionary);

    return 0;
}
