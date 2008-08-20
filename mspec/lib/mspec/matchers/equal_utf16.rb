class EqualUtf16Matcher
  def initialize(expected)
    @expected = expected
  end

  def matches?(actual)
    @actual = actual
    @actual == @expected || @actual == expected_swapped
  end

  def expected_swapped
    if @expected.respond_to?(:to_str)
      @expected_swapped ||= @expected.to_str.gsub(/(.)(.)/, '\2\1')
    else
      @expected_swapped ||= @expected.collect { |s| s.to_str.gsub(/(.)(.)/, '\2\1') }
    end
  end

  def failure_message
    ["Expected #{@actual.pretty_inspect}",
     "to equal #{@expected.pretty_inspect} or #{expected_swapped.pretty_inspect}"]
  end

  def negative_failure_message
    ["Expected #{@actual.pretty_inspect}",
     "not to equal #{@expected.pretty_inspect} nor #{expected_swapped.pretty_inspect}"]
  end
end

class Object
  def equal_utf16(expected)
    EqualUtf16Matcher.new(expected)
  end
end
