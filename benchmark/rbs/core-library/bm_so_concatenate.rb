# $Id: strcat-ruby.code,v 1.4 2004/11/13 07:43:28 bfulgham Exp $
# http://www.bagley.org/~doug/shootout/
# based on code from Aristarkh A Zagorodnikov and Dat Nguyen
# Modified for the Ruby Benchmark Suite.

# TODO: This bench is so unrealistic as to be useless.
# Rewrite as a realistic bench.

inputs  = [5000, 10_000, 15_000]
strings = inputs.map { |n| "a" * n }
idx     = -1

Bench.run inputs do |n|
  base = strings[idx+=1]
  string = ''
  300_000.times do
    string << base
  end
  puts string.length
end
