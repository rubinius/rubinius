# -*- encoding: us-ascii -*-

class Regexp

  def self.allocate
    Rubinius.primitive :regexp_allocate
    raise PrimitiveFailure, "Regexp.allocate primitive failed"
  end

  ##
  # See Regexp.new. This may be overridden by subclasses.

  def compile(pattern, opts)
    Rubinius.primitive :regexp_initialize
    raise PrimitiveFailure, "Regexp.compile(#{pattern.inspect}, #{opts}) primitive failed"
  end

  private :compile

  def search_region(str, start, finish, forward) # equiv to MRI's re_search
    Rubinius.primitive :regexp_search_region
    raise PrimitiveFailure, "Regexp#search_region primitive failed"
  end

  def match_start(str, offset) # equiv to MRI's re_match
    Rubinius.primitive :regexp_match_start
    raise PrimitiveFailure, "Regexp#match_start primitive failed"
  end

  def search_from(str, offset) # equiv to MRI's rb_reg_search
    Rubinius.primitive :regexp_search_from
    raise PrimitiveFailure, "Regexp#search_from primitive failed"
  end

  def options
    Rubinius.primitive :regexp_options
    raise PrimitiveFailure, "Regexp#options primitive failed"
  end

  def self.last_match(field=nil)
    Rubinius.primitive :regexp_last_match

    return last_match(Integer(field)) if field
    raise PrimitiveFailure, "Regexp#last_match primitive failed"
  end

  def self.last_match=(match)
    Rubinius.primitive :regexp_set_last_match

    unless match.kind_of? MatchData
      raise TypeError, "Expected MatchData, got #{match.inspect}"
    end

    raise PrimitiveFailure, "Regexp#set_last_match primitive failed"
  end

  def self.propagate_last_match
    Rubinius.primitive :regexp_propagate_last_match
    raise PrimitiveFailure, "Regexp#propagate_last_match primitive failed"
  end

  def self.set_block_last_match
    Rubinius.primitive :regexp_set_block_last_match
    raise PrimitiveFailure, "Regexp#set_block_last_match primitive failed"
  end

end
