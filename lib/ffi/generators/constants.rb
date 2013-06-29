module Rubinius
module FFI
  module Generators
    ##
    # Constants turns C constants into ruby values.

    class Constants

      class Constant
        attr_reader :name, :format, :cast
        attr_accessor :value

        def initialize(name, format, cast, ruby_name=nil, converter=nil)
          @name = name
          @format = format
          @cast = cast
          @ruby_name = ruby_name
          @converter = converter
          @value = nil
        end

        def value?
          @value != nil
        end

        def converted_value
          @converter ? @converter.call(@value) : @value
        end

        def ruby_name
          @ruby_name || @name
        end

        def to_ruby
          "#{ruby_name} = #{converted_value}"
        end
      end


      attr_reader :constants

      ##
      # Creates a new constant generator that uses +prefix+ as a name, and an
      # options hash.
      #
      # The only option is :required, which if set to true raises an error if a
      # constant you have requested was not found.
      #
      # When passed a block, #calculate is automatically called at the end of
      # the block, otherwise you must call it yourself.

      def initialize(prefix=nil, options={})
        @includes = []
        @include_dirs = []
        @constants = {}
        @prefix = prefix
        @platform = Platform.new

        @required = options[:required]

        if block_given?
          yield self
          calculate
        end
      end

      def [](name)
        @constants[name].value
      end

      ##
      # Request the value for C constant +name+.  +format+ is a printf format
      # string to print the value out, and +cast+ is a C cast for the value.
      # +ruby_name+ allows you to give the constant an alternate ruby name for
      # #to_ruby.  +converter+ or +converter_proc+ allow you to convert the
      # value from a string to the appropriate type for #to_ruby.

      def const(name, format=nil, cast=nil, ruby_name=nil, converter=nil, &block)
        format ||= '%ld'
        cast ||= '(long)'

        if block and converter
          raise ArgumentError, "Supply only converter or converter block"
        end

        converter = block if converter.nil?

        const = Constant.new name, format, cast, ruby_name, converter
        @constants[name.to_s] = const
        return const
      end

      def source(io)
        io.puts "#include <stdio.h>"

        @includes.each do |inc|
          io.puts "#include <#{inc}>"
        end

        io.puts "#include <stddef.h>\n\n"
        io.puts "int main(int argc, char **argv)\n{"

        @constants.each_value do |const|
          io.puts <<-EOF
  #ifdef #{const.name}
  printf("#{const.name} #{const.format}\\n", #{const.cast}#{const.name});
  #endif
          EOF
        end

        io.puts "\n\treturn 0;\n}"
      end

      def prepare(name, target)
        @platform.compile(@include_dirs, name, target)
      end

      def prepare_failed
        "Compilation error generating constants #{@prefix}"
      end

      def process(target)
        target
      end

      def process_failed
        "Error generating constants #{@prefix}"
      end

      def calculate
        output = BodyGuard.new(self, @prefix, @platform).perform

        output.each_line do |line|
          line =~ /^(\S+)\s(.*)$/
          const = @constants[$1]
          const.value = $2
        end

        missing_constants = @constants.reject { |_, c| c.value? }.keys

        if @required and not missing_constants.empty?
          raise "Missing required constants for #{@prefix}: #{missing_constants.join ', '}"
        end
      end

      def write_constants(io)
        @constants.each do |name, constant|
          io.print @prefix, "."
          io.puts constant.to_ruby
        end
      end

      ##
      # Outputs values for discovered constants.  If the constant's value was
      # not discovered it is not omitted.

      def to_ruby
        @constants.sort_by { |name,| name }.map do |name, constant|
          if constant.value?
            constant.to_ruby
          else
            "# #{name} not defined"
          end
        end.join "\n"
      end

      def include(i)
        @includes << i
      end

      def include_dir(i)
        @include_dirs << i
      end
    end

  end

  ConstGenerator = Generators::Constants
end
end
