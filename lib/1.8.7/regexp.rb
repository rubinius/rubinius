class Regexp
  def self.union(*patterns)
    if patterns.length == 1
      pat = patterns.first
      return Regexp.new(pat) unless pat.respond_to?(:to_ary)
      patterns = Type.coerce_to(pat, Array, :to_ary)
    end
    return /(?!)/ if patterns.empty?
    patterns.map! do |pat|
      pat = pat.to_s if pat.is_a? Regexp
      Type.coerce_to(pat, String, :to_str)
    end
    Regexp.new(patterns.join('|'))
  end
end