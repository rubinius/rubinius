out = VM.load_library "extensions/subtend_test/subtend_test", "subtend_test"
unless out
  puts "Unable to load extension."
  exit 1
end
# require 'subtend_test'

def assert_equals(expected, actual)
  if expected == actual
    puts "+"
  else
    puts "- (#{expected.inspect} doesn't equal #{actual.inspect})"
  end
end

def assert_not_equals(expected, actual)
  if expected != actual
    puts "+"
  else
    puts "- (#{expected.inspect} equals #{actual.inspect})"
  end
end


@s = SubtendTest.new
assert_equals [], @s.new_array
assert_equals [], @s.new_array_of_size_two
assert_equals [4], @s.push_on_new_array(4)
# assert_equals [1,2,3,4], @s.push_on_array([1,2,3], 4)
s1 = "hi!"
s2 = @s.dup_string(s1)
assert_equals s1, s2
assert_not_equals s1.object_id, s2.object_id
assert_equals "what!?", @s.make_urgent!('what')