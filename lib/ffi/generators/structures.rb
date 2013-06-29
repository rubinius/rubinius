module Rubinius
module FFI
  module Generators
    ##
    # Generates an FFI Struct layout.
    #
    # Given the @@@ portion in:
    #
    #   module Zlib::ZStream < FFI::Struct
    #     @@@
    #     name "struct z_stream_s"
    #     include "zlib.h"
    #   
    #     field :next_in,   :pointer
    #     field :avail_in,  :uint
    #     field :total_in,  :ulong
    #   
    #     # ...
    #     @@@
    #   end
    #
    # Structures will create the layout:
    #
    #   layout :next_in, :pointer, 0,
    #          :avail_in, :uint, 4,
    #          :total_in, :ulong, 8,
    #          # ...
    #
    # StructGenerator does its best to pad the layout it produces to preserve
    # line numbers.  Place the struct definition as close to the top of the file
    # for best results.

    class Structures

      ##
      # A field in a Struct.

      class Field

        attr_reader :name
        attr_reader :type
        attr_reader :offset
        attr_accessor :size

        def initialize(name, type)
          @name = name
          @type = type
          @offset = nil
          @size = nil
        end

        def offset=(o)
          @offset = o
        end

        def to_config(name)
          buf = []
          buf << "rbx.platform.#{name}.#{@name}.offset = #{@offset}"
          buf << "rbx.platform.#{name}.#{@name}.size = #{@size}"
          buf << "rbx.platform.#{name}.#{@name}.type = #{@type}" if @type
          buf
        end
      end


      attr_accessor :size
      attr_reader   :fields

      def initialize(name)
        @name = name
        @struct_name = nil
        @includes = []
        @include_dirs = []
        @fields = []
        @found = false
        @size = nil
        @platform = Platform.new

        if block_given? then
          yield self
          calculate
        end
      end

      def source(io)
        io.puts "#include <stdio.h>"

        @includes.each do |inc|
          io.puts "#include <#{inc}>"
        end

        io.puts "#include <stddef.h>\n\n"
        io.puts "int main(int argc, char **argv)\n{"
        io.puts "  #{@struct_name} s;"
        io.puts %[  printf("sizeof(#{@struct_name}) %u\\n", (unsigned int) sizeof(#{@struct_name}));]

        @fields.each do |field|
          io.puts <<-EOF
    printf("#{field.name} %u %u\\n", (unsigned int) offsetof(#{@struct_name}, #{field.name}),
           (unsigned int) sizeof(s.#{field.name}));
  EOF
        end

        io.puts "\n  return 0;\n}"
      end

      def prepare(name, target)
        @platform.compile(@include_dirs, name, target)
      end

      def prepare_failed
        "Compilation error generating struct #{@name} (#{@struct_name})"
      end

      def process(target)
        target
      end

      def process_failed
        "Error generating struct #{@name} (#{@struct_name})"
      end

      def calculate
        raise "struct name not set" if @struct_name.nil?

        output = BodyGuard.new(self, @struct_name, @platform).perform.split "\n"

        sizeof = output.shift
        unless @size
          m = /\s*sizeof\([^)]+\) (\d+)/.match sizeof
          @size = m[1]
        end

        line_no = 0
        output.each do |line|
          md = line.match(/.+ (\d+) (\d+)/)
          @fields[line_no].offset = md[1].to_i
          @fields[line_no].size   = md[2].to_i

          line_no += 1
        end

        @found = true
      end

      def field(name, type=nil)
        field = Field.new(name, type)
        @fields << field
        return field
      end

      def found?
        @found
      end

      def write_config(io)
        io.puts "rbx.platform.#{@name}.sizeof = #{@size}"

        @fields.each { |field| io.puts field.to_config(@name) }
      end

      def generate_layout
        buf = ""

        @fields.each_with_index do |field, i|
          if buf.empty?
            buf << "layout :#{field.name}, :#{field.type}, #{field.offset}"
          else
            buf << "       :#{field.name}, :#{field.type}, #{field.offset}"
          end

          if i < @fields.length - 1
            buf << ",\n"
          end
        end

        buf
      end

      def get_field(name)
        @fields.find { |f| name == f.name }
      end

      def include(i)
        @includes << i
      end

      def include_dir(i)
        @include_dirs << i
      end

      def name(n)
        @struct_name = n
      end
    end
  end

  StructGenerator = Generators::Structures
end
end
