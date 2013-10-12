module Rubinius
module FFI
  module Generators
    class Types

      ##
      # Maps different C types to the C type representations we use

      TYPE_MAP = {
                 "char" => :char,
          "signed char" => :char,
        "__signed char" => :char,
        "unsigned char" => :uchar,

                 "short"     => :short,
          "signed short"     => :short,
          "signed short int" => :short,
        "unsigned short"     => :ushort,
        "unsigned short int" => :ushort,

                 "int" => :int,
          "signed int" => :int,
        "unsigned int" => :uint,

                 "long" => :long,
                 "long int" => :long,
          "signed long" => :long,
          "signed long int" => :long,
        "unsigned long" => :ulong,
        "unsigned long int" => :ulong,
        "long unsigned int" => :ulong,

                 "long long"     => :long_long,
                 "long long int" => :long_long,
          "signed long long"     => :long_long,
          "signed long long int" => :long_long,
        "unsigned long long"     => :ulong_long,
        "unsigned long long int" => :ulong_long,

        "char *" => :string,
        "void *" => :pointer,
      }

      def self.generate
        new.generate
      end

      def initialize
        @platform = Platform.new
      end

      def source(io)
        io.puts "#include <stdint.h>"
        io.puts "#include <sys/types.h>"
        unless @platform.windows?
          io.puts "#include <sys/socket.h>"
          io.puts "#include <sys/resource.h>"
        end
      end

      def prepare(name, target)
        # we have nothing to do in this stage
      end

      def process(target)
        "#{@platform.compiler} -E #{@platform.defines} #{target}"
      end

      def process_failed
        "Error generating C types"
      end

      def generate
        typedefs = BodyGuard.new(self, "ffi_types_generator", @platform).perform

        code = ""

        typedefs.split(/\n/).each do |type|
          # We only care about single line typedef
          next unless type =~ /typedef/
          # Ignore unions or structs
          next if type =~ /union|struct/

          # strip off the starting typedef and ending ;
          type.gsub!(/^(.*typedef\s*)/, "")
          type.gsub!(/\s*;\s*$/, "")

          parts = type.split(/\s+/)
          def_type   = parts.join(" ")

          # GCC does mapping with __attribute__ stuf, also see
          # http://hal.cs.berkeley.edu/cil/cil016.html section 16.2.7.  Problem
          # with this is that the __attribute__ stuff can either occur before or
          # after the new type that is defined...
          if type =~ /__attribute__/
            if parts.last =~ /__QI__|__HI__|__SI__|__DI__|__word__/

              # In this case, the new type is BEFORE __attribute__ we need to
              # find the final_type as the type before the part that starts with
              # __attribute__
              final_type = ""
              parts.each do |p|
                break if p =~ /__attribute__/
                final_type = p
              end
            else
              final_type = parts.pop
            end

            def_type = case type
                       when /__QI__/   then "char"
                       when /__HI__/   then "short"
                       when /__SI__/   then "int"
                       when /__DI__/   then "long long"
                       when /__word__/ then "long"
                       else                 "int"
                       end

            def_type = "unsigned #{def_type}" if type =~ /unsigned/
          else
            final_type = parts.pop
            def_type   = parts.join(" ")
          end

          if type = TYPE_MAP[def_type]
            code << "rbx.platform.typedef.#{final_type} = #{type}\n"
            TYPE_MAP[final_type] = TYPE_MAP[def_type]
          else
            # Fallback to an ordinary pointer if we don't know the type
            if def_type =~ /\*/
              code << "rbx.platform.typedef.#{final_type} = pointer\n"
              TYPE_MAP[final_type] = :pointer
            end
          end
        end

        code
      end
    end
  end

  TypesGenerator = Generators::Types
end
end
