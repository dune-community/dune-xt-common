import os
import pprint


def parse_cache(filepath):
    import pyparsing as p
    EQ = p.Literal('=').suppress()
    DP = p.Literal(':').suppress()
    token = p.Word(p.alphanums+'_- ')
    line = p.Group(token('key') + DP + token('type') + EQ + p.restOfLine('value'))
    line.ignore(p.pythonStyleComment)
    line.ignore(p.dblSlashComment)
    grammar = p.OneOrMore(line)

    kv = {}
    types = {}
    for key, type, value in grammar.parseFile(filepath, parseAll=True):
        kv[key] = value.strip()
        types[key] = type
        if key.endswith('_DIR'):
            kv[key[:-4]] = os.path.isdir(value)
    return kv, types

if __name__ == '__main__':
    kv, types = parse_cache('/tmp/CMakeCache.txt')
    pprint.pprint(kv)