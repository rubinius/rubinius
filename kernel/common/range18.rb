# -*- encoding: us-ascii -*-

class Range
  protected

  def can_iterate_from?(object)
    object.respond_to? :succ
  end
end