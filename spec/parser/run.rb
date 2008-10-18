#!bin/rbx
#
# A tiny spec-style runner for working on a parser.

def run(node, hash)
  src = hash['Ruby']
  expected = hash['RawParseTree']
  actual = src.to_sexp_pt '(string)', 1, false

  if actual != expected
    return false, "#{node}\n\n  #{src}\n\nexpected\n  #{actual.inspect}\n" \
                  "to equal\n  #{expected.inspect}\n\n"
  else
    return true, nil
  end
end

files = []
while x = ARGV.shift
  if File.file? x
    files << x
  elsif File.directory? x
    files.concat Dir["#{x}/**/*_spec.rb"]
  end
end

if files.empty?
  puts "No files given"
  puts "\nusage: run (FILE|DIRECTORY|GLOB)+"
  exit 1
end

report = []
total = failures = errors = 0

start = Time.now
files.each do |name|
  total += 1
  load name
  begin
    node = File.basename(name, ".rb").split("/").last
    status, output = run(node, test_case)
    if status
      print "."
    else
      print "F"
      failures += 1
      report << output
    end
  rescue Object => e
    print "E"
    errors += 1
    report << "#{name}\n#{e.inspect}\n#{e.backtrace.join("\n")}"
  end
end
finish = Time.now

unless report.empty?
  puts "\n"
  report.each_with_index do |e, i|
    puts "\n#{i+1})\n"
    puts e
  end
end

puts "\nFinished in #{finish-start} seconds"
puts "\n#{total} examples, #{failures} failures, #{errors} errors"
