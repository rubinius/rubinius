class Regexp
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
