require 'benchmark'

total = (ENV['TOTAL'] || 100).to_i

strings = Dir["spec/**/*_spec.rb"]
`gcc -Wall #{File.dirname(__FILE__)}/stat.c -o #{File.dirname(__FILE__)}/stat`
Stat = File::Stat

Benchmark.bmbm do |x|
  x.report("loop") do
    total.times do |i|
      strings.each { |s| s }
    end
  end

  x.report("File::Stat.new") do
    total.times do |i|
      strings.each { |s| Stat.new s }
    end
  end
  
  x.report("File.stat") do
    total.times do |i|
      strings.each { |s| File.stat s }
    end
  end
  
  x.report("File.exist?") do
    total.times do |i|
      strings.each { |s| File.exist? s }
    end
  end
  
  x.report("File.file?") do
    total.times do |i|
      strings.each { |s| File.file? s }
    end
  end
    
  x.report("C stat (#{total * strings.size})") do
    `#{File.dirname(__FILE__)}/stat #{total * strings.size}`
  end
end
