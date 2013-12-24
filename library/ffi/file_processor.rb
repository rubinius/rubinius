require File.expand_path('../generators', __FILE__)

module Rubinius
module FFI

  # Processes a file containing Ruby code with blocks of FFI definitions
  # delimited by @@@. The blocks are replaced with Ruby code produced by
  # running the FFI generators contained in the blocks. For example:
  #
  #   module Something
  #     @@@
  #     constants do |c|
  #       c.include 'somefile.h'
  #
  #       c.const 'MAX'
  #       c.const 'MIN'
  #     end
  #     @@@
  #   end
  #
  # would be converted to:
  #
  #   module Something
  #     MAX = 1
  #     MIN = 2
  #   end
  #
  # assuming that
  # 
  #   #define MAX 1
  #   #define MIN 2
  #
  # was contained in the file 'something.h'.

  class FileProcessor

    def initialize(ffi_name, rb_name)
      @name = File.basename rb_name, '.rb'

      definitions = File.read ffi_name

      replacement = definitions.gsub(/^( *)@@@(.*?)@@@/m) do
        @constants = []
        @structs = []

        indent = $1
        line_count = $2.count("\n") + 1

        instance_eval $2

        lines = []
        @constants.each { |c| lines << c.to_ruby }
        @structs.each { |s| lines << s.generate_layout }

        # expand multiline blocks
        lines = lines.join("\n").split "\n"
        lines = lines.map { |line| indent + line }

        # preserve source line numbers in output
        padding = line_count - lines.length
        lines += [nil] * padding if padding >= 0

        lines.join "\n"
      end

      File.open rb_name, 'wb' do |f|
        f.puts "# This file is generated #{self.class} from #{ffi_name}."
        f.puts
        f.puts replacement
      end
    end

    def constants(options={}, &block)
      @constants << FFI::ConstGenerator.new(@name, options, &block)
    end

    def struct(&block)
      @structs << FFI::StructGenerator.new(@name, &block)
    end

    ##
    # Utility converter for constants

    def to_s
      proc { |obj| obj.to_s.inspect }
    end
  end
end
end
