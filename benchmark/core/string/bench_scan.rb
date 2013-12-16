  require 'benchmark'
  require 'benchmark/ips'

  long_file = File.join("..","file",'large.txt')
  long_text = File.new(long_file).read.downcase
  medium_text = long_text.slice(0,1000)
  short_text = long_text.slice(0,100)

  Benchmark.ips do |x|

    x.report "string scan (long length)" do |times|
      i = 0
      while i < times
         a = long_text.scan(/[a-z]+/)
         i += 1
      end
    end

    x.report "string scan (medium length)" do |times|
      i = 0
      while i < times
         a = medium_text.scan(/[a-z]+/)
         i += 1
      end
    end

    x.report "string scan (short length)" do |times|
      i = 0
      while i < times
         a = short_text.scan(/[a-z]+/)
         i += 1
      end
    end

  end
