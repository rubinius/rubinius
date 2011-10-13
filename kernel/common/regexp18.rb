class Regexp
  
  # Performs normal match and returns MatchData object from $~ or nil.
  def match(str)
    unless str
      Regexp.last_match = nil
      return nil
    end
    
    str = StringValue(str)

    Regexp.last_match = search_region(str, 0, str.size, true)
  end

  def ===(other)
    unless other.kind_of?(String)
      other = Rubinius::Type.check_convert_type other, String, :to_str
      unless other
        Regexp.last_match = nil
        return false
      end
    end

    if match = match_from(other, 0)
      Regexp.last_match = match
      true
    else
      Regexp.last_match = nil
      false
    end
  end
end
