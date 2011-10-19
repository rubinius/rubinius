class BasicObject
  def method_missing(meth, *args)
    Kernel.raise NoMethodError, "Unable to send '#{meth}' on instance of BasicObject"
  end
  private :method_missing

  def singleton_method_added(name) end
  private :singleton_method_added

  def singleton_method_removed(name) end
  private :singleton_method_removed

  def singleton_method_undefined(name) end
  private :singleton_method_undefined

  def initialize(*args) end
  private :initialize
end
