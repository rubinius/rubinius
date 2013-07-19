# -*- encoding: us-ascii -*-

# TODO: Temporary! Object should find these methods in BasicObject
# but we fake that until method lookup is fixed for 1.9.
module Kernel
  def equal?(other)
    Rubinius.primitive :object_equal
    raise PrimitiveFailure, "Kernel#equal? primitive failed"
  end

  alias_method :eql?, :equal?
  alias_method :==,   :equal?
  alias_method :===,  :equal?

  def !~(other)
    res = self =~ other ? false : true
    Regexp.last_match = Regexp.last_match
    res
  end

  def trust
    Rubinius.primitive :object_trust
    raise PrimitiveFailure, "Kernel#trust primitive failed"
  end

  def untrust
    Rubinius.primitive :object_untrust
    raise PrimitiveFailure, "Kernel#untrust primitive failed"
  end

  def untrusted?
    Rubinius.primitive :object_untrusted_p
    raise PrimitiveFailure, "Kernel#untrusted? primitive failed"
  end

  def respond_to?(meth, include_private=false)
    respond_to_prim?(meth, include_private)
  end.custom_call_site

  def respond_to_missing?(meth, include_private)
    false
  end

end
