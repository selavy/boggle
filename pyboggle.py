#!/usr/bin/env python3

from collections import namedtuple


class TrieNode: 
    __slots__ = ('char', 'children', 'word_finished', 'counter',)
    def __init__(self, char):
        self.char = char
        self.children = []
        self.word_finished = False
        self.counter = 1
    

def trie_add(root, word):
    node = root
    for char in word:
        found_in_child = False
        for child in node.children:
            if child.char == char:
                child.counter += 1
                node = child
                found_in_child = True
                break
        if not found_in_child:
            new_node = TrieNode(char)
            node.children.append(new_node)
            node = new_node
    node.word_finished = True


def trie_find_prefix(root, prefix):
    node = root
    if not root.children:
        return False, 0, False
    for char in prefix:
        char_not_found = True
        for child in node.children:
            if child.char == char:
                char_not_found = False
                node = child
                break
        if char_not_found:
            return False, 0, False
    return True, node.counter, node.word_finished


Position = namedtuple('Position', ['word', 'used', 'move'])

stack = []
board = [
    [ 't', 'l', 'b', 'i', ],
    [ 'e', 'd', 'e', 's', ],
    [ 'p', 'a', 'n', 'g', ],
    [ 'p', 't', 'h', 's', ],
]


def make_move(x, y):
    return (x, y)


def get_x(m):
    return m[0]


def get_y(m):
    return m[1]


def get_location(x, y):
    return y*4 + x


def set_used(b, x, y):
    return b | get_location(x, y)


def is_used(b, x, y):
    return b & (1 << get_location(x, y)) != 0


def add_move(x, y, word, used, trie):
    word += board[y][x]
    found, nwords, _ = trie_find_prefix(trie, word)
    if not found:
        return
    # TODO(plesslie): can stop search on this branch if nwords == 1
    move = make_move(x, y)
    used = set_used(used, x, y)
    stack.append(Position(word=word, used=used, move=move))


def generate_moves(x, y, word, used, trie):
    def add(xx, yy):
        add_move(xx, yy, word, used, trie)

    if x > 0 and not is_used(used, x-1, y):
        add(x-1, y)
    if x < 3 and not is_used(used, x+1, y):
        add(x+1, y)
    if y > 0 and not is_used(used, x, y-1):
        add(x, y-1)
    if y < 3 and not is_used(used, x, y+1):
        add(x, y+1)
    if x > 0 and y > 0 and not is_used(used, x-1, y-1):
        add(x-1, y-1)
    if x > 0 and y < 3 and not is_used(used, x-1, y+1):
        add(x-1, y+1)
    if x < 3 and y > 0 and not is_used(used, x+1, y-1):
        add(x+1, y-1)
    if x < 3 and y < 3 and not is_used(used, x+1, y+1):
        add(x+1, y+1)
    

if __name__ == "__main__":
    trie = TrieNode("")
    nwords = 0
    with open('words_alpha.txt') as f:
        for line in f:
            word = line.rstrip().lower()
            if not word.isalpha():
                continue
            trie_add(trie, word)
            nwords += 1
    print(f"# Words: {nwords}")

    words = []
    for x in range(4):
        for y in range(4):
            generate_moves(x, y, word=board[x][y], used=0, trie=trie)
            while stack:
                cur = stack.pop()
                word = cur.word
                found, nwords, finished = trie_find_prefix(trie, cur.word)
                assert found is True
                if finished and len(word) >= 3:
                    print(f"Found word: {word}")
                    words.append(word)
                if nwords > 1:
                    generate_moves(x, y, word, cur.used, trie)
    print("Found {} words".format(len(words)))