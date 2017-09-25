#!/usr/bin/env python3

import sys
from os import path

from common import list_tele_ops, list_tele_mods, OP_C

if (sys.version_info.major, sys.version_info.minor) < (3, 6):
    raise Exception("need Python 3.6 or later")

THIS_FILE = path.realpath(__file__)
THIS_DIR = path.dirname(THIS_FILE)
OP_ENUM_H = path.abspath(path.join(THIS_DIR, "../src/ops/op_enum.h"))

HEADER_PRE = """// clang-format off

#ifndef _OP_ENUM_H_
#define _OP_ENUM_H_

// This file has been autogenerated by 'utils/op_enums.py'

"""
HEADER_POST = "#endif\n"


def make_ops():
    return [s[3:] for s in list_tele_ops()]


def make_mods():
    return [s[4:] for s in list_tele_mods()]


def make_enum(name, prefix, entries):
    entries = list(entries)    # make a copy
    entries.append("_LENGTH")  # add a final entry for length
    output = ""
    output += "typedef enum {\n"
    for e in entries:
        output += f"    {prefix}{e},\n"
    output += "}} {};\n\n".format(name)
    return output


def main():
    print("reading:    {}".format(OP_C))
    print("generating: {}".format(OP_ENUM_H))
    ops = make_ops()
    mods = make_mods()
    op_enum = make_enum("tele_op_idx_t", "E_OP_", ops)
    mod_enum = make_enum("tele_mod_idx_t", "E_MOD_", mods)
    header = HEADER_PRE + op_enum + mod_enum + HEADER_POST
    with open(OP_ENUM_H, "w") as g:
        g.write(header)


if __name__ == '__main__':
    main()