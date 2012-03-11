# -*- encoding: us-ascii -*-

module Comparable
  def ==(other)
    return true if equal?(other)

    begin
      unless comp = (self <=> other)
        return false
      end

      return Comparable.compare_int(comp) == 0
    rescue StandardError
      return false
    end
  end
end
