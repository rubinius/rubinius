out = VM.load_library "extensions/test/test", "test"
unless out
  puts "Unable to load extension."
  exit 1
end

puts "Test 1 (primitive function):"

t = TestExtension.new
t.test_print

puts "\nTest 2 (call from C into ruby):"

class TestExtension
  def from_c
    puts "R* called from C into ruby!"
  end
end

puts "R* before call in ruby"
t.test_call
puts "R* after call in ruby"

puts "\nTest 3 (detect and convert segfault):"

puts "R* before segfault"
begin
  t.test_segfault
  puts "Segfault did not occur!"
  exit 1
rescue Object => e
  puts "R* detected segfault '#{e.message}'"
end

puts "R* after segfault"

puts "\nTest 4 (nested C calls)"

class TestExtension
  def from_c
    puts "R* call 1 from C to ruby"
    test_call2
    puts "R* return 1"
  end
  
  def from_c2
    puts "R* call 2 from C to ruby"
    test_call3
    puts "R* return 2"
  end
  
  def from_c3
    puts "R* call 3 from C to ruby"
    puts "R* return 3"
  end
end

puts "R* before call1"
t.test_call
puts "R* after call1"

class TestExtension
  def didit(arg)
    puts "R* woo! callback with arg => #{arg.inspect}" 
  end
end

puts "\nTest 5 (calling with symbol, then callback):"

puts "R* calling :didit with arg"
t.test_arg1(:didit, 12)
puts "R* finished arg call"

puts "\nTest 6 (call var args):"

out = t.test_vargs(:blah, "you suck")
if out != "you suck"
  puts "Return value wrong!"
  exit 1
end

puts "\nTest 7 (call var args, gets as array):"

p t.test_vargs2(:blah, 8, 12, "hello")
