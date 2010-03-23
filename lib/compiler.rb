base = File.expand_path "../", __FILE__

require base + "/melbourne"

require base + "/compiler/compiler"
require base + "/compiler/stages"
require base + "/compiler/locals"
require base + "/compiler/ast"
require base + "/compiler/generator_methods"
require base + "/compiler/generator"
require base + "/compiler/iseq"
require base + "/compiler/opcodes"
require base + "/compiler/compiled_file"
require base + "/compiler/evaluator"
require base + "/compiler/printers"
