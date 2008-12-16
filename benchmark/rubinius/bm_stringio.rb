###############################################################
# bench_string.rb
#
# Benchmark suite for the String class and all its methods.
###############################################################
require "benchmark"
require "stringio"

if ARGV[0] == "-e"
  @to_run = ARGV[1].split("||")
elsif ARGV[0] == "-x"
  @to_ignore = ARGV[1].split("||")
end

# While MAX is a sort of base loop count, all the benches
# have been scaled with factors to have approximately the
# same magnitude run time.
MAX = (ENV['TOTAL'] || 1_000).to_i

# Not a realistic string, but it is a random string with
# 3% punctuation, 5% digits, 8% upper case, 13% space,
# 71% lower case, shuffled to perfection, then overlayed
# with 4 \n just to confuse everything.
STRING = "rdnqsp uxq\nhnokjirs\nb c6rlh|4c@jcb av8\nPvunszwijhy lz  kdgy7hlKlR nzqxg\ndqldeg nm-yg vmnb mk gdrn  x"

Benchmark.bmbm do |x|

  @x = x

  def run(title, &blk)
    run_it = false
    run_it = true if @to_run && @to_run.include?(title)
    run_it = true if @to_ignore && !@to_ignore.include?(title)
    run_it = true if !@to_run && !@to_ignore

    @x.report(title, &blk) if run_it
  end

  run("base") {
    (MAX*1.0).to_i.times { STRING.dup }
  }

  run("StringIO.new") {
    (MAX*6.3).to_i.times { StringIO.new(STRING.dup) }
  }
  
  run("StringIO.open") {
    (MAX*6.3).to_i.times { StringIO.new(STRING.dup) }
  }
  
  run("StringIO.open with block") {
    (MAX*6.3).to_i.times { StringIO.new(STRING.dup) { |io| io } }
  }
  
  run("StringIO#<< when in append mode") {
    io = StringIO.new(STRING.dup, "a")
    string = STRING[0, STRING.size/4]
    (MAX*6.3).to_i.times { io << string }
  }

  run("StringIO#<<") {
    io = StringIO.new(STRING.dup)
    string = STRING[0, STRING.size/2]
    (MAX*6.3).to_i.times { io << string }
  }
  
  run("StringIO#each_byte") {
    io = StringIO.new(STRING.dup)
    (MAX*6.3).to_i.times {
      io.each_byte { |b| b }
      io.pos = 0
    }
  }

  run("StringIO#each") {
    io = StringIO.new(STRING.dup)
    (MAX*6.3).to_i.times { io.each_line { |line| line } }
  }
  
  run("StringIO#getc") {
    io = StringIO.new(STRING.dup)
    (MAX*13).to_i.times { io.getc; io.pos = 0 }
  }

  run("StringIO#gets") {
    io = StringIO.new(STRING.dup)
    (MAX*6.3).to_i.times { io.gets; io.pos = 0 }
  }
  
  run("StringIO#dup") {
    io = StringIO.new(STRING.dup)
    (MAX*6.3).to_i.times { io.dup }
  }

  run("StringIO#print") {
    io = StringIO.new(STRING.dup)
    string = STRING[0, STRING.size/2]
    (MAX*6.3).to_i.times { io.print(string) }
  }

  run("StringIO#print with Array") {
    io = StringIO.new(STRING.dup)
    ary = (1..10).to_a
    (MAX*2).to_i.times { io.print(ary) }
  }

  run("StringIO#print with multiple arguments") {
    io = StringIO.new(STRING.dup)
    ary = (1..10).to_a
    (MAX*2).to_i.times { io.print(*ary) }
  }
  
  run("StringIO#putc with Integer") {
    io = StringIO.new(STRING.dup)
    (MAX*6.3).to_i.times { io.putc(?A) }
  }

  run("StringIO#putc with String") {
    io = StringIO.new(STRING.dup)
    (MAX*6.3).to_i.times { io.putc("test") }
  }

  run("StringIO#puts") {
    io = StringIO.new(STRING.dup)
    string = STRING[0, STRING.size/2]
    (MAX*6.3).to_i.times { io.puts(string) }
  }

  run("StringIO#puts with Array") {
    io = StringIO.new(STRING.dup)
    ary = (1..10).to_a
    (MAX*2).to_i.times { io.puts(ary) }
  }

  run("StringIO#puts with multiple arguments") {
    io = StringIO.new(STRING.dup)
    ary = (1..10).to_a
    (MAX*2).to_i.times { io.puts(*ary) }
  }
  
  run("StringIO#read") {
    io = StringIO.new(STRING.dup)
    (MAX*13).to_i.times { io.read(100); io.pos = 0 }
  }

  run("StringIO#readchar") {
    io = StringIO.new(STRING.dup)
    (MAX*13).to_i.times { io.readchar; io.pos = 0 }
  }

  run("StringIO#readline") {
    io = StringIO.new(STRING.dup)
    (MAX*13).to_i.times { io.readline; io.pos = 0 }
  }

  run("StringIO#readlines") {
    io = StringIO.new(STRING.dup)
    (MAX*6.3).to_i.times { io.readlines; io.pos = 0 }
  }

  run("StringIO#write") {
    io = StringIO.new(STRING.dup)
    string = STRING[0, STRING.size/2]
    (MAX*13).to_i.times { io.write(string) }
  }
end