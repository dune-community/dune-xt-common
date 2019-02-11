# ~~~
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   René Fritze (2018)
# ~~~

name = 'This file is part of the dune-xt-common project:'
url = 'https://github.com/dune-community/dune-xt-common'
copyright_statement = 'Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.'
license = '''Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
          with "runtime exception" (http://www.dune-project.org/license.html)'''
prefix = '//'

include_patterns = ('*.cc', '*.cxx', '*.hh', '*.hxx', '*cmake_config.h.in', '*headercheck.cpp.in', '*config.h.cmake',
                    '*version.hh.in', '*.pbh')
exclude_patterns = ('*mathexpr.*', '*gtest-all.cc', '*.vcsetup*', '*gtest-all.cxx')
