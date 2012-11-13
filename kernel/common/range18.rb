# -*- encoding: us-ascii -*-

class Range
  alias_method :===, :include?

  alias_method :first, :begin

  protected

  def can_iterate_from?(object)
    object.respond_to? :succ
  end
end
