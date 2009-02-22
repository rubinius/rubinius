class Compiler
  class BlockGrapher
    def initialize(entry, style=:full)
      @start_block = entry
      @style = style
    end

    def run(output)
      File.open(output, "w") do |f|
        f.puts "digraph G {"
        f.puts "  labeljust=l;"
        bi = Compiler::BlockExtractor::BlockInspector.new do |b|
          if @style == :full
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

        bi.run(@start_block)

        f.puts "}"
      end
    end
  end
end
