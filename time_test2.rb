ENV['TZ']="America/New_York"

puts "\n\n\ncalling Time.local"

t1 = Time.local(1801, 12, 31, 23, 59, 59)

puts "calling t1.wday"
puts t1.wday

