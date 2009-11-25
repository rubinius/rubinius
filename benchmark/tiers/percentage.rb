require 'yaml'

file = ARGV.shift
compare_to = ARGV.shift

data = YAML.load File.read(file)

totals = Hash.new { |h,k| h[k] = 0 }

comp_total = 0

data.each do |run|
  file = run['file']
  runs = run['runs']

  unless comp_run = runs.delete(compare_to)
    raise "Unable to find: #{compare_to}"
  end

  comp = comp_run['median']

  comp_total += comp

  puts file
  puts comp
  runs.keys.sort.each do |key|
    median = runs[key]['median']
    perc = comp.to_f / median
    totals[key] += median
    puts "  #{key} #{perc} #{median}"
  end

  puts

end

p comp_total
p totals

totals.each do |key, val|
  puts "#{key} #{comp_total.to_f / val}"
end
