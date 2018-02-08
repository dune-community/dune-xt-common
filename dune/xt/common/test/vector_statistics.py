
from dune.xt.codegen import typeid_to_typedef_name

vectortypes = ('std::vector<double>', 'Dune::FieldVector<double,3>',
               'Dune::XT::Common::FieldVector<double,3>',
               'Dune::DynamicVector<double>')

vectortypes = [(typeid_to_typedef_name(v), v) for v in vectortypes]

multi_out = { 'std.cc': {'vectortypes': vectortypes },
             'xt.cc': {'vectortypes': vectortypes },
             'common.cc': {'vectortypes': vectortypes }}
