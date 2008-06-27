require 'tempfile'
require 'open3'

module FFI

  class ConstGenerator

    attr_reader :constants

    def initialize(prefix = nil)
      @includes = []
      @constants = {}
      @prefix = prefix

      if block_given? then
        yield self
        calculate
      end
    end

    def [](name)
      @constants[name].value
    end

    def const(name, format="%d", cast="", &converter)
      const = Constant.new(name, format, cast, converter)
      @constants[name.to_s] = const
      return const
    end

    def calculate
      binary = File.join Dir.tmpdir, "rb_const_gen_bin_#{Process.pid}"

      Tempfile.open("#{@prefix}.const_generator") do |f|
        f.puts "#include <stdio.h>"

        @includes.each do |inc|
          f.puts "#include <#{inc}>"
        end

        f.puts "#include <stddef.h>\n\n"
        f.puts "int main(int argc, char **argv)\n{"

        @constants.each_value do |const|
          f.puts <<-EOF
  #ifdef #{const.name}
  printf("#{const.name} #{const.format}\\n", #{const.cast}#{const.name});
  #endif
          EOF
        end

        f.puts "\n\treturn 0;\n}"
        f.flush

        output = `gcc -D_DARWIN_USE_64_BIT_INODE -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -x c -Wall -Werror #{f.path} -o #{binary} 2>&1`

        unless $?.success? then
          output = output.split("\n").map { |l| "\t#{l}" }.join "\n"
          raise "Compilation error generating constants #{@prefix}:\n#{output}"
        end
      end

      output = `#{binary}`
      File.unlink binary

      output.each_line do |line|
        line =~ /^(\S+)\s(.*)$/
        const = @constants[$1]
        const.value = $2
      end
    end

    def dump_constants(io)
      @constants.each do |name, constant|
        name = [@prefix, name].join '.'
        io.puts "#{name} = #{constant.converted_value}"
      end
    end

    def to_ruby
      @constants.map do |name, constant|
        "#{name} = #{constant.converted_value.inspect}"
      end.join "\n"
    end

    def include(i)
      @includes << i
    end

  end

  class ConstGenerator::Constant

    attr_reader :name, :format, :cast
    attr_accessor :value

    def initialize(name, format, cast, converter=nil)
      @name = name
      @format = format
      @cast = cast
      @converter = converter
      @value = nil
    end

    def converted_value
      if @converter
        @converter.call(@value)
      else
        @value
      end
    end

  end  

end
