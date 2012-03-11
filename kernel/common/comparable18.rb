# -*- encoding: us-ascii -*-

module Comparable
  def ==(other)
    return true if equal?(other)

    begin
      unless comp = (self <=> other)
        return nil
      end

      return Comparable.compare_int(comp) == 0
    rescue StandardError
      return nil
    end
  end
end
