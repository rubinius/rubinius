require 'benchmark'
require 'benchmark/ips'

Benchmark.ips do |x|
  string = "aaaa|bbbbbbbbbbbbbbbbbbbbbbbbbbbb|cccccccccccccccccccccccccccccccccccc|dd|eeeeeeeeeeeeeeeeeeeeeeeeeeeeeee|ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff|gggggggggggggggggggggggggggggggggggggggggggggggggggg|hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh|i|j|k|l|m|n|ooooooooooooooooooooooooo|"

  x.report "matching delimiter" do
    string.split('|')
  end

  x.report "matching delimiter with negative limit" do
    string.split('|', -1)
  end

  x.report "mismatched delimiter" do
    string.split('.')
  end

  x.report "matching delimiter regexp" do
    string.split(/\|/)
  end

  x.report "mismatched delimiter regexp" do
    string.split(/\./)
  end

  irc_str = ":irc.malkier.net UID UqdTi59atgtYoV9NUKvE7qMOwG2Fl 1 1305135275 +x UqdTi59atgtYoV9NUKvE7qMOwG2Fl UqdTi59atgtYoV9NUKvE7qMOwG2Fl 127.0.0.1 XXX :fake omg"

  x.report "awk split (on spaces)" do
    irc_str.split(' ')
  end

  newline_string = "string with newline\n" * 100

  x.report "regexp split on newlines" do |times|
    i = 0
    while i < times
      newline_string.split(/\n/)
      i += 1
    end
  end

  no_newline_string = "string with newline" * 100

  x.report "regexp split with no newlines" do |times|
    i = 0
    while i < times
      newline_string.split(/\n/)
      i += 1
    end
  end

end
