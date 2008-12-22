require 'pp'

if defined?(RUBY_ENGINE) and RUBY_ENGINE == 'rbx'
  Object.const_set(:Compiler, Compile.compiler)
  require 'compiler/text'
else
  $: << 'lib'
  require File.join(File.dirname(__FILE__), '..', 'compiler', 'mri_shim')
end

require 'compiler/blocks'

file = ARGV.shift
flags = []

puts "Graphing #{file}"

top = Compiler.compile_file(file, flags)

be = Compiler::BlockExtractor.new(top.iseq)

output = ARGV.shift || "blocks.dot"

puts "Writing graph to #{output}"

style = (ARGV.shift || "full").to_sym

entry = be.run

File.open(output, "w") do |f|
  f.puts "digraph G {"
  f.puts "  labeljust=l;"
  bi = Compiler::BlockExtractor::BlockInspector.new do |b|
    if style == :full
      str = b.instructions.map do |i|
        if i.kind_of? Array
          i.join(" ")
        else
          i.to_s
        end
      end.join("\\n")

      f.puts "  b#{b.block_id} [shape=box, label=\"<block #{b.block_id} @ #{b.ip}>\\n#{str}\"];"
    else
      f.puts "  b#{b.block_id} [shape=box, label=\"block #{b.block_id}\"];"
    end

    b.next_blocks.each do |sub, reason|
      if reason == :condition
        f.puts "  b#{b.block_id} -> b#{sub.block_id} [style=dotted];"
      else
        f.puts "  b#{b.block_id} -> b#{sub.block_id};"
      end
    end
  end

  bi.run(entry)

  f.puts "}"
end

