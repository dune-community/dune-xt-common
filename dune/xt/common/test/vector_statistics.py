
from dune.xt.codegen import typeid_to_typedef_name

vectortypes = ('std::vector<double>', 'Dune::FieldVector<double,3>',
               'Dune::XT::Common::FieldVector<double,3>',
               'Dune::DynamicVector<double>')

vectortypes = [(typeid_to_typedef_name(v), v) for v in vectortypes]
