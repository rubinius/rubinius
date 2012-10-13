# -*- encoding: us-ascii -*-

class EncodingError < StandardError
end

class Encoding
  class UndefinedConversionError < EncodingError
    attr_accessor :source_encoding_name
    attr_accessor :destination_encoding_name
    attr_accessor :source_encoding
    attr_accessor :destination_encoding
    attr_accessor :error_char

    private :source_encoding_name=
    private :destination_encoding_name=
    private :source_encoding=
    private :destination_encoding=
    private :error_char=
  end

  class InvalidByteSequenceError < EncodingError
    attr_accessor :source_encoding_name
    attr_accessor :destination_encoding_name
    attr_accessor :source_encoding
    attr_accessor :destination_encoding
    attr_accessor :error_bytes
    attr_accessor :readagain_bytes

    private :source_encoding_name=
    private :destination_encoding_name=
    private :source_encoding=
    private :destination_encoding=
    private :error_bytes=
    private :readagain_bytes=
    private :incomplete_input=

    def initialize(message)
      super(message)

      @incomplete_input = false
    end

    def incomplete_input?
      @incomplete_input
    end
  end

  class ConverterNotFoundError < EncodingError
  end

  class CompatibilityError < EncodingError
  end

  class Transcoding
    attr_accessor :source
    attr_accessor :target

    def inspect
      "#<#{super} #{source} to #{target}"
    end
  end

  class Converter
    attr_accessor :source_encoding
    attr_accessor :destination_encoding
    attr_accessor :replacement

    def self.allocate
      Rubinius.primitive :encoding_converter_allocate
      raise PrimitiveFailure, "Encoding::Converter.allocate primitive failed"
    end

    def self.asciicompat_encoding(string_or_encoding)
      encoding = Rubinius::Type.try_convert_to_encoding string_or_encoding

      return if not encoding or encoding.equal? undefined
      return if encoding.ascii_compatible?

      transcoding = TranscodingMap[encoding.name.upcase]
      return unless transcoding and transcoding.size == 1

      Encoding.find transcoding.keys.first.to_s
    end

    def initialize(from, to, options=undefined)
      @source_encoding = Rubinius::Type.coerce_to_encoding from
      @destination_encoding = Rubinius::Type.coerce_to_encoding to

      if options.kind_of? Fixnum
        @options = options
      elsif !options.equal? undefined
        options = Rubinius::Type.coerce_to options, Hash, :to_hash

        @options = 0
        unless options.empty?
          @options |= INVALID_REPLACE if options[:invalid] == :replace
          @options |= UNDEF_REPLACE if options[:undef] == :replace

          if options[:newline] == :universal or options[:universal_newline]
            @options |= UNIVERSAL_NEWLINE_DECORATOR
          end

          if options[:newline] == :crlf or options[:crlf_newline]
            @options |= CRLF_NEWLINE_DECORATOR
          end

          if options[:newline] == :cr or options[:cr_newline]
            @options |= CR_NEWLINE_DECORATOR
          end

          @options |= XML_TEXT_DECORATOR if options[:xml] == :text
          if options[:xml] == :attr
            @options |= XML_ATTR_CONTENT_DECORATOR
            @options |= XML_ATTR_QUOTE_DECORATOR
          end

          replacement = options[:replace]
        end
      else
        @options = 0
      end

      if replacement.nil?
        if @destination_encoding == Encoding::UTF_8
          @replacement = "\xef\xbf\xbd".force_encoding("utf-8")
        else
          @replacement = "?".force_encoding("us-ascii")
        end
      else
        @replacement = Rubinius::Type.coerce_to replacement, String, :to_str
      end

      source_name = @source_encoding.name.upcase.to_sym
      dest_name = @destination_encoding.name.upcase.to_sym
      found = false

      if entry = TranscodingMap[source_name]
        @convpath = [source_name]

        if encoding = entry[dest_name]
          @convpath << dest_name
          found = true
        else
          visited = {source_name => true}
          search = [entry]

          until search.empty?
            table = search.shift
            table.each do |key, _|
              next if visited.key? key
              visited[key] = true

              next unless entry = TranscodingMap[key]
              if encoding = entry[dest_name]
                @convpath << key << dest_name
                found = true
                break
              end

              search << entry
            end
          end
        end
      end

      unless found
        msg = "code converter not found (#{@source_encoding.name} to #{@destination_encoding.name}"
        raise ConverterNotFoundError, msg
      end

      @converters = []
      total = @convpath.size - 1
      i = 0
      while i < total
        entry = TranscodingMap[@convpath[i]][@convpath[i + 1]]

        if entry.kind_of? String
          require "19/encoding/converter/#{entry}"
          entry = TranscodingMap[@convpath[i]][@convpath[i + 1]]
        end

        @converters << entry

        i += 1
      end
    end

    def convert(str)
      str = StringValue(str)

      dest = ""
      status = primitive_convert str, dest, nil, nil, PARTIAL_INPUT

      if status == :invalid_byte_sequence or
         status == :undefined_conversion or
         status == :incomplete_input
        raise last_error
      end

      if status == :finished
        raise ArgumentError, "converter already finished"
      end

      if status != :source_buffer_empty
        raise RuntimeError, "unexpected result of Encoding::Converter#primitive_convert"
      end

      dest
    end

    def primitive_convert(source, target, offset=nil, size=nil, options=undefined)
      Rubinius.primitive :encoding_converter_primitive_convert

      if options.equal? undefined
        options = 0
      elsif !options.kind_of? Fixnum
        opts = Rubinius::Type.coerce_to options, Hash, :to_hash

        options = 0
        options |= PARTIAL_INPUT if opts[:partial_input]
        options |= AFTER_OUTPUT if opts[:after_output]
      end

      primitive_convert source, target, offset, size, options
    end

    def putback(maxbytes=nil)
      Rubinius.primitive :encoding_converter_putback

      putback maxbytes
    end

    def finish
      Rubinius.primitive :encoding_converter_finish
      raise PrimitiveFailure, "Encoding::Converter#finish primitive failed"
    end

    def last_error
      error = Rubinius.invoke_primitive :encoding_converter_last_error, self
      return if error.nil?

      error_string = error[:error_string].dump
      source_encoding_name = error[:source_encoding_name]
      destination_encoding_name = error[:destination_encoding_name]

      case error[:result]
      when :invalid_byte_sequence
        read_again_string = error[:read_again_string]
        if read_again_string
          msg = "#{error_string} followed by #{read_again_string.dump} on #{source_encoding_name}"
        else
          msg = "#{error_string} on #{source_encoding_name}"
        end

        exc = InvalidByteSequenceError.new msg
      when :incomplete_input
        msg = "incomplete #{error_string} on #{source_encoding_name}"

        exc = InvalidByteSequenceError.new msg
      when :undefined_conversion
        error_char = error_string
        if codepoint = error[:codepoint]
          error_string = "U+%04X" % codepoint
        end

        if source_encoding_name.to_sym == @source_encoding.name and
           destination_encoding_name.to_sym == @destination_encoding.name
          msg = "#{error_string} from #{source_encoding_name} to #{destination_encoding_name}"
        else
          msg = "#{error_string} to #{destination_encoding_name} in conversion from #{source_encoding_name}"
          transcoder = @converters.first
          msg << " to #{transcoder.target}"
        end

        exc = UndefinedConversionError.new msg
      end

      Rubinius.privately do
        exc.source_encoding_name = source_encoding_name
        src = Rubinius::Type.try_convert_to_encoding source_encoding_name
        exc.source_encoding = src unless src.equal? undefined

        exc.destination_encoding_name = destination_encoding_name
        dst = Rubinius::Type.try_convert_to_encoding destination_encoding_name
        exc.destination_encoding = dst unless dst.equal? undefined

        if error_char
          error_char.force_encoding src unless src.equal? undefined
          exc.error_char = error_char
        end
      end

      exc
    end

    def primitive_errinfo
      Rubinius.primitive :encoding_converter_primitive_errinfo
      raise PrimitiveFailure, "Encoding::Converter#primitive_errinfo primitive failed"
    end

    def convpath
      path = []
      a = 0
      b = @convpath.size - 1

      while a < b
        path << [Encoding.find(@convpath[a].to_s), Encoding.find(@convpath[a + 1].to_s)]
        a += 1
      end

      path << "xml_text_escape" if @options & XML_TEXT_DECORATOR != 0
      path << "xml_attr_content_escape" if @options & XML_ATTR_CONTENT_DECORATOR != 0
      path << "xml_attr_quote" if @options & XML_ATTR_QUOTE_DECORATOR != 0
      path << "universal_newline" if @options & UNIVERSAL_NEWLINE_DECORATOR != 0
      path << "crlf_newline" if @options & CRLF_NEWLINE_DECORATOR != 0
      path << "cr_newline" if @options & CR_NEWLINE_DECORATOR != 0

      path
    end

    def self.search_convpath(from, to, options=undefined)
      new(from, to, options).convpath
    end
  end

  def self.aliases
    aliases = {}
    EncodingMap.each do |n, r|
      index = r.last
      next unless index

      aname = r.first
      aliases[aname] = EncodingList[index].name if aname
    end

    aliases
  end

  def self.set_alias_index(name, obj)
    key = name.upcase.to_sym

    case obj
    when Encoding
      source_name = obj.name
    when nil
      EncodingMap[key][1] = nil
      return
    else
      source_name = StringValue(obj)
    end

    entry = EncodingMap[source_name.upcase.to_sym]
    raise ArgumentError, "unknown encoding name - #{source_name}" unless entry
    index = entry.last

    EncodingMap[key][1] = index
  end
  private_class_method :set_alias_index

  def self.default_external
    if @default_external.equal? undefined
      @default_external = find "external"
    end
    @default_external
  end

  def self.default_external=(enc)
    raise ArgumentError, "default external encoding cannot be nil" if enc.nil?

    set_alias_index "external", enc
    set_alias_index "filesystem", enc
    @default_external = undefined
  end

  def self.default_internal
    if @default_internal.equal? undefined
      @default_internal = find "internal"
    end
    @default_internal
  end

  def self.default_internal=(enc)
    set_alias_index "internal", enc
    @default_internal = undefined
  end

  def self.find(name)
    enc = Rubinius::Type.try_convert_to_encoding name
    return enc unless enc.equal? undefined

    raise ArgumentError, "unknown encoding name - #{name}"
  end

  def self.list
    EncodingList
  end

  def self.locale_charmap
    LocaleCharmap
  end

  def self.name_list
    EncodingMap.map do |n, r|
      index = r.last
      r.first or (index and EncodingList[index].name)
    end
  end

  def inspect
    "#<Encoding:#{name}#{" (dummy)" if dummy?}>"
  end

  def names
    entry = EncodingMap[name.upcase.to_sym]
    names = [name]
    EncodingMap.each do |k, r|
      aname = r.first
      names << aname if aname and r.last == entry.last
    end
    names
  end

  def _dump(depth)
    name
  end

  def self._load(name)
    find name
  end
end
