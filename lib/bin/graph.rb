require 'pp'

if defined?(RUBY_ENGINE) and RUBY_ENGINE == 'rbx'
  Object.const_set(:Compiler, Compiler)
  require 'compiler/text'
else
  $: << 'lib'
  require File.join(File.dirname(__FILE__), '..', 'compiler', 'mri_shim')
end

require 'compiler/blocks'
require 'compiler/blocks_graph'

file = ARGV.shift
flags = []

puts "Graphing #{file}"

top = Compiler.compile_file(file, flags)

be = Compiler::BlockExtractor.new(top.iseq)

output = ARGV.shift || "blocks.dot"

puts "Writing graph to #{output}"

style = (ARGV.shift || "full").to_sym

entry = be.run

grapher = Compiler::BlockGrapher.new(entry, style)
grapher.run(output)

