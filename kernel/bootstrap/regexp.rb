class Regexp
  ivar_as_index :__ivars__ => 0, :source => 1, :data => 2, :names => 3
  def __ivars__; @__ivars__ ; end
  def source   ; @source    ; end
  def data     ; @data      ; end
  def names    ; @names     ; end

  ValidKcode = [?n,?e,?s,?u]
  KcodeValue = [16,32,48,64]

  IGNORECASE  = 1
  EXTENDED    = 2
  MULTILINE   = 4
  OPTION_MASK = 7

  KCODE_ASCII = 0
  KCODE_NONE  = 16
  KCODE_EUC   = 32
  KCODE_SJIS  = 48
  KCODE_UTF8  = 64
  KCODE_MASK  = 112

  class << self
    def new(arg, opts=nil, lang=nil)
     if arg.is_a?(Regexp)
        opts = arg.options
        arg  = arg.source
      elsif opts.is_a?(Fixnum)
          opts = opts & (OPTION_MASK | KCODE_MASK)
      elsif opts
        opts = IGNORECASE
      else
        opts = 0
      end

      if !lang.nil? && !opts.nil? && lang.is_a?(String)
        opts &= OPTION_MASK
        idx   = ValidKcode.index(lang[0])
        opts |= KcodeValue[idx] if idx
      end

      __regexp_new__(arg, opts)
    end

    alias :compile :new

    def __regexp_new__(str, opts)
      Ruby.primitive :regexp_new
    end

    # FIXME - Optimize me using String#[], String#chr, etc.
    # Do away with the control-character comparisons.
    def escape(str)
      meta = %w![ ] { } ( ) | - * . \\ ? + ^ $ #!
      quoted = ""
      str.codepoints.each do |c|
        quoted << if meta.include?(c)
        "\\#{c}"
        elsif c == "\n"
        "\\n"
        elsif c == "\r"
        "\\r"
        elsif c == "\f"
        "\\f"
        else
          c
        end
      end
      quoted
    end
    alias_method :quote, :escape
  end

  def inspect
    "#<Regexp /#{self.source}/>"
  end

  def match(str)
    Ruby.primitive :regexp_match
  end
end

class MatchData
  ivar_as_index :__ivars__ => 0, :source => 1, :regexp => 2, :full => 3, :region => 4
  def __ivars__; @__ivars__ ; end
  def source   ; @source    ; end
  def regexp   ; @regexp    ; end
  def full     ; @full      ; end
  def region   ; @region    ; end
  
  def string
    @source
  end
  
  def begin(idx)
   return full.at(0) if idx == 0
   return @region.at(idx - 1).at(0)
  end

  def end(idx)
   return full.at(1) if idx == 0
   @region.at(idx - 1).at(1)
  end

  def offset(idx)
   out = []
   out << self.begin(idx)
   out << self.end(idx)
   return out
  end

  def length
   @region.fields + 1
  end

  def captures
    out = []
    @region.each do |tup|
      x = tup.at(0)
      y = tup.at(1)
      out << @source[x...y]
    end
    return out
  end

  def [](idx)
    if idx == 0
      x = full.at(0)
      y = full.at(1) - 1
      @source[x..y]
    else
      captures[idx.to_i - 1]
    end
  end
  
  def pre_match
    return "" if full.at(0) == 0
    nd = full.at(0) - 1
    @source[0..nd]
  end
  
  def post_match
    nd = @source.size - 1
    st = full.at(1)
    @source[st..nd]
  end
end

