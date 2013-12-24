require 'pp'

unless defined?(RUBY_ENGINE) and RUBY_ENGINE == 'rbx'
  $: << 'lib'
  require File.join(File.dirname(__FILE__), '..', 'compiler', 'mri_shim')
end

file = ARGV.shift
flags = []

puts "Graphing #{file}"

top = Rubinius::Compiler.compile_file_old(file, flags)

be = Compiler::BlockExtractor.new(top.iseq)

output = ARGV.shift || "blocks.dot"

puts "Writing graph to #{output}"

style = (ARGV.shift || "full").to_sym

entry = be.run

grapher = Compiler::BlockGrapher.new(entry, style)
grapher.run(output)

