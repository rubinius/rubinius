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

  mega = "/Users/evan/git/rbx-2.0/scratch/rails_bench/rbx-perf/app/views/layouts/perf{.en,.en,}{.html,.text.html,.text,.js,.css,.ics,.csv,.xml,.rss,.atom,.yaml,.multipart_form,.url_encoded_form,.json,}{.erb,.builder,.rxml,.rjs,.rhtml,}"

  x.report "Dir[mega]" do
    Dir[mega]
  end

  if defined? Rubinius
    patterns = Dir::Glob.compile("./lib/psych{,.rb,.bundle}")

    x.report "Dir[] - {} - precomp" do
      patterns.each do |x|
        Dir::Glob.run x
      end
    end

    x.report "Dir[mega] - compile only" do
      Dir::Glob.compile mega
    end

    patterns_mega = Dir::Glob.compile mega
    p patterns_mega.size

    x.report "Dir[mega] - precomp" do
      patterns_mega.each do |x|
        Dir::Glob.run x
      end
    end
  end
end
