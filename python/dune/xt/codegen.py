# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Felix Schindler (2017)
#   Rene Milk       (2017)

def typeid_to_typedef_name(typeid, replacement='_'):
    """returns a sanitized typeid
    """
    illegal_chars = ['-', '>', '<', ':', ' ', ',', '+', '.']
    for ch in illegal_chars:
        typeid = typeid.replace(ch, replacement)
    return typeid


def is_found(cache, name):
    if name in cache.keys():
        return 'notfound' not in cache[name].lower()
    return false


def la_backends(cache):
    ret = []
    if is_found(cache, 'EIGEN3_INCLUDE_DIR'):
        ret.append('eigen_sparse')
    if is_found(cache, 'dune-istl_DIR'):
        ret.append('istl_sparse')
    return ret