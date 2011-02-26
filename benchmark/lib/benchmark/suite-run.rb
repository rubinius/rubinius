require 'benchmark/suite'
require 'optparse'

quiet = false

opts = OptionParser.new do |o|
  o.on "-q", "--quiet" do
    quiet = true
  end
end

opts.parse!

results = ARGV.shift

suite = Benchmark::Suite.create do |s|
  s.quiet! if quiet

  ARGV.each do |f|
    if File.directory?(f)
      more = Dir["#{f}/**/{bench,bm}_*.rb"]
      more.each do |x|
        s.run x
      end
    else
      s.run f
    end
  end
end

File.open(results, "w") { |f| f << Marshal.dump(suite) }
