class Regexp

  def self.allocate
    Ruby.primitive :regexp_allocate
    raise PrimitiveFailure, "Regexp.allocate primitive failed"
  end

  ##
  # See Regexp.new. This may be overridden by subclasses.

  def compile(pattern, opts)
    Ruby.primitive :regexp_initialize
    raise PrimitiveFailure,
          "regexp_new(#{str.inspect}, #{opts}, #{lang.inspect}) primitive failed"
  end

  private :compile

  def search_region(str, start, finish, forward) # equiv to MRI's re_search
    Ruby.primitive :regexp_search_region
    raise PrimitiveFailure, "Regexp#search_region primitive failed"
  end

  def match_start(str, offset) # equiv to MRI's re_match
    Ruby.primitive :regexp_match_start
    raise PrimitiveFailure, "Regexp#match_start primitive failed"
  end

  def search_from(str, offset) # equiv to MRI's rb_reg_search
    Ruby.primitive :regexp_search_from
    raise PrimitiveFailure, "Regexp#search_from primitive failed"
  end

  def options
    Ruby.primitive :regexp_options
    raise PrimitiveFailure, "Regexp#options primitive failed"
  end

  def self.last_match(field=nil)
    Ruby.primitive :regexp_last_match

    return last_match(Integer(field)) if field
    raise PrimitiveFailure, "Regexp#last_match failed"
  end

  def self.last_match=(match)
    Ruby.primitive :regexp_set_last_match

    unless match.kind_of? MatchData
      raise TypeError, "Expected MatchData, got #{match.inspect}"
    end

    raise PrimitiveFailure, "Regexp#set_last_match failed"
  end

  def self.propagate_last_match
    Ruby.primitive :regexp_propagate_last_match
    raise PrimitiveFailure, "Regexp#propagate_last_match failed"
  end

  def self.set_block_last_match
    Ruby.primitive :regexp_set_block_last_match
    raise PrimitiveFailure, "Regexp#set_block_last_match failed"
  end

end
