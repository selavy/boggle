#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

typedef int8_t s8;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

__attribute__ ((noreturn))
void panic(const char *msg) {
    fprintf(stderr, "PANIC: %s\n", msg);
    exit(0);
}

void expect(int cond, const char *msg) {
    // assert(cond != 0);
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

int Trie_contains(const Trie *trie, const char *word) {
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

    return prev && prev->compl[index] ? 2 : 1;
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

static u8 make_move(u8 x, u8 y) {
    return (x & 0x3) | ((y & 0x3) << 2);
}

static u8 get_x(u8 m) {
    return m & 0x3;
}

static u8 get_y(u8 m) {
    return (m >> 2) & 0x3;
}

static int get_location(u8 x, u8 y) {
    return y*4 + x;
}

static u16 set_used(u16 b, u8 x, u8 y) {
    return b | get_location(x, y);
}

static int is_used(u16 b, u8 x, u8 y) {
    return (b & get_location(x, y)) != 0;
}

struct Position {
    u8 move;
    // TODO: change Trie interface to take length, not null terminated string
    char word[17];
    u8 len;
    u16 used;
};
typedef struct Position Position;
#define STACKSZ (1 << 20)
static Position stack[STACKSZ];
static int stacklen = 0;
const s8 board[4][4] = {
    { 't', 'l', 'b', 'i', },
    { 'e', 'd', 'e', 's', },
    { 'p', 'a', 'n', 'g', },
    { 'p', 't', 'h', 's', },
};

static void add_move(u8 x, u8 y, const Position *p) {
    const u8 len = p->len;
    const u16 used = p->used;
    // printf("Adding move at (%u, %u) %u\n", x, y, len);
    stack[stacklen].move = make_move(x, y);
    memcpy(&stack[stacklen].word[0], &p->word[0], len);
    stack[stacklen].word[len] = board[y][x];
    stack[stacklen].len = len + 1;
    stack[stacklen].used = set_used(used, x, y);
    ++stacklen;
    // expect(stacklen < STACKSZ, "ran out of stack space!");
    if (!(stacklen < STACKSZ)) {
        printf("ran out of stack space: %d\n", stacklen);
        panic("ran out of stack space!");
    }
}

static void generate_moves(const Position *pos, const Trie *trie) {
    const u8 x = get_x(pos->move);
    const u8 y = get_y(pos->move);
    const u16 used = pos->used;
    // const u8 len = pos->len;
    // if (len == 16) {
    //     return;
    // }

    if (x > 0 && !is_used(used, x-1, y)) {
        add_move(x-1, y, pos);
    }
    if (x < 4 && !is_used(used, x+1, y)) {
        add_move(x+1, y, pos);
    }
    if (y > 0 && !is_used(used, x, y-1)) {
        add_move(x, y-1, pos);
    }
    if (y < 4 && !is_used(used, x, y+1)) {
        add_move(x, y+1, pos);
    }
    if (x > 0 && y > 0 && !is_used(used, x-1, y-1)) {
        add_move(x-1, y-1, pos);
    }
    if (x > 0 && y < 4 && !is_used(used, x-1, y+1)) {
        add_move(x-1, y+1, pos);
    }
    if (x < 4 && y > 0 && !is_used(used, x+1, y-1)) {
        add_move(x+1, y-1, pos);
    }
    if (x < 4 && y < 4 && !is_used(used, x+1, y+1)) {
        add_move(x+1, y+1, pos);
    }
}

int main(int argc, char **argv) {
    const char *dictionary = "words_alpha.txt";
    Trie trie;
    Trie_init(&trie);
    load_dictionary(&trie, dictionary);


    // for x in 0 .. 4:
    //   for y in 0 .. 4:
    u8 x = 0;
    u8 y = 2;
    Position pos;
    memset(&pos, 0, sizeof(pos));
    pos.move = make_move(x, y);
    pos.word[0] = board[y][x];
    pos.len = 1;
    pos.used = set_used(0, x, y);
    generate_moves(&pos, &trie);

    Position *cur;
    int result;
    while (stacklen > 0) {
        cur = &stack[--stacklen];
        result = Trie_contains(&trie, &cur->word[0]);
        if (result == 0) {
            continue;
        } else if (cur->len >= 3 && result == 2) {
            printf("Found word! %s", &cur->word[0]);
        }
        generate_moves(&pos, &trie);
    }

    // Position *p;
    // printf("Stack length: %d\n\n", stacklen);
    // for (int i = 0; i < stacklen; ++i) {
    //     p = &stack[i];
    //     printf("(%u,%u) %d => %s\n", get_x(p->move), get_y(p->move), p->len, &p->word[0]);
    // }

    return 0;
}
