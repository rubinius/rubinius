# -*- encoding: us-ascii -*-

class Time
  def inspect
    str = @is_gmt ? strftime("%Y-%m-%d %H:%M:%S UTC") : strftime("%Y-%m-%d %H:%M:%S %z")
    str.force_encoding Encoding::US_ASCII
  end
  alias_method :to_s, :inspect
end
