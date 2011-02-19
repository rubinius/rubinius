module Rubinius::JITCompiler
  module Analysis
    class Pass
      def initialize(debug=false)
        @debug = debug
      end

      def pass_name
        name = self.class.to_s
        pos = name.rindex(":")
        name[pos+1..-1]
      end

      def debug(str)
        return unless @debug
        STDERR.puts "#{pass_name}: #{str}"
      end

      def run(pc, blocks)
        blocks.each do |ip,blk|
          run_on_block pc, blk
        end
      end

      def after_run(pc)
      end
    end

    class PassCollection
      def initialize(show_changes=false)
        @passes = []
        @show_changes = show_changes
        @local_info = nil
      end

      attr_writer :local_info

      def consume_local_info
        li = @local_info
        @local_info = nil
        return li
      end

      def <<(pass)
        @passes << pass
      end

      def show_diff(diffs)
        diffs.each do |change|
          if change.unchanged?
            puts "  #{change.old_element.asm}"
          elsif change.action == "!"
            puts "- #{change.old_element.asm}"
            puts "+ #{change.new_element.asm}"
          elsif change.adding?
            puts "+ #{change.new_element.asm}"
          elsif change.deleting?
            puts "- #{change.old_element.asm}"
          else
            p change
          end
        end
      end

      def show_all_changes(blocks)
        save = {}
        blocks.each do |ip, blk|
          save[ip] = blk.instructions
        end

        yield

        require 'rubygems'
        require 'diff/lcs'

        blocks.keys.sort.each do |ip|
          blk = blocks[ip]

          if blk.users
            puts "#{ip}: # users=#{blk.users.map { |i| i.ip }.inspect}"
          else
            puts "#{ip}:"
          end

          insn = blk.instructions
          if insn == save[ip]
            blk.each do |i|
              puts "  #{i.asm}"
            end
          else
            diffs = Diff::LCS.sdiff(save[ip], blk.instructions)
            show_diff diffs
          end
        end
      end

      def run(blocks)
        @passes.each do |pass|
          if @show_changes
            puts "PASS CHANGES: #{pass.class}"
            show_all_changes(blocks) do
              pass.run self, blocks
            end
          else
            pass.run self, blocks
          end

          pass.after_run self
        end
      end
    end
  end
end

require 'rubinius/jit/analysis/redundent_load'
require 'rubinius/jit/analysis/local_info'
require 'rubinius/jit/analysis/math_locals'
