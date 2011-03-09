require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  string = "2011-Mar-08T10:09:08.467"

  x.report "gsub!" do |times|
    i = 0
    while i < times
      string.gsub!(/[^-+',.\/:@[:alnum:]\[\]\x80-\xff]+/n, ' ')
      i += 1
    end
  end

end
