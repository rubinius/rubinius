# -*- encoding: us-ascii -*-

class EncodingError < StandardError
end

class Encoding
  class UndefinedConversionError < EncodingError
  end

  class CompatibilityError < EncodingError
  end

  EncodingMap   = Rubinius::EncodingClass::EncodingMap
  EncodingList  = Rubinius::EncodingClass::EncodingList
  LocaleCharmap = Rubinius::EncodingClass::LocaleCharmap

  class Transcoder
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
    attr_accessor :convpath

    def initialize(from, to, options=undefined)
      @source_encoding = Rubinius::Type.coerce_to_encoding from
      @destination_encoding = Rubinius::Type.coerce_to_encoding to

      if options.kind_of? Fixnum
      elsif !options.equal? undefined
        options = Rubinius::Type.coerce_to options, Hash, :to_hash

        replacement = options[:replace]
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
    end

    def convert(str)
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
    find "external"
  end

  def self.default_external=(enc)
    raise ArgumentError, "default external encoding cannot be nil" if enc.nil?

    set_alias_index "external", enc
    set_alias_index "filesystem", enc
  end

  def self.default_internal
    find "internal"
  end

  def self.default_internal=(enc)
    set_alias_index "internal", enc
  end

  def self.find(name)
    key = StringValue(name).upcase

    EncodingMap.each do |n, r|
      if n.to_s.upcase == key
        index = r.last
        return index && EncodingList[index]
      end
    end

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

# TODO: This psuedo variable should represent a scripts encoding.
Object.send :define_method, :__ENCODING__ do
  Encoding.new
end
