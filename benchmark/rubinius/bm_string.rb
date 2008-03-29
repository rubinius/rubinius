###############################################################
# bench_string.rb
#
# Benchmark suite for the String class and all its methods.
###############################################################
require "benchmark"

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
    if @to_run
    run_it = true if @to_run.include?(title)
    end
    if @to_ignore
    run_it = true if !@to_ignore.include?(title)
    end
    run_it = true if !@to_run && !@to_ignore
    if run_it
    @x.report(title, &blk)
    end
  end

  run("base") {
    (MAX*1.0).to_i.times { STRING }
  }

  run("String.new") {
    (MAX*6.3).to_i.times { String.new(STRING) }
  }

  run("String#%") {
    string = "%05d"
    number = 123
    (MAX*2.6).to_i.times { string % number }
  }

  run("String#*") {
    (MAX*4.5).to_i.times { STRING * 3 }
  }

  run("String#+") {
    (MAX*5.6).to_i.times { STRING + STRING }
  }

  run("String#<<") {
    string = STRING.dup
    a = rand(STRING.size/2)
    string1 = string[a, STRING.size/2]
    (MAX*10.4).to_i.times { string << string1 }
  }

  run("String#<=>") {
    string1 = STRING
    string2 = STRING.reverse
    (MAX*14.2).to_i.times { string1 <=> string2 }
  }

  run("String#==") {
    string1 = STRING
    string2 = STRING.reverse
    (MAX*14.5).to_i.times { string1 == string2 }
  }

  run("String#===") {
    string1 = STRING
    string2 = STRING.reverse
    (MAX*10.8).to_i.times { string1 === string2 }
  }

  run("String#=~") {
    (MAX*5.6).to_i.times { STRING =~ /\w+/ }
  }

  run("String#[int]") {
    (MAX*7.5).to_i.times { STRING[rand(STRING.size)] }
  }

  run("String#[int,int]") {
    a = rand(STRING.size/2)
    (MAX*6.5).to_i.times { STRING[a, 15] }
  }

  run("String#[range]") {
    a = rand(STRING.size/2)
    b = a + 15
    (MAX*1.9).to_i.times { STRING[a..b] }
  }

  run("String#[regexp]") {
    (MAX*3.3).to_i.times { STRING[/\w+/] }
  }

  run("String#[regexp,int]") {
    (MAX*5.1).to_i.times { STRING[/\w+/, 5] }
  }

  run("String#[string]") {
    (MAX*6.2).to_i.times { STRING["lo"] }
  }

  # TODO: Fix
  #run("String#~") {
  #  string = "hello"
  #  (MAX*).to_i.times { ~ string }
  #}

  run("String#capitalize") {
    (MAX*3.5).to_i.times { STRING.capitalize }
  }

  run("String#capitalize!") {
    string = STRING
    (MAX*3.5).to_i.times { string.dup.capitalize! }
  }

  run("String#casecmp") {
    string1 = STRING
    string2 = STRING.reverse
    (MAX*17.9).to_i.times { string1.casecmp(string2) }
  }

  run("String#center") {
    a = STRING.size*2
    (MAX*7.2).to_i.times { STRING.center(a) }
  }

  run("String#chomp") {
    string = STRING + "\n"
    (MAX*7.2).to_i.times { string.chomp }
  }

  run("String#chomp!") {
    string = STRING + "\n"
    (MAX*4.1).to_i.times { string.dup.chomp! }
  }

  run("String#chop") {
    (MAX*6.9).to_i.times { STRING.chop }
  }

  run("String#chop!") {
    string = STRING
    (MAX*5.3).to_i.times { string.dup.chop! }
  }

  run("String#count(string)") {
    a = rand(STRING.size/2) - 5
    string = STRING[a,5]
    (MAX*3.9).to_i.times { STRING.count(string) }
  }

  run("String#count(^string)") {
    a = rand(STRING.size/2) - 5
    string = "^" + STRING[a,5]
    (MAX*3.9).to_i.times { STRING.count(string) }
  }

  run("String#crypt") {
    (MAX*0.3).to_i.times { STRING.crypt("sh") }
  }

  run("String#delete") {
    a = rand(STRING.size/2) - 15
    string = STRING[a, 15]
    (MAX*1.8).to_i.times { STRING.delete(string) }
  }

  run("String#delete!") {
    a = rand(STRING.size/2) - 15
    string = STRING[a, 15]
    (MAX*1.7).to_i.times { STRING.dup.delete!(string) }
  }

  run("String#downcase") {
    (MAX*4.4).to_i.times { STRING.downcase }
  }

  run("String#downcase!") {
    (MAX*3.5).to_i.times { STRING.dup.downcase! }
  }

  run("String#dump") {
    (MAX*1.5).to_i.times { STRING.dump }
  }

  run("String#dup") {
    (MAX*6.1).to_i.times { STRING.dup }
  }
  run("String#each") {
    (MAX*1.4).to_i.times { STRING.each { |e| } }
  }

  run("String#each_byte") {
    (MAX*0.2).to_i.times { STRING.each_byte { |e| } }
  }

  run("String#empty?") {
    (MAX*18.5).to_i.times { STRING.empty? }
  }

  run("String#eql?") {
    string1= STRING
    string2= STRING.reverse
    (MAX*17.7).to_i.times { string1.eql?(string2) }
  }

  run("String#gsub(regexp, repl)") {
    (MAX*0.6).to_i.times { STRING.dup.gsub(/[aeiou]/, '*') }
  }

  run("String#gsub(regexp) { block }") {
    (MAX*0.02).to_i.times { STRING.dup.gsub(/./) { |s| } }
  }

  run("String#gsub!(regexp) { block }") {
    (MAX*0.02).to_i.times { STRING.dup.gsub!(/./) { |s| } }
  }

  run("String#hex") {
    string = "0x"
    8.to_i.times  { string << rand(16).to_s(16) }
    (MAX*7.4).to_i.times { string.hex }
  }

  run("String#include?") {
    (MAX*6.3).to_i.times { STRING.include?("lo") }
  }

  run("String#index(string)") {
    (MAX*6.9).to_i.times { STRING.index("e") }
  }

  run("String#index(string, offset)") {
    a = rand(STRING.size/2)
    (MAX*8.8).to_i.times { STRING.index("e", -a) }
  }

  run("String#index(int)") {
    a = rand(STRING.size)
    (MAX*10.9).to_i.times { STRING.index(a) }
  }

  run("String#index(int, offset)") {
    a = rand(STRING.size/2)
    b = rand(STRING.size/2)
    (MAX*8.5).to_i.times { STRING.index(a, -b) }
  }

  run("String#index(regexp)") {
    (MAX*4.6).to_i.times { STRING.index(/[aeiou]/) }
  }

  run("String#index(regexp, offset)") {
    a = rand(STRING.size/2)
    (MAX*6.0).to_i.times { STRING.index(/[aeiou]/, -a) }
  }

  run("String#insert") {
    a = rand(STRING.size/2)
    (MAX*4.8).to_i.times { STRING.dup.insert(a, "world") }
  }

  run("String#inspect") {
    (MAX*1.2).to_i.times { STRING.inspect }
  }

  run("String#intern") {
    string = STRING * (((MAX*11.6).to_i / STRING.size) + 2)
    (MAX*0.1).to_i.times { string.chop!.intern }
  }

  run("String#length") {
    (MAX*18.4).to_i.times { STRING.length }
  }

  run("String#ljust") {
    a = STRING.size*2
    (MAX*7.4).to_i.times { STRING.ljust(a) }
  }

  run("String#lstrip") {
    string = (" " * 10) + STRING
    (MAX*5.8).to_i.times { string.lstrip }
  }

  run("String#lstrip!") {
    string = (" " * 10) + STRING
    (MAX*21.0).to_i.times { string.lstrip! }
  }

  run("String#match(regexp)") {
    (MAX*4.8).to_i.times { STRING.match(/lo/) }
  }

  run("String#match(string)") {
    (MAX*4.5).to_i.times { STRING.match("lo") }
  }

  run("String#oct") {
    string = "0"
    8.to_i.times  { string << rand(8).to_s }
    (MAX*13.0).to_i.times { string.oct }
  }

  run("String#replace") {
    string = ""
    8.to_i.times  { string << STRING[rand(STRING.size)] }
    (MAX*5.2).to_i.times { STRING.dup.replace(string) }
  }

  run("String#reverse") {
    (MAX*9.0).to_i.times { STRING.reverse }
  }

  run("String#reverse!") {
    (MAX*5.5).to_i.times { STRING.dup.reverse! }
  }

  run("String#rindex(string)") {
    (MAX*6.4).to_i.times { STRING.rindex("e") }
  }

  run("String#rindex(string, int)") {
    a = rand(STRING.size/2)
    (MAX*6.8).to_i.times { STRING.rindex("e", a) }
  }

  run("String#rindex(int, int)") {
    a = rand(STRING.size/2)
    b = rand(STRING.size/2)
    (MAX*12.0).to_i.times { STRING.rindex(a, b) }
  }

  run("String#rindex(regexp)") {
    (MAX*4.5).to_i.times { STRING.rindex(/[aeiou]/) }
  }

  run("String#rindex(regexp, int)") {
    a = rand(STRING.size/2)
    (MAX*6.3).to_i.times { STRING.rindex(/[aeiou]/, a) }
  }

  run("String#rjust(width)") {
    a = STRING.size*2
    (MAX*4.5).to_i.times { STRING.rjust(a) }
  }

  run("String#rjust(width, padding)") {
    a = STRING.size*2
    (MAX*4.8).to_i.times { STRING.rjust(a, "-") }
  }

  run("String#rstrip") {
    string = STRING + (" " * 10)
    (MAX*9.3).to_i.times { string.rstrip }
  }

  run("String#rstrip!") {
    string = STRING + (" " * 10)
    (MAX*20.1).to_i.times { string.rstrip! }
  }

  run("String#scan") {
    (MAX*0.3).to_i.times { STRING.scan(/\w+/) }
  }

  run("String#scan { block }") {
    (MAX*0.1).to_i.times { STRING.scan(/\w+/) { |w| } }
  }

  run("String#slice(int)") {
    a = rand(STRING.size/2)
    (MAX*7.1).to_i.times { STRING.dup.slice(a) }
  }

  run("String#slice(int, int)") {
    a = rand(STRING.size/2)
    b = rand(STRING.size/2)
    (MAX*9.7).to_i.times { STRING.slice(a, b) }
  }

  run("String#slice(range)") {
    a = rand(STRING.size/2)
    b = rand(STRING.size/2)
    (MAX*2.0).to_i.times { STRING.slice(a..b) }
  }

  run("String#slice(regexp)") {
    (MAX*6.9).to_i.times { STRING.slice(/ell/) }
  }

  run("String#slice(string)") {
    (MAX*3.3).to_i.times { STRING.slice("lo") }
  }

  run("String#split") {
    (MAX*0.6).to_i.times { STRING.split }
  }

  run("String#split(string)") {
    string = STRING.dup
    10.to_i.times  { string[rand(STRING.size)] = "-" }
    (MAX*0.4).to_i.times { string.split("-") }
  }

  run("String#split(string, limit)") {
    string = STRING.dup
    10.to_i.times  { string[rand(STRING.size)] = "-" }
    a = rand(5)
    (MAX*1.6).to_i.times { string.split("-", a) }
  }

  run("String#split(regexp)") {
    string = STRING.dup
    10.to_i.times  { string[rand(STRING.size)] = "-" }
    (MAX*0.2).to_i.times { string.split(/\w+/) }
  }

  run("String#split(regexp, limit)") {
    string = STRING.dup
    10.to_i.times  { string[rand(STRING.size)] = "-" }
    a = rand(5)
    (MAX*0.2).to_i.times { string.split(/\w+/, a) }
  }

  run("String#squeeze") {
    string = STRING.dup
    20.to_i.times  { string[rand(STRING.size)] = " " }
    (MAX*5.3).to_i.times { string.squeeze }
  }

  run("String#squeeze(char)") {
    string = STRING.dup
    20.to_i.times  { string[rand(STRING.size)] = " " }
    (MAX*3.1).to_i.times { string.squeeze(" ") }
  }

  run("String#squeeze('m-n')") {
    string = STRING.dup
    (MAX*1.9).to_i.times { string.squeeze("m-z") }
  }

  run("String#squeeze!") {
    string = STRING.dup
    20.to_i.times  { string[rand(STRING.size)] = " " }
    (MAX*3.9).to_i.times { string.dup.squeeze! }
  }

  run("String#squeeze!(char)") {
    string = STRING.dup
    20.to_i.times  { string[rand(STRING.size)] = " " }
    (MAX*2.5).to_i.times { string.dup.squeeze!(" ") }
  }

  run("String#squeeze!('m-n')") {
    (MAX*2.4).to_i.times { STRING.dup.squeeze!("m-z") }
  }

  run("String#strip") {
    string = (" " * 10) + STRING + (" " * 10)
    (MAX*7.8).to_i.times { string.strip }
  }

  run("String#strip!") {
    string = (" " * 10) + STRING + (" " * 10)
    (MAX*9.1).to_i.times { string.strip! }
  }

  run("String#sub(pattern, repl)") {
    (MAX*3.6).to_i.times { STRING.sub(/[aeiou]/, '*') }
  }

  run("String#sub(pattern) { block }") {
    (MAX*2.1).to_i.times { STRING.sub(/./) { |s| } }
  }

  run("String#sub!(pattern, repl)") {
    (MAX*3.2).to_i.times { STRING.dup.sub!(/[aeiou]/, '*') }
  }

  run("String#sub!(pattern) { block }") {
    (MAX*2.2).to_i.times { STRING.dup.sub!(/./) { |s| } }
  }

  run("String#succ") {
    (MAX*10.4).to_i.times { STRING.succ }
  }

  run("String#succ!") {
    (MAX*4.8).to_i.times { STRING.dup.succ! }
  }

  run("String#sum") {
    (MAX*9.1).to_i.times { STRING.sum }
  }

  run("String#sum(int)") {
    (MAX*10.7).to_i.times { STRING.sum(8) }
  }

  run("String#swapcase") {
    (MAX*2.3).to_i.times { STRING.swapcase }
  }

  run("String#swapcase!") {
    (MAX*2.0).to_i.times { STRING.dup.swapcase! }
  }

  run("String#to_f") {
    string = "123.45"
    (MAX*13.6).to_i.times { string.to_f }
  }

  run("String#to_i") {
    string = "12345"
    (MAX*11.7).to_i.times { string.to_i }
  }

  run("String#to_i(base)") {
    string = "12345"
    (MAX*13.2).to_i.times { string.to_i(8) }
  }

  run("String#to_s") {
    (MAX*18.1).to_i.times { STRING.to_s }
  }

  run("String#to_str") {
    (MAX*18.5).to_i.times { STRING.to_str }
  }

  run("String#to_sym") {
    string = STRING.reverse * (((MAX*11.6).to_i / STRING.size) + 1)
    (MAX*1.0).to_i.times { string.chop!.to_sym }
  }

  run("String#tr") {
    (MAX*4.8).to_i.times { STRING.tr("el", "ip") }
  }

  run("String#tr!") {
    (MAX*3.7).to_i.times { STRING.dup.tr!("el", "ip") }
  }

  run("String#tr_s") {
    (MAX*4.1).to_i.times { STRING.tr_s("l", "r") }
  }

  run("String#tr_s!") {
    (MAX*3.3).to_i.times { STRING.dup.tr_s!("l", "r") }
  }

  # TODO: Add more variations for String#unpack
  run("String#unpack") {
    (MAX*1.9).to_i.times { STRING.unpack("A5") }
  }

  run("String#upcase") {
    (MAX*2.3).to_i.times { STRING.upcase }
  }

  run("String#upcase!") {
    (MAX*1.7).to_i.times { STRING.dup.upcase! }
  }

  run("String#upto") {
    string = "a1"
    (MAX*0.4).to_i.times { string.upto("b6") { |s| } }
  }
end
