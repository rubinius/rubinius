util_dir = File.expand_path "#{File.dirname(__FILE__)}/../utils"

tier = ARGV.shift

if File.file?(tier)
  files = [tier]
else
  files = Dir["#{tier}/*.rb"]
end

STDOUT.sync = true

def median(ary)
  sorted = ary.sort
  if ary.size & 1 == 1
    return sorted[ary.size / 2]
  else
    mid = ary.size / 2
    return (sorted[mid] + sorted[mid + 1]) / 2
  end
end

puts "---"

files.each do |file|
  puts "- file: #{file}"
  puts "  runs:"
  ARGV.each do |runtime|
    print "    #{File.basename runtime}: "
    str = `#{runtime} #{util_dir}/harness.rb #{file} 2>&1`
    if $?.exitstatus != 0
      if str.strip == "TIMEOUT"
        puts "TIMEOUT"
      else
        puts "ERROR"
        STDERR.puts str
      end
    else
      m = /TIME (.*)/.match(str)
      if m
        puts
        nums = m[1].split(" ").map { |i| i.to_i }
        puts "      median: #{median nums}"
        puts "      times: [#{m[1].split(" ").join(', ')}]"
      elsif str.strip == "TIMEOUT"
        puts "TIMEOUT"
      else
        puts "FAIL"
        STDERR.puts str
      end
    end
  end
end
