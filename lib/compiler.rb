require Rubinius::PARSER_PATH

path = Rubinius::COMPILER_PATH
require path + "/compiler/compiler"
require path + "/compiler/stages"
require path + "/compiler/locals"
require path + "/compiler/ast"
require path + "/compiler/generator_methods"
require path + "/compiler/generator"
require path + "/compiler/iseq"
require path + "/compiler/opcodes"
require path + "/compiler/compiled_file"
require path + "/compiler/evaluator"
require path + "/compiler/printers"
require path + "/compiler/runtime"
