class RubiconTestCase < Test::Unit::TestCase
  
  def truth_table(method, *result)
    [false, true].each do |a|
      expected = result.shift
      assert_equal(expected, method.call(a))
      assert_equal(expected, method.call(a ? self : nil))
    end
  end
  
end