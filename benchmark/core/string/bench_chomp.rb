require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  multiple_eol = "The quick brown fox jumps over the lazy dog.\r\n\r\n\n\n\r\n"
  no_eol = "The quick brown fox jumps over the lazy dog."
  multiple_lf = "The quick brown fox jumps over the lazy dog.\n\n\n\n\n\n"
  empty_string = ""

  x.report "chomp, std separator, multiple EOL" do |times|
    i = 0
    while i < times
      multiple_eol.chomp
      i += 1
    end
  end

  x.report "chomp, std separator, no EOL" do |times|
    i = 0
    while i < times
      no_eol.chomp
      i += 1
    end
  end

  x.report "chomp, std separator, multiple LF" do |times|
    i = 0
    while i < times
      multiple_lf.chomp
      i += 1
    end
  end

  x.report "chomp, std separator, empty string" do |times|
    i = 0
    while i < times
      empty_string.chomp
      i += 1
    end
  end
  
  x.report "chomp!, std separator, multiple EOL" do |times|
    i = 0
    while i < times
      multiple_eol.dup.chomp!
      i += 1
    end
  end

  x.report "chomp!, std separator, no EOL" do |times|
    i = 0
    while i < times
      no_eol.dup.chomp!
      i += 1
    end
  end

  x.report "chomp!, std separator, multiple LF" do |times|
    i = 0
    while i < times
      multiple_lf.dup.chomp!
      i += 1
    end
  end

  x.report "chomp!, std separator, empty string" do |times|
    i = 0
    while i < times
      empty_string.dup.chomp!
      i += 1
    end
  end

end
