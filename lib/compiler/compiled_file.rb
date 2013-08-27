# -*- encoding: us-ascii -*-

module Rubinius
  ##
  # A decode for the .rbc file format.

  class CompiledFile
    ##
    # Create a CompiledFile with +magic+ bytes, +signature+, and +version+.
    # The optional +stream+ is used to lazy load the body.

    def initialize(magic, signature, version, stream=nil)
      @magic = magic
      @signature = signature
      @version = version
      @stream = stream
      @data = nil
    end

    attr_reader :magic
    attr_reader :signature
    attr_reader :version
    attr_reader :stream

    ##
    # From a stream-like object +stream+ load the data in and return a
    # CompiledFile object.

    def self.load(stream)
      raise IOError, "attempted to load nil stream" unless stream

      magic = stream.gets.strip
      signature = Integer(stream.gets.strip)
      version = Integer(stream.gets.strip)

      return new(magic, signature, version, stream)
    end

    ##
    # Writes the CompiledFile +code+ to +file+.
    def self.dump(code, file, signature, version)
      File.open(file, "wb") do |f|
        new("!RBIX", signature, version).encode_to(f, code)
      end
    rescue SystemCallError
      # just skip writing the compiled file if we don't have permissions
    end

    ##
    # Encode the contets of this CompiledFile object to +stream+ with
    # a body of +body+. Body use marshalled using CompiledFile::Marshal

    def encode_to(stream, body)
      stream.puts @magic
      stream.puts @signature.to_s
      stream.puts @version.to_s

      mar = CompiledFile::Marshal.new
      stream << mar.marshal(body)
    end

    ##
    # Return the body object by unmarshaling the data

    def body
      return @data if @data

      mar = CompiledFile::Marshal.new
      @data = mar.unmarshal(stream)
    end

    ##
    # A class used to convert an CompiledCode to and from
    # a String.

    class Marshal

      ##
      # Read all data from +stream+ and invoke unmarshal_data

      def unmarshal(stream)
        if stream.kind_of? String
          str = stream
        else
          str = stream.read
        end

        @start = 0
        @size = str.size
        @data = str.data

        unmarshal_data
      end

      ##
      # Process a stream object +stream+ as as marshalled data and
      # return an object representation of it.

      def unmarshal_data
        kind = next_type
        case kind
        when 116 # ?t
          return true
        when 102 # ?f
          return false
        when 110 # ?n
          return nil
        when 73  # ?I
          return next_string.to_i(16)
        when 100 # ?d
          str = next_string.chop

          # handle the special NaN, Infinity and -Infinity differently
          if str[0] == ?\     # leading space
            x = str.to_f
            e = str[-5..-1].to_i

            # This is necessary because (2**1024).to_f yields Infinity
            if e == 1024
              return x * 2 ** 512 * 2 ** 512
            else
              return x * 2 ** e
            end
          else
            case str.downcase
            when "infinity"
              return 1.0 / 0.0
            when "-infinity"
              return -1.0 / 0.0
            when "nan"
              return 0.0 / 0.0
            else
              raise TypeError, "Invalid Float format: #{str}"
            end
          end
        when 115 # ?s
          enc = unmarshal_data
          count = next_string.to_i
          str = next_bytes count
          str.force_encoding enc if enc and defined?(Encoding)
          return str
        when 120 # ?x
          enc = unmarshal_data
          count = next_string.to_i
          str = next_bytes count
          str.force_encoding enc if enc and defined?(Encoding)
          return str.to_sym
        when 99  # ?c
          count = next_string.to_i
          str = next_bytes count
          return str.split("::").inject(Object) { |a,n| a.const_get(n) }
        when 112 # ?p
          count = next_string.to_i
          obj = Tuple.new(count)
          i = 0
          while i < count
            obj[i] = unmarshal_data
            i += 1
          end
          return obj
        when 105 # ?i
          count = next_string.to_i
          seq = InstructionSequence.new(count)
          i = 0
          while i < count
            seq[i] = next_string.to_i
            i += 1
          end
          return seq
        when 69  # ?E
          count = next_string.to_i
          name = next_bytes count
          return Encoding.find(name) if defined?(Encoding)
        when 77  # ?M
          version = next_string.to_i
          if version != 1
            raise "Unknown CompiledCode version #{version}"
          end
          code = CompiledCode.new
          code.metadata      = unmarshal_data
          code.primitive     = unmarshal_data
          code.name          = unmarshal_data
          code.iseq          = unmarshal_data
          code.stack_size    = unmarshal_data
          code.local_count   = unmarshal_data
          code.required_args = unmarshal_data
          code.post_args     = unmarshal_data
          code.total_args    = unmarshal_data
          code.splat         = unmarshal_data
          code.literals      = unmarshal_data
          code.lines         = unmarshal_data
          code.file          = unmarshal_data
          code.local_names   = unmarshal_data
          return code
        else
          raise "Unknown type '#{kind.chr}'"
        end
      end

      private :unmarshal_data

      ##
      # Returns the next character in _@data_ as a Fixnum.
      #--
      # The current format uses a one-character type indicator
      # followed by a newline. If that format changes, this
      # will break and we'll fix it.
      #++
      def next_type
        chr = @data[@start]
        @start += 2
        chr
      end

      private :next_type

      ##
      # Returns the next string in _@data_ including the trailing
      # "\n" character.
      def next_string
        count = @data.locate "\n", @start, @size
        count = @size unless count
        str = String.from_bytearray @data, @start, count - @start
        @start = count
        str
      end

      private :next_string

      ##
      # Returns the next _count_ bytes in _@data_, skipping the
      # trailing "\n" character.
      def next_bytes(count)
        str = String.from_bytearray @data, @start, count
        @start += count + 1
        str
      end

      private :next_bytes

      ##
      # For object +val+, return a String represetation.

      def marshal(val)
        case val
        when TrueClass
          "t\n"
        when FalseClass
          "f\n"
        when NilClass
          "n\n"
        when Fixnum, Bignum
          "I\n#{val.to_s(16)}\n"
        when String
          if defined?(Encoding)
            # We manually construct the Encoding data to avoid recursion
            # marshaling an Encoding name as a String.
            name = val.encoding.name
            enc_name = "E\n#{name.bytesize}\n#{name}\n"
          else
            # The kernel code is all US-ASCII. When building melbourne for 1.8
            # Ruby, we fake a bunch of encoding stuff so force US-ASCII here.
            enc_name = "E\n8\nUS-ASCII\n"
          end

          "s\n#{enc_name}#{val.bytesize}\n#{val}\n"
        when Symbol
          s = val.to_s
          if defined?(Encoding)
            # We manually construct the Encoding data to avoid recursion
            # marshaling an Encoding name as a String.
            name = s.encoding.name
            enc_name = "E\n#{name.bytesize}\n#{name}\n"
          else
            # The kernel code is all US-ASCII. When building melbourne for 1.8
            # Ruby, we fake a bunch of encoding stuff so force US-ASCII here.
            enc_name = "E\n8\nUS-ASCII\n"
          end

          "x\n#{enc_name}#{s.bytesize}\n#{s}\n"
        when Tuple
          str = "p\n#{val.size}\n"
          val.each do |ele|
            data = marshal(ele)
            data.force_encoding(str.encoding) if defined?(Encoding)
            str.append data
          end
          str
        when Float
          str = "d\n"
          if val.infinite?
            str.append "-" if val < 0.0
            str.append "Infinity"
          elsif val.nan?
            str.append "NaN"
          else
            str.append " %+.54f %5d" % Math.frexp(val)
          end
          str.append "\n"
        when InstructionSequence
          str = "i\n#{val.size}\n"
          val.opcodes.each do |op|
            unless op.kind_of?(Fixnum)
              raise TypeError, "InstructionSequence contains non Fixnum: #{op.inspect}"
            end
            str.append "#{op}\n"
          end
          str
        when CompiledCode
          str = "M\n1\n"
          str.append marshal(val.metadata)
          str.append marshal(val.primitive)
          str.append marshal(val.name)
          str.append marshal(val.iseq)
          str.append marshal(val.stack_size)
          str.append marshal(val.local_count)
          str.append marshal(val.required_args)
          str.append marshal(val.post_args)
          str.append marshal(val.total_args)
          str.append marshal(val.splat)
          str.append marshal(val.literals)
          str.append marshal(val.lines)
          str.append marshal(val.file)
          str.append marshal(val.local_names)
          str
        else
          if val.respond_to? :rbx_marshal_constant
            name = StringValue(val.rbx_marshal_constant)
            "c\n#{name.size}\n#{name}\n"
          else
            raise ArgumentError, "Unknown type #{val.class}: #{val.inspect}"
          end
        end
      end
    end
  end
end

