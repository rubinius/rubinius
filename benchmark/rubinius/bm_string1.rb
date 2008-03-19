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

  run("base"){
    MAX.times{ STRING }
  }

  run("String.new"){
    MAX.times{ String.new(STRING) }
  }

  run("String#%"){
    string = "%05d"
    number = 123
    MAX.times{ string % number }
  }

  run("String#*"){
    MAX.times{ STRING * 3 }
  }

  run("String#+"){
    MAX.times{ STRING + STRING }
  }

  run("String#<<"){
    string = STRING.dup
    a = rand(STRING.size/2)
    string1 = string[a, STRING.size/2]
    MAX.times{ string << string1 }
  }

  run("String#<=>"){
    string1 = STRING
    string2 = STRING.reverse
    MAX.times{ string1 <=> string2 }
  }

  run("String#=="){
    string1 = STRING
    string2 = STRING.reverse
    MAX.times{ string1 == string2 }
  }

  run("String#==="){
    string1 = STRING
    string2 = STRING.reverse
    MAX.times{ string1 === string2 }
  }

  run("String#=~"){
    MAX.times{ STRING =~ /\w+/ }
  }

  run("String#[int]"){
    MAX.times{ STRING[rand(STRING.size)] }
  }

  run("String#[int,int]"){
    a = rand(STRING.size/2)
    MAX.times{ STRING[a, 15] }
  }

  run("String#[range]"){
    a = rand(STRING.size/2)
    b = a + 15
    MAX.times{ STRING[a..b] }
  }

  run("String#[regexp]"){
    MAX.times{ STRING[/\w+/] }
  }

  run("String#[regexp,int]"){
    string = "hello"
    a = rand(10)
    MAX.times{ STRING[/\w+/, a] }
  }

  run("String#[string]"){
    MAX.times{ STRING["lo"] }
  }

  # TODO: Fix
  #run("String#~"){
  #  string = "hello"
  #  MAX.times{ ~ string }
  #}

  run("String#capitalize"){
    MAX.times{ STRING.capitalize }
  }

  run("String#capitalize!"){
    string = STRING
    MAX.times{ string.dup.capitalize! }
  }

  run("String#casecmp"){
    string1 = STRING
    string2 = STRING.reverse
    MAX.times{ string1.casecmp(string2) }
  }

  run("String#center"){
    a = rand(STRING.size*2)
    MAX.times{ STRING.center(a) }
  }

  run("String#chomp"){
    string = STRING + "\n"
    MAX.times{ string.chomp }
  }

  run("String#chomp!"){
    string = STRING + "\n"
    MAX.times{ string.dup.chomp! }
  }

  run("String#chop"){
    MAX.times{ STRING.chop }
  }

  run("String#chop!"){
    string = STRING
    MAX.times{ string.dup.chop! }
  }

  run("String#count(string)"){
    a = rand(STRING.size/2)
    string = STRING[a,5]
    MAX.times{ STRING.count(string) }
  }

  run("String#count(^string)"){
    a = rand(STRING.size/2)
    string = "^" + STRING[a,5]
    MAX.times{ STRING.count(string) }
  }

  run("String#crypt"){
    MAX.times{ STRING.crypt("sh") }
  }

  run("String#delete"){
    string = "hello"
    MAX.times{ string.delete("lo") }
  }

  run("String#delete!"){
    string = "hello"
    MAX.times{ string.delete!("lo") }
  }

  run("String#downcase"){
    string = "HELLO"
    MAX.times{ string.downcase }
  }

  run("String#downcase!"){
    MAX.times{ STRING.dup.downcase! }
  }

  run("String#dump"){
    string = "hello&%"
    MAX.times{ string.dump }
  }

  run("String#each"){
    MAX.times{ STRING.each{ |e| } }
  }

  run("String#each_byte"){
    MAX.times{ STRING.each_byte{ |e| } }
  }

  run("String#empty?"){
    MAX.times{ STRING.empty? }
  }

  run("String#eql?"){
    string1= STRING
    string2= STRING.reverse
    MAX.times{ string1.eql?(string2) }
  }

  run("String#gsub(regexp, repl)"){
    MAX.times{ STRING.dup.gsub(/[aeiou]/, '*') }
  }

  run("String#gsub(regexp){ block }"){
    MAX.times{ STRING.dup.gsub(/./){ |s| } }
  }

  run("String#gsub!(regexp){ block }"){
    MAX.times{ STRING.dup.gsub!(/./){ |s| } }
  }

  run("String#hex"){
    string = "0x"
    8.times { string << rand(16).to_s(16) }
    MAX.times{ string.hex }
  }

  run("String#include?"){
    MAX.times{ STRING.include?("lo") }
  }

  run("String#index(string)"){
    MAX.times{ STRING.index("e") }
  }

  run("String#index(string, offset)"){
    a = rand(STRING.size/2)
    MAX.times{ STRING.index("e", -a) }
  }

  run("String#index(int)"){
    a = rand(STRING.size)
    MAX.times{ STRING.index(a) }
  }

  run("String#index(int, offset)"){
    a = rand(STRING.size/2)
    b = rand(STRING.size/2)
    MAX.times{ STRING.index(a, -b) }
  }

  run("String#index(regexp)"){
    MAX.times{ STRING.index(/[aeiou]/) }
  }

  run("String#index(regexp, offset)"){
    a = rand(STRING.size/2)
    MAX.times{ STRING.index(/[aeiou]/, -a) }
  }

  run("String#insert"){
    a = rand(STRING.size/2)
    MAX.times{ STRING.insert(a, "world") }
  }

  run("String#intern"){
    string = STRING.dup
    MAX.times{ string.chop!.intern }
  }

  run("String#length"){
    MAX.times{ STRING.length }
  }

  run("String#ljust"){
    a = rand(STRING.size*2)
    MAX.times{ STRING.ljust(a) }
  }

  run("String#lstrip"){
    string = (" " * rand(10)) + STRING
    MAX.times{ string.lstrip }
  }

  run("String#lstrip!"){
    string = (" " * rand(10)) + STRING
    MAX.times{ string.lstrip! }
  }

  run("String#match(regexp)"){
    MAX.times{ STRING.match(/lo/) }
  }

  run("String#match(string)"){
    MAX.times{ STRING.match("lo") }
  }

  run("String#oct"){
    string = "0"
    8.times { string << rand(8).to_s }
    MAX.times{ string.oct }
  }

  run("String#replace"){
    string = ""
    8.times { string << STRING[rand(STRING.size)] }
    MAX.times{ STRING.dup.replace(string) }
  }

  run("String#reverse"){
    MAX.times{ STRING.reverse }
  }

  run("String#reverse!"){
    MAX.times{ STRING.dup.reverse! }
  }

  run("String#rindex(string)"){
    MAX.times{ STRING.rindex("e") }
  }

  run("String#rindex(string, int)"){
    a = rand(STRING.size/2)
    MAX.times{ STRING.rindex("e",a) }
  }

  run("String#rindex(int, int)"){
    a = rand(STRING.size/2)
    b = rand(STRING.size/2)
    MAX.times{ STRING.rindex(a, b) }
  }

  run("String#rindex(regexp)"){
    MAX.times{ STRING.rindex(/[aeiou]/) }
  }

  run("String#rindex(regexp, int)"){
    a = rand(STRING.size/2)
    MAX.times{ STRING.rindex(/[aeiou]/, a) }
  }

  run("String#rjust(width)"){
    a = rand(STRING.size*2)
    MAX.times{ STRING.rjust(a) }
  }

  run("String#rjust(width, padding)"){
    a = rand(STRING.size*2)
    MAX.times{ STRING.rjust(a, "-") }
  }

  run("String#rstrip"){
    string = STRING + (" " * rand(10))
    MAX.times{ string.rstrip }
  }

  run("String#rstrip!"){
    string = STRING + (" " * rand(10))
    MAX.times{ string.rstrip! }
  }

  run("String#scan"){
    MAX.times{ STRING.scan(/\w+/) }
  }

  run("String#scan{ block }"){
    MAX.times{ STRING.scan(/\w+/){ |w| } }
  }

  run("String#slice(int)"){
    a = rand(STRING.size/2)
    MAX.times{ STRING.dup.slice(a) }
  }

  run("String#slice(int, int)"){
    a = rand(STRING.size/2)
    b = rand(STRING.size/2)
    MAX.times{ STRING.slice(a, b) }
  }

  run("String#slice(range)"){
    a = rand(STRING.size/2)
    b = rand(STRING.size/2)
    MAX.times{ STRING.slice(a..b) }
  }

  run("String#slice(regexp)"){
    MAX.times{ STRING.slice(/ell/) }
  }

  run("String#slice(string)"){
    MAX.times{ STRING.slice("lo") }
  }

  run("String#split"){
    MAX.times{ STRING.split }
  }

  run("String#split(string)"){
    string = STRING.dup
    10.times { string[rand(STRING.size)] = "-" }
    MAX.times{ string.split("-") }
  }

  run("String#split(string, limit)"){
    string = STRING.dup
    10.times { string[rand(STRING.size)] = "-" }
    a = rand(5)
    MAX.times{ string.split("-", a) }
  }

  run("String#split(regexp)"){
    string = STRING.dup
    10.times { string[rand(STRING.size)] = "-" }
    MAX.times{ string.split(/\w+/) }
  }

  run("String#split(regexp, limit)"){
    string = STRING.dup
    10.times { string[rand(STRING.size)] = "-" }
    a = rand(5)
    MAX.times{ string.split(/\w+/, a) }
  }

  run("String#squeeze"){
    string = STRING.dup
    20.times { string[rand(STRING.size)] = " " }
    MAX.times{ string.squeeze }
  }

  run("String#squeeze(char)"){
    string = STRING.dup
    20.times { string[rand(STRING.size)] = " " }
    MAX.times{ string.squeeze(" ") }
  }

  run("String#squeeze('m-n')"){
    string = STRING.dup
    MAX.times{ string.squeeze("m-z") }
  }

  run("String#squeeze!"){
    string = STRING.dup
    20.times { string[rand(STRING.size)] = " " }
    MAX.times{ string.dup.squeeze! }
  }

  run("String#squeeze!(char)"){
    string = STRING.dup
    20.times { string[rand(STRING.size)] = " " }
    MAX.times{ string.dup.squeeze!(" ") }
  }

  run("String#squeeze!('m-n')"){
    MAX.times{ STRING.dup.squeeze!("m-z") }
  }

  run("String#strip"){
    string = (" " * rand(10)) + STRING + (" " * rand(10))
    MAX.times{ string.strip }
  }

  run("String#strip!"){
    string = (" " * rand(10)) + STRING + (" " * rand(10))
    MAX.times{ string.strip! }
  }

  run("String#sub(pattern, repl)"){
    MAX.times{ STRING.sub(/[aeiou]/, '*') }
  }

  run("String#sub(pattern){ block }"){
    MAX.times{ STRING.sub(/./){ |s| } }
  }

  run("String#sub!(pattern, repl)"){
    MAX.times{ STRING.dup.sub!(/[aeiou]/, '*') }
  }

  run("String#sub!(pattern){ block }"){
    MAX.times{ STRING.dup.sub!(/./){ |s| } }
  }

  run("String#succ"){
    MAX.times{ STRING.succ }
  }

  run("String#succ!"){
    MAX.times{ STRING.dup.succ! }
  }

  run("String#sum"){
    MAX.times{ STRING.sum }
  }

  run("String#sum(int)"){
    MAX.times{ STRING.sum(8) }
  }

  run("String#swapcase"){
    MAX.times{ STRING.swapcase }
  }

  run("String#swapcase!"){
    MAX.times{ STRING.dup.swapcase! }
  }

  run("String#to_f"){
    string = "123.45"
    MAX.times{ string.to_f }
  }

  run("String#to_i"){
    string = "12345"
    MAX.times{ string.to_i }
  }

  run("String#to_i(base)"){
    string = "12345"
    MAX.times{ string.to_i(8) }
  }

  run("String#to_s"){
    MAX.times{ STRING.to_s }
  }

  run("String#to_str"){
    MAX.times{ STRING.to_str }
  }

  run("String#to_sym"){
    string = STRING
    MAX.times{ string.chop!.to_sym }
  }

  run("String#tr"){
    MAX.times{ STRING.tr("el","ip") }
  }

  run("String#tr!"){
    MAX.times{ STRING.dup.tr!("el","ip") }
  }

  run("String#tr_s"){
    MAX.times{ STRING.tr_s("l","r") }
  }

  run("String#tr_s!"){
    string = "hello"
    MAX.times{ STRING.dup.tr_s!("l","r") }
  }

  # TODO: Add more variations for String#unpack
  run("String#unpack"){
    MAX.times{ STRING.unpack("A5") }
  }

  run("String#upcase"){
    MAX.times{ STRING.upcase }
  }

  run("String#upcase!"){
    MAX.times{ STRING.dup.upcase! }
  }

  run("String#upto"){
    string = "a1"
    MAX.times{ string.upto("b6"){ |s| } }
  }
end