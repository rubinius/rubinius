# depends on: class.rb

class MethodTable

  def initialize()
    setup
  end

  def public_names
    filter_names :public
  end

  def private_names
    filter_names :private
  end

  def protected_names
    filter_names :protected
  end

  alias_method :to_a, :public_names

  def filter_names(filter)
    map do |name, meth|
      if meth.kind_of? RuntimePrimitive or
         (meth.kind_of?(Tuple) and meth.first == filter) then
        name
      end
    end.compact
  end

end

