class NumericSub < Numeric
  # Allow methods to be mocked
  def singleton_method_added(val)
  end
end