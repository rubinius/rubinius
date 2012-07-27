# -*- encoding: us-ascii -*-

class Range
  alias_method :===, :include?

  protected

  def can_iterate_from?(object)
    object.respond_to? :succ
  end
end
