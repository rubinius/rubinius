require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  x.compare!
  x.report "Dir['']" do
    Dir['']
  end

  x.report "Dir[*]" do
    Dir["spec/core/string/*_spec.rb"]
  end

  x.report "Dir[**]" do
    Dir["spec/core/**/unknown_spec.rb"]
  end

  x.report "Dir[{}]" do
    # This is a pattern used in rubygems
    Dir["./lib/psych{,.rb,.bundle}"]
  end

  if defined? Rubinius
    patterns = Dir::Glob.compile("./lib/psych{,.rb,.bundle}")

    x.report "Dir[] - {} - precomp" do
      patterns.each do |x|
        Dir::Glob.run x
      end
    end
  end
end
