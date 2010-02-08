def results(what, file)
  str = `#{what} ./cps.rb #{file}`
  raise "Unable to execute #{what} on #{file}" unless str
  begin
    Integer(str.split("\n").last.split(" : ").last)
  rescue
    puts "ERROR"
    puts str
    return 0
  end
end

def compare(name, base, int)
  base_ips = Integer(base)
  int_ips = Integer(int)

  times_fast = "%.3f" % (int_ips.to_f / base_ips)

  puts "%10s %6sx #{int_ips}" % [name, times_fast]
end

def compute_files(arg)
  return [arg] if File.file?(arg)

  if arg.index(":")
    if arg.index("/")
      files = Dir["methods/#{arg[1..-1]}*.rb"]
    else
      files = Dir["methods/#{arg[1..-1]}/*.rb"]
    end
  elsif File.directory?(arg)
    files = Dir["#{arg}/*.rb"]
  else
    files = Dir[arg].reject { |i| !/\.rb$/.match(i) }
  end

  files.reject do |i|
    File.basename(i) == "helpers.rb" or File.read(i).index("not implemented") != nil
  end
end

def expand_int(int)
  case int
  when ":rbx"
    return "../../bin/rbx"
  else
    return int
  end
end

files = compute_files(ARGV.shift).sort
baseline_name = ARGV.shift
baseline_total = 0

totals = Hash.new { |h,k| h[k] = 0 }

files.each do |file|
  puts file
  baseline = results(baseline_name, file)

  baseline_total += baseline
  compare(baseline_name, baseline, baseline)

  ARGV.each do |int|
    int = expand_int(int)
    res = results(int, file)
    totals[int] += res
    compare(File.basename(int), baseline, res)
  end
end

puts "\nTOTAL:"

ARGV.each do |int|
  int = expand_int(int)
  res = totals[int]
  compare(File.basename(int), baseline_total, res)
end
