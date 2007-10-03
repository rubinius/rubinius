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

MAX = 1000

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
    MAX.times{}
  }
  
   run("String.new"){
     MAX.times{ String.new("test") }
   }

   run("String#%"){
      string = "%05d"
      number = 123
      MAX.times{ string % number }
   }

   run("String#*"){
      string = "test"
      int = 3
      MAX.times{ string * int }
   }

   run("String#+"){
      string1 = "hello"
      string2 = "world"
      MAX.times{ string1 + string2 }
   }

   run("String#<<"){
      string1 = "hello"
      string2 = "world"
      MAX.times{ string1 << string2 }
   }

   run("String#<=>"){
      string1 = "abcdef"
      string2 = "abcdef"
      MAX.times{ string1 <=> string2 }
   }

   run("String#=="){
      string1 = "abc"
      string2 = "abcd"
      MAX.times{ string1 == string2 }
   }

   run("String#==="){
      string1 = "Hello"
      string2 = "HellO"
      MAX.times{ string1 === string2 }
   }

   run("String#=~"){
      string = "hello"
      MAX.times{ string =~ /\w+/ }
   }

   run("String#[int]"){
      string = "hello"
      MAX.times{ string[1] }
   }

   run("String#[int,int]"){
      string = "hello"
      MAX.times{ string[1,2] }
   }

   run("String#[range]"){
      string = "hello"
      MAX.times{ string[1..2] }
   }

   run("String#[regexp]"){
      string = "hello"
      MAX.times{ string[/\w+/] }
   }

   run("String#[regexp,int]"){
      string = "hello"
      MAX.times{ string[/\w+/,1] }
   }

   run("String#[string]"){
      string = "hello"
      MAX.times{ string["lo"] }
   }

   # TODO: Fix
   #run("String#~"){
   #   string = "hello"
   #   MAX.times{ ~ string }
   #}

   run("String#capitalize"){
      string = "hello"
      MAX.times{ string.capitalize }
   }

   run("String#capitalize!"){
      string = "hello"
      MAX.times{ string.capitalize! }
   }

   run("String#casecmp"){
      string1 = "hello"
      string2 = "HELLO"
      MAX.times{ string1.casecmp(string2) }
   }

   run("String#center"){
      string = "hello"
      MAX.times{ string.center(4) }
   }

   run("String#chomp"){
      string = "hello\n"
      MAX.times{ string.chomp }
   }

   run("String#chomp!"){
      string = "hello\n"
      MAX.times{ string.chomp! }
   }

   run("String#chop"){
      string = "hello"
      MAX.times{ string.chop }
   }

   run("String#chop!"){
      string = "hello"
      MAX.times{ string.chop! }
   }

   run("String#count(string)"){
      string = "hello"
      MAX.times{ string.count("lo") }
   }

   run("String#count(^string)"){
      string = "hello"
      MAX.times{ string.count("^l") }
   }

   run("String#crypt"){
      string = "hello"
      MAX.times{ string.crypt("sh") }
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
      string = "HELLO"
      MAX.times{ string.downcase! }
   }

   run("String#dump"){
      string = "hello&%"
      MAX.times{ string.dump }
   }

   run("String#each"){
      string = "hello\nworld"
      MAX.times{ string.each{ |e| } }
   }

   run("String#each_byte"){
      string = "hello"
      MAX.times{ string.each_byte{ |e| } }
   }

   run("String#empty?"){
      string = ""
      MAX.times{ string.empty? }
   }

   run("String#eql?"){
      string1= "hello"
      string2= "hello"
      MAX.times{ string1.eql?(string2) }
   }

   run("String#gsub(regexp, repl)"){
      string = "hello"
      MAX.times{ string.gsub(/[aeiou]/, '*') }
   }

   run("String#gsub(regexp){ block }"){
      string = "hello"
      MAX.times{ string.gsub(/./){ |s| } }
   }

   run("String#gsub!(regexp){ block }"){
      string = "hello"
      MAX.times{ string.gsub!(/./){ |s| } }
   }

   run("String#hex"){
      string = "0x0a"
      MAX.times{ string.hex }
   }

   run("String#include?"){
      string = "hello"
      MAX.times{ string.include?("lo") }
   }

   run("String#index(string)"){
      string = "hello"
      MAX.times{ string.index("e") }
   }

   run("String#index(string, offset)"){
      string = "hello"
      MAX.times{ string.index("e", -1) }
   }

   run("String#index(int)"){
      string = "hello"
      MAX.times{ string.index(1) }
   }

   run("String#index(int, offset)"){
      string = "hello"
      MAX.times{ string.index(1, -1) }
   }

   run("String#index(regexp)"){
      string = "hello"
      MAX.times{ string.index(/[aeiou]/) }
   }

   run("String#index(regexp, offset)"){
      string = "hello"
      MAX.times{ string.index(/[aeiou]/, -1) }
   }

   run("String#insert"){
      string = "hello"
      MAX.times{ string.insert(2, "world") }
   }

   run("String#intern"){
      string = "hello"
      MAX.times{ string.intern }
   }

   run("String#length"){
      string = "hello"
      MAX.times{ string.length }
   }

   run("String#ljust"){
      string = "hello"
      MAX.times{ string.ljust(10) }
   }

   run("String#lstrip"){
      string = "   hello"
      MAX.times{ string.lstrip }
   }

   run("String#lstrip!"){
      string = "   hello"
      MAX.times{ string.lstrip! }
   }

   run("String#match(regexp)"){
      string = "hello"
      MAX.times{ string.match(/lo/) }
   }

   run("String#match(string)"){
      string = "hello"
      MAX.times{ string.match("lo") }
   }
   
   run("String#oct"){
      string = "123"
      MAX.times{ string.oct }
   }

   run("String#replace"){
      string = "hello"
      MAX.times{ string.replace("world") }
   }

   run("String#reverse"){
      string = "hello"
      MAX.times{ string.reverse }
   }

   run("String#reverse!"){
      string = "hello"
      MAX.times{ string.reverse! }
   }

   run("String#rindex(string)"){
      string = "hello"
      MAX.times{ string.rindex("e") }
   }

   run("String#rindex(string, int)"){
      string = "hello"
      MAX.times{ string.rindex("e",1) }
   }

   run("String#rindex(int, int)"){
      string = "hello"
      MAX.times{ string.rindex(1,1) }
   }

   run("String#rindex(regexp)"){
      string = "hello"
      MAX.times{ string.rindex(/[aeiou]/) }
   }

   run("String#rindex(regexp, int)"){
      string = "hello"
      MAX.times{ string.rindex(/[aeiou]/, 1) }
   }

   run("String#rjust(width)"){
      string = "hello"
      MAX.times{ string.rjust(10) }
   }

   run("String#rjust(width, padding)"){
      string = "hello"
      MAX.times{ string.rjust(10, "-") }
   }

   run("String#rstrip"){
      string = "hello    "
      MAX.times{ string.rstrip }
   }

   run("String#rstrip!"){
      string = "hello    "
      MAX.times{ string.rstrip! }
   }

   run("String#scan"){
      string = "cruel world"
      MAX.times{ string.scan(/\w+/) }
   }

   run("String#scan{ block }"){
      string = "cruel world"
      MAX.times{ string.scan(/\w+/){ |w| } }
   }

   run("String#slice(int)"){
      string = "hello"
      MAX.times{ string.slice(1) }
   }

   run("String#slice(int, int)"){
      string = "hello"
      MAX.times{ string.slice(1,3) }
   }

   run("String#slice(range)"){
      string = "hello"
      MAX.times{ string.slice(1..3) }
   }

   run("String#slice(regexp)"){
      string = "hello"
      MAX.times{ string.slice(/ell/) }
   }

   run("String#slice(string)"){
      string = "hello"
      MAX.times{ string.slice("lo") }
   }

   run("String#split"){
      string = "now is the time"
      MAX.times{ string.split }
   }

   run("String#split(string)"){
      string = "now-is-the-time"
      MAX.times{ string.split("-") }
   }

   run("String#split(string, limit)"){
      string = "now-is-the-time"
      MAX.times{ string.split("-", 2) }
   }

   run("String#split(regexp)"){
      string = "now-is-the-time"
      MAX.times{ string.split(/\w+/) }
   }

   run("String#split(regexp, limit)"){
      string = "now-is-the-time"
      MAX.times{ string.split(/\w+/, 2) }
   }

   run("String#squeeze"){
      string = "foo    moo    hello"
      MAX.times{ string.squeeze }
   }

   run("String#squeeze(char)"){
      string = "foo    moo    hello"
      MAX.times{ string.squeeze(" ") }
   }

   run("String#squeeze!"){
      string = "foo    moo    hello"
      MAX.times{ string.squeeze! }
   }

   run("String#squeeze!(char)"){
      string = "foo    moo    hello"
      MAX.times{ string.squeeze!(" ") }
   }

   run("String#strip"){
      string = "   hello    "
      MAX.times{ string.strip }
   }

   run("String#strip!"){
      string = "   hello    "
      MAX.times{ string.strip! }
   }

   run("String#sub(pattern, repl)"){
      string = "hello"
      MAX.times{ string.sub(/[aeiou]/, '*') }
   }

   run("String#sub(pattern){ block }"){
      string = "hello"
      MAX.times{ string.sub(/./){ |s| } }
   }

   run("String#sub!(pattern, repl)"){
      string = "hello"
      MAX.times{ string.sub!(/[aeiou]/, '*') }
   }

   run("String#sub!(pattern){ block }"){
      string = "hello"
      MAX.times{ string.sub!(/./){ |s| } }
   }

   run("String#succ"){
      string = "hello"
      MAX.times{ string.succ }
   }

   run("String#succ!"){
      string = "hello"
      MAX.times{ string.succ! }
   }

   run("String#sum"){
      string = "now is the time"
      MAX.times{ string.sum }
   }

   run("String#sum(int)"){
      string = "now is the time"
      MAX.times{ string.sum(8) }
   }

   run("String#swapcase"){
      string = "Hello"
      MAX.times{ string.swapcase }
   }

   run("String#swapcase!"){
      string = "Hello"
      MAX.times{ string.swapcase! }
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
      string = "hello"
      MAX.times{ string.to_s }
   }

   run("String#to_str"){
      string = "hello"
      MAX.times{ string.to_str }
   }

   run("String#to_sym"){
      string = "hello"
      MAX.times{ string.to_sym }
   }

   run("String#tr"){
      string = "hello"
      MAX.times{ string.tr("el","ip") }
   }

   run("String#tr!"){
      string = "hello"
      MAX.times{ string.tr!("el","ip") }
   }

   run("String#tr_s"){
      string = "hello"
      MAX.times{ string.tr_s("l","r") }
   }

   run("String#tr_s!"){
      string = "hello"
      MAX.times{ string.tr_s!("l","r") }
   }
   
   # TODO: Add more variations for String#unpack
   run("String#unpack"){
      string = "hello"
      MAX.times{ string.unpack("A5") }
   }

   run("String#upcase"){
      string = "heLLo"
      MAX.times{ string.upcase }
   }

   run("String#upcase!"){
      string = "heLLo"
      MAX.times{ string.upcase! }
   }

   run("String#upto"){
      string = "a1"
      MAX.times{ string.upto("b6"){ |s| } }
   }
end