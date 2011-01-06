class BasicObject
  def method_missing(*args) end
  private :method_missing

  def singleton_method_added(name) end
  private :singleton_method_added

  def singleton_method_removed(name) end
  private :singleton_method_removed

  def singleton_method_undefined(name) end
  private :singleton_method_undefined
end
