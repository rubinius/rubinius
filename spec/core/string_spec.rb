require File.dirname(__FILE__) + '/../spec_helper'

# %, *, +, <<, <=>, ==, =~, [], []=, capitalize, capitalize!,
# casecmp, center, chomp, chomp!, chop, chop!, concat, count, crypt,
# delete, delete!, downcase, downcase!, dump, each, each_byte,
# each_line, empty?, eql?, gsub, gsub!, hash, hex, include?, index,
# initialize_copy, insert, inspect, intern, length, ljust, lstrip,
# lstrip!, match, next, next!, oct, replace, reverse, reverse!,
# rindex, rjust, rstrip, rstrip!, scan, size, slice, slice!, split,
# squeeze, squeeze!, strip, strip!, sub, sub!, succ, succ!, sum,
# swapcase, swapcase!, to_f, to_i, to_s, to_str, to_sym, tr, tr!,
# tr_s, tr_s!, unpack, upcase, upcase!, upto

context "String instance method" do
  
  specify "% should return a string resulting from applying self as a format specification to other" do
    example do
      [ "%b" % 10,
        "% b" % 10,
        "%1$b" % [10, 20],
        "%#b" % 10,
        "%+b" % 10,
        "%-9b" % 10,
        "%05b" % 10,
        "%*b" % [10, 6],

        "%c" % 10,
        "%2$c" % [10, 11, 14],
        "%-4c" % 10,
        "%*c" % [10, 3],

        "%d" % 10,
        "% d" % 10,
        "%1$d" % [10, 20],
        "%+d" % 10,
        "%-7d" % 10,
        "%04d" % 10,
        "%*d" % [10, 4],

        "%E" % 10,
        "% E" % 10,
        "%1$E" % 10,
        "%#E" % 10,
        "%+E" % 10,
        "%-7E" % 10,
        "%05E" % 10,
        "%*E" % [10, 9],
        
        "%e" % 10,
        "% e" % 10,
        "%1$e" % 10,
        "%#e" % 10,
        "%+e" % 10,
        "%-7e" % 10,
        "%05e" % 10,
        "%*e" % [10, 9],

        "%f" % 10,
        "% f" % 10,
        "%1$f" % 10,
        "%#f" % 10,
        "%+f" % 10,
        "%-7f" % 10,
        "%05f" % 10,
        "%*f" % [10, 9],

        "%G" % 10,
        "% G" % 10,
        "%1$G" % 10,
        "%#G" % 10,
        "%+G" % 10,
        "%-7G" % 10,
        "%05G" % 10,
        "%*G" % [10, 9],

        "%g" % 10,
        "% g" % 10,
        "%1$g" % 10,
        "%#g" % 10,
        "%+g" % 10,
        "%-7g" % 10,
        "%05g" % 10,
        "%*g" % [10, 9],

        "%i" % 10,
        "% i" % 10,
        "%1$i" % [10, 20],
        "%+i" % 10,
        "%-7i" % 10,
        "%04i" % 10,
        "%*i" % [10, 4],

        "%o" % 10,
        "% o" % 10,
        "%1$o" % [10, 20],
        "%#o" % 10,
        "%+o" % 10,
        "%-9o" % 10,
        "%05o" % 10,
        "%*o" % [10, 6],
        
        "%p" % 10,
        "%1$p" % [10, 5],
        "%-22p" % 10,
        "%*p" % [10, 10],

        "%s" % 10,
        "%1$s" % [10, 8],
        "%-5s" % 10,
        "%*s" % [10, 9],

        "%u" % 10,
        "% u" % 10,
        "%1$u" % [10, 20],
        "%+u" % 10,
        "%-7u" % 10,
        "%04u" % 10,
        "%*u" % [10, 4],

        "%X" % 10,
        "% X" % 10,
        "%1$X" % [10, 20],
        "%#X" % 10,
        "%+X" % 10,
        "%-9X" % 10,
        "%05X" % 10,
        "%*X" % [10, 6],

        "%x" % 10,
        "% x" % 10,
        "%1$x" % [10, 20],
        "%#x" % 10,
        "%+x" % 10,
        "%-9x" % 10,
        "%05x" % 10,
        "%*x" % [10, 6] ]
    end.should == ["1010", " 1010", "1010", "0b1010", "+1010", "1010     ", "01010", "       110", 
          "\n", "\v", "\n   ", "         \003", "10", " 10", "10", "+10", "10     ", "0010", 
          "         4", "1.000000E+01", " 1.000000E+01", "1.000000E+01", "1.000000E+01", 
          "+1.000000E+01", "1.000000E+01", "1.000000E+01", "9.000000E+00", "1.000000e+01", 
          " 1.000000e+01", "1.000000e+01", "1.000000e+01", "+1.000000e+01", "1.000000e+01", 
          "1.000000e+01", "9.000000e+00", "10.000000", " 10.000000", "10.000000", "10.000000", 
          "+10.000000", "10.000000", "10.000000", "  9.000000", "10", " 10", "10", "10.0000", 
          "+10", "10     ", "00010", "         9", "10", " 10", "10", "10.0000", "+10", "10     ", 
          "00010", "         9", "10", " 10", "10", "+10", "10     ", "0010", "         4", 
          "12", " 12", "12", "012", "+12", "12       ", "00012", "         6", "10", "10", 
          "10                    ", "        10", "10", "10", "10   ", "         9", "10", 
          " 10", "10", "+10", "10     ", "0010", "         4", "A", " A", "A", "0XA", "+A", 
          "A        ", "0000A", "         6", "a", " a", "a", "0xa", "+a", "a        ", "0000a", "         6" ]
  end
  
  specify "* should return a new string that is n copies of self" do
    example do
      "cool" * 3
    end.should == "coolcoolcool"
  end
  
  specify "+ should concatentate self with other" do
    example do
      "Ruby !" + "= Rubinius"
    end.should == "Ruby != Rubinius"
  end
  
  specify "<< should concatenate the other object" do
    example do
      @a = 'hello '
      @a << 'world'
      @b = @a.clone
      @b << 33
      [@a, @b]
    end.should == ["hello world", "hello world!"]
  end
  
  specify "<=> should return -1, 0, 1 when self is less than, equal, or greater than other" do
    example do
      [ "this" <=> "those", "yep" <=> "yep", "yoddle" <=> "griddle" ]
    end.should == [-1, 0, 1]
  end
  
  specify "== with other String should return true if <=> returns 0" do
    example do
      [ "equal" == "equal", "more" == "MORE", "less" == "greater" ]
    end.should == [true, false, false]
  end
  
  specify "== with other not String should return self == other.to_str if other responds to to_str" do
    example do
      class Foo
        def to_str
        end
        def ==(other)
          "foo" == other
        end
      end
      "foo" == Foo.new
    end.should == true
  end
  
  specify "== with other not String should return false if other does not respond to to_str" do
    example do
      class Bar
        def ==(other)
          "foo" == other
        end
      end
      "foo" == Bar.new
    end.should == false
  end
  
  specify "=== should be a synonym for ==" do
    example do
      [ "equal" == "equal", "more" == "MORE", "less" == "greater" ]
    end.should == [true, false, false]
  end
  
  specify "=~ should return the position of match start" do
    example do
      ["rudder" =~ /udder/, "boat" =~ /[^fl]oat/]
    end.should == [1, 0]
  end
  
  specify "=~ should return nil if there is no match" do
    example do
      ["bean" =~ /bag/, "true" =~ /false/ ]
    end.should == [nil, nil]
  end
  
  specify "[] with index should return the code of the character at index" do
    example do
      "hello"[1]
    end.should == 101
  end
  
  specify "[] with start, length should return the substring of length characters begin at start" do
    example do
      [ "hello"[1, 3],
        "hello"[-3, 2],
        "hello"[5, 7] ]
    end.should == ["ell", "ll", ""]
  end
  
  specify "[] with range should return the substring specified by range" do
    example do
      [ "world"[0..2],
        "world"[-4..-2],
        "world"[1...3] ]
    end.should == ["wor", "orl", "or"]
  end
  
  specify "[] with regexp should return the string matching pattern" do
    example do
      [ "hello there"[/[aeiou](.)\1/],
        "hello there"[/ell/],
        "hello there"[/o /],
        "hello there"[/d/] ]
    end.should == ["ell", "ell", "o ", nil]
  end
  
  specify "[] with regexp, index should return the string specified by the nth component MatchData" do
    example do
      [ "hello there"[/[aeiou](.)\1/, 0],
        "hello there"[/he/, 1],
        "hello there"[/he/, 0],
        "hello there"[/[aeiou](.)\1/, 2] ]
    end.should == ["ell", nil, "he", nil]
  end
  
  specify "[] with string should return other if it occurs in self, otherwise nil" do
    example do
      [ "hello"["el"],
        "hello"["bye"],
        "hello"["hello"] ]
    end.should == ["el", nil, "hello"]
  end
  
  specify "[]= with index should replace the character at index with the character code" do
    example do
      @s = "barf"
      [@s[3] = ?k, @s]
    end.should == [107, "bark"]
  end
  
  specify "[]= with index should replace the character at index with string" do
    example do
      @s = "Ruby"
      [@s[3] = "inius", @s]
    end.should == ["inius", "Rubinius"]
  end
  
  specify "[]= with start, length should replace length characters at start with string" do
    example do
      @s = "once upon a time"
      [@s[5, 11] = "a lifetime", @s]
    end.should == ["a lifetime", "once a lifetime"]
  end
  
  specify "[]= with range should replace characters in range with string" do
    example do
      @s = "drabble"
      [@s[3..6] = "pe", @s]
    end.should == ["pe", "drape"]
  end
  
  specify "[]= with regexp should replace the characters that match pattern with string" do
    example do
      @s = "caddie"
      [@s[/d{2}/] = "bb", @s]
    end.should == ["bb", "cabbie"]
  end

  specify "[]= with regexp and index should replace the specified portion of the match with string" do
    example do
      @s = "abc123def411"
      [@s[/(\d+)([a-e]*)/, 1] = "abcc", @s]
    end.should == ["abcc", "abcabccdef411"]
  end

  specify "[]= with string should replace the string with other string" do
    example do
      @s = "ten times"
      [@s["ten"] = "twenty", @s]
    end.should == ["twenty", "twenty times"]
  end
  
  specify "~ should be equivalent to $_ =~ self" do
    example do
      $_ = "bat"
      ~ /c?at/
    end.should == 1
  end

  specify "capitalize should return a copy of string and convert the first character to uppercase and the rest to lowercase" do
    example do
      @a = "LOWER"
      @b = "MenTaLguY"
      [@a.capitalize, @a, @b.capitalize, @b]
    end.should == ["Lower", "LOWER", "Mentalguy", "MenTaLguY"]
  end
  
  specify "capitalize! should modify self to convert the first character to upper case and the rest to lowercase" do
    example do
      @a = "this"
      @b = "THIS"
      [@a.capitalize!, @a, @b, @b.capitalize!]
    end.should == ["This", "This", "This", "This"]
  end

  specify "casecmp should be a case-insensitive version of <=>" do
    example do
      ["One".casecmp("one"), "Two".casecmp("too"), "MINE".casecmp("nINE")]
    end.should == [0, 1, -1]
  end

  specify "center should return a string padded on both sides" do
    example do
      ["one".center(9,'.'), "two".center(5), "middle".center(13,'-')]
    end.should == ["...one...", " two ", "---middle----"]
  end
  
  specify "center should return a string when length of self is greater than argument" do
    example do
      ["this".center(3), "radiology".center(8, '-')]
    end.should == ["this", "radiology"]
  end

  specify "chomp should return a new string with the given record separator removed from the end" do
    example do
      @s = "one\n"
      @t = "two\r\n"
      @u = "three\r"
      @v = "four"
      [@s.chomp, @s, @t.chomp, @t, @u.chomp, @u, @v.chomp, @v]
    end.should == ["one", "one\n", "two", "two\r\n", "three", "three\r", "four", "four"]
  end

  specify "chomp! should modify self to remove the given record separator from the end" do
    example do
      @s = "one\n"
      @t = "two\r\n"
      @u = "three\r"
      @v = "four"
      @w = "four\n\r"
      [@s.chomp!, @t.chomp!, @u.chomp!, @v.chomp!, @w.chomp!]
    end.should == ["one", "two", "three", nil, "four\n"]
  end
  
  specify "chomp! should return nil if no changes are made" do
    example do
      ["".chomp!, "line".chomp!]
    end.should == [nil, nil]
  end

  specify "chop should return a new string with the last character removed" do
    example do
      @s = "block"
      [@s.chop, @s]
    end.should == ["bloc", "block"]
  end

  specify "chop! should modify self to remove the last character" do
    example do
      "ouch".chop!
    end.should == "ouc"
  end
  
  specify "chop! should return nil if no changes are made" do
    example do
      "".chop!
    end.should == nil
  end

  specify "concat should be a synonym for <<" do
    example do
      @a = 'hello '
      @a.concat('world')
      @b = @a.clone
      @b.concat(33)
      [@a, @b]
    end.should == ["hello world", "hello world!"]
  end

  specify "count should return the count of characters specified by the union of the arguments" do
    example do
      @a = "hello world" 
      [ @a.count("lo"),
        @a.count("lo", "o"),
        @a.count("hello", "^l"),
        @a.count("ej-m") ]
    end.should == [5, 2, 4, 4]
  end

  specify "crypt should return a one-way cryptographic hash of self using the standard libary function crypt" do
    example do
      "secret".crypt("mysterious")
    end.should == "my6b6UoW5FeNw"
  end

  specify "delete should return a copy of self removing all characters in the intersection of its arguments" do
    example do
      [ "hello".delete("l","lo"),
        "hello".delete("lo"),
        "hello".delete("aeiou", "^e"),
        "hello".delete("ej-m") ]
    end.should == ["heo", "he", "hell", "ho"]
  end

  specify "delete! should perform method delete in place on self" do
    example do
      @s = "hello"
      @t = "world"
      [@s.delete!("l", "lo"), @s, @t.delete!("aeiou", "^e"), @t]
    end.should == ["heo", "heo", "wrld", "wrld"]
  end
  
  specify "delete! should return nil if no changes are made" do
    example do
      "help".delete!("q", "r", "biff")
    end.should == nil
  end

  specify "downcase should return a copy of self with A-Z converted to a-z" do
    example do
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ".downcase
    end.should == "abcdefghijklmnopqrstuvwxyz"
  end

  specify "downcase! should perform downcase in place on self" do
    example do
      @s = "MenTaLguY"
      [@s.downcase!, @s]
    end.should == ["mentalguy", "mentalguy"]
  end
  
  specify "downcase! should return nil if no changes are made" do
    example do
      "lower case".downcase!
    end.should == nil
  end

  specify "dump should return a string with all non-printing characters replaced with \\nnn notation" do
    example do
      ("\000".."A").to_a.to_s.dump
    end.should == "\"\\000\\001\\002\\003\\004\\005\\006\\a\\b\\t\\n\\v\\f\\r\\016\\017\\020\\021\\022\\023\\024\\025\\026\\027\\030\\031\\032\\e\\034\\035\\036\\037 !\\\"\\\#$%&'()*+,-./0123456789\""
  end

  specify "each should split self on the argument ($/ default) and pass each substring to block" do
    example do
      @a = []
      "once\nup\na\ntime\n".each { |s| @a << s }
      @a
    end.should == ["once\n", "up\n", "a\n", "time\n"]
  end
  
  specify "each_byte should pass each byte to block" do
    example do
      @a = []
      "Rubinius".each_byte { |b| @a << b }
      @a
    end.should == [82, 117, 98, 105, 110, 105, 117, 115]
  end

  specify "each_line should be a synonym for each" do
    example do
      @a = []
      "once\nup\na\ntime\n".each { |s| @a << s }
      @a
    end.should == ["once\n", "up\n", "a\n", "time\n"]
  end

  specify "empty? should return true if self is sero-length" do
    example do
      ["".empty?, "Ruby".empty?]
    end.should == [true, false]
  end

  specify "eql? should return true if other is a String with identical contents" do
    example do
      ["".eql?(""), "Rubinius".eql?(Fixnum), "1".eql?(1)]
    end.should == [true, false, false]
  end

  specify "gsub should return a string with all instances of pattern replaced by replacement" do
    example do
      "abracadabra".gsub("a", "xu")
    end.should == "xubrxucxudxubrxu"
  end
  
  specify "gsub with block should return a string with all instances of pattern replaced by the value of block" do
    example do
      "every which way to grab one".gsub!(/[aeiou]/) { |s| s.succ }
    end.should == "fvfry whjch wby tp grbb pnf"
  end

  specify "gsub! should perform gsub in place on self" do
    example do
      @s = "Ruby Ruby Ruby"
      [@s.gsub!(/Ru/, "cu"), @s]
    end.should == ["cuby cuby cuby", "cuby cuby cuby"]
  end
  
  specify "gsub! should return nil if no changes are made" do
    example do
      "".gsub!(/./, 'R')
    end.should == nil
  end

  specify "hash should be provided" do
    example do
      "".respond_to?(:hash)
    end.should == true
  end

  specify "hex should convert the leading characters (0x optional) to a number base 16" do
    example do
      ["0x1".hex, "0x".hex, "0x10".hex, "ffb3".hex ]
    end.should == [1, 0, 16, 65459]
  end
  
  specify "hex should return 0 when characters are not a hexadecimal representation" do
    example do
      ["x0".hex, "Jupiter".hex]
    end.should == [0, 0]
  end

  specify "include? should return true when self contains other string or character" do
    example do
      @s = "abracadabra"
      [@s.include?('a'), @s.include?("ba"), @s.include?("dab"), @s.include?("bra")]
    end.should == [true, false, true, true]
  end

  specify "index with fixnum should return the index of the given character" do
    example do
      [ "hello".index(101),
        "hello".index(52) ]
    end.should == [1, nil]
  end
  
  specify "index with string should return the index of the beginning of string" do
    # example do
    #   [ "hello".index('e'),
    #     "hello".index('lo', -3),
    #     "hello".index('lo', -1),
    #     "hello".index('a') ]
    # end.should == [1, 3, nil, nil]
  end
  
  specify "index with regexp should return the index of the beginning of pattern" do
    example do
      [ "hello".index(/[aeiou]/, -3),
        "hello".index(/he/, 2),
        "hello".index(/el/) ]
    end.should == [4, nil, 1]
  end

  specify "index with substring arg should return substring when it exists" do
    example do
      "hello".index('e')
    end.should == 1
  end

  specify "initialize_copy should return a new string initialized with other" do
    example do
      "".send(:initialize_copy, "Dolly")
    end.should == "Dolly"
  end
  
  specify "insert with positive index should insert other string before the character at index" do
    example do
      ["rinius".insert(1,"ub"), "p".insert(0,"lis")]
    end.should == ["rubinius", "lisp"]
  end
  
  specify "insert with negative index should insert other string after the charracter at index" do
    example do
      ["to".insert(-1,"day"), "hosal".insert(-3,"pit")]
    end.should == ["today", "hospital"]
  end

  specify "inspect should return a quoted string suitable to eval" do
    example do
      ["Rubinius".inspect, "Ruby".inspect]
    end.should == ["\"Rubinius\"", "\"Ruby\""]
  end

  specify "intern should return the symbol corresponding to self" do
    example do
      ["intern".intern, "rubinius".intern]
    end.should == [:intern, :rubinius]
  end

  specify "length should return the length of self" do
    example do
      ["".length, "one".length, "two".length, "three".length, "four".length]
    end.should == [0, 3, 3, 5, 4]
  end

  specify "ljust should return a string left justified in width characters padded on the right" do
    example do
      [ "one".ljust(4),
        "two".ljust(5),
        "three".ljust(6),
        "four".ljust(7) ]
    end.should == ["one ", "two  ", "three ", "four   "]
  end
  
  specify "ljust should return self if length of self is greater than width" do
    example do
      "five".ljust(3)
    end.should == "five"
  end

  specify "lstrip should return self with leading whitespace characters removed" do
    example do
      [ "".lstrip,
        " hello ".lstrip,
        "hello".lstrip ]
    end.should == ["", "hello ", "hello"]
  end

  specify "lstrip! should modify self removing leading whitespace characters" do
    example do
      @s = "\n\t This \000"
      @t = " another one"
      @u = "  two  "
      [@s.lstrip!, @t.lstrip!, @u.lstrip!]
    end.should == ["This \000", "another one", "two  "]
  end
  
  specify "lstrip! should return nil if no changes were made" do
    example do
      "this".lstrip!
    end.should == nil
  end

  specify "match should convert pattern to a Regexp and invoke its match method on self" do
    example do
      "backwards".match("ack").to_s
    end.should == "ack"
  end

  specify "next should be a synonym for succ" do
    example do
      [ "abcd".succ,
        "THX1138".succ, 
        "<<koala>>".succ,
        "1999zzz".succ,
        "ZZZ9999".succ,
        "***".succ ]
    end.should == ["abce", "THX1139", "<<koalb>>", "2000aaa", "AAAA0000", "**+"]
  end

  specify "next! should be a synonym for succ!" do
    example do
      @a = "abcd"
      @b = "THX1138"
      @c = "<<koala>>"
      @d = "1999zzz"
      @e = "ZZZ9999"
      @f = "***"
      [ @a.succ!, @a,
        @b.succ!, @b,
        @c.succ!, @c,
        @d.succ!, @d,
        @e.succ!, @e,
        @f.succ!, @f ]
    end.should == ["abce", "abce", "THX1139", "THX1139", "<<koalb>>", "<<koalb>>",
        "2000aaa", "2000aaa", "AAAA0000", "AAAA0000", "**+", "**+"]
  end

  specify "oct should convert the leading characters (+/- optional) to a number base 8" do
    example do
      ["0845".oct, "7712".oct, "-012345678".oct]
    end.should == [0, 4042, -342391]
  end

  specify "replace should replace the contents of self with other" do
    example do
      @a = "me"
      @b = "you"
      ["".replace(@a), @a.replace(@b), @b.replace("we")]
    end.should == ["me", "you", "we"]
  end
  
  specify "replace should copy the taintedness of other" do
    example do
      @a = "tainted"
      @a.taint
      @b = "pure"
      @c = "replace"
      @c.replace(@a)
      [@a, @c, @c.tainted?, "".replace(@b), @b.tainted?]
    end.should == ["tainted", "tainted", true, "pure", false]
  end

  specify "reverse should reverse the order of the characters" do
    example do
      [ "0123456789".reverse,
        "M".reverse,
        "".reverse ]
    end.should == ["9876543210", "M", ""]
  end
  
  specify "reverse! should reverse the order of the characters and return self" do
    example do
      @a = "detsalb"
      @b = @a.reverse!
      [@a, @b, @a.equal?(@b)]
    end.should == ["blasted", "blasted", true]
  end
  
  specify "rindex with integer should return the last index of the character" do
    example do
      "babble".rindex(?b)
    end.should == 3
  end
  
  specify "rindex with string should return the last index of the substring" do
    example do
      "wobble".rindex("b")
    end.should == 3
  end

  specify "rindex with regexp should return the last index of the substring" do
    example do
      "abbadabba".rindex(/ba/)
    end.should == 7
  end
  
  specify "rjust should return a string right justified in width characters padded on the left" do
    example do
      [ "more".rjust(10), "less".rjust(11, '+')]
    end.should == ["      more", "+++++++less"]
  end
  
  specify "rjust should return self if length of self is greater than width" do
    example do
      "batter".rjust(4)
    end.should == "batter"
  end

  specify "rstrip should return a string first removing trailing \\000 characters and then removing trailing spaces" do
    example do
      [ " hello ".rstrip,
        "\tgoodbye\r\n".rstrip ]
    end.should == [" hello", "\tgoodbye"]
  end

  specify "rstrip! should modify self by first removing trailing \\000 characters and then removing trailing spaces " do
    example do
      @s = " hello "
      @t = "\tgoodbye\r\n"
      @u = "goodbye\000"
      [ @s.rstrip!,
        @t.rstrip!,
        @u.rstrip! ]
    end.should == [" hello", "\tgoodbye", "goodbye"]
  end
  
  specify "rstrip! should return nil if no changes are made" do
    example do
      "rstrip".rstrip!
    end.should == nil
  end
    

  specify "scan should return an array containing each substring matching pattern" do
    example do
      "hello, rubinius world".scan(/[aeo][ralf]/)
    end.should == ["el", "or"]
  end
  
  specify "scan with block should pass each substring matching pattern to block" do
    example do
      @a = []
      "abbadabbadoo".scan(/[db]./) { |s| @a << s }
      @a
    end.should == ["bb", "da", "bb", "do"]
  end

  specify "size should be a synonym for length" do
    example do
      ["".size, "one".size, "two".size, "three".size, "four".size]
    end.should == [0, 3, 3, 5, 4]
  end

  specify "slice should be a synonym for []" do
    example do
      [ "hello".slice(1),
        "hello".slice(-3, 2),
        "world".slice(-4..-2),
        "hello there".slice(/[aeiou](.)\1/),
        "hello there".slice(/[aeiou](.)\1/, 0),
        "hello".slice("bye"),
        "baz.rb".slice(2, 4),
        "z.rb".slice(1, 1) ]
    end.should == [101, "ll", "orl", "ell", "ell", nil, "z.rb", "."]
  end
  
  specify "slice! should remove and return the specified portion from self" do
    example do
      @s = "hello"
      @r = "hello"
      @t = "world"
      @u = "hello there"
      @v = "hello there"
      @w = "hello"
      @x = "baz.rb"
      @y = "z.rb"
      [ @s.slice!(1), @r.slice!(-3, 2), @t.slice!(-4..-2), @u.slice!(/[aeiou](.)\1/),
        @v.slice!(/[aeiou](.)\1/, 0), @w.slice!("bye"), @x.slice!(2, 4), @y.slice!(1, 1) ]
    end.should == [101, "ll", "orl", "ell", "ell", nil, "z.rb", "."]
  end
  
  specify "slice! with index should return nil if the value is out of range" do
    example do
      "two".slice!(4)
    end.should == nil
  end
  
  specify "slice! with range should return nil if the value is out of range" do
    example do
      # BUG?: how does 3..6 touch string? 
      ["one".slice!(3..6), "one".slice!(4..6)]
    end.should == ["", nil]
  end
  
  specify "split with no argument should return an array of substrings separated by $;" do
    example do
      [ $;.nil?,
        "hello world".split,
        "hello\frubinius".split,
        $; = 't',
        "splitme".split ]
    end.should == [true, ["hello", "world"], ["hello", "rubinius"], "t", ["spli", "me"]]
  end
  
  specify "split with string should return an array of substrings separated by string" do
    example do
      [ "hello".split('l'),
        "hello".split('e'),
        "hello".split('h'),
        "hello".split('el'),
        "hello".split('o'),
        "hello".split('d') ]
    end.should == [["he", "", "o"], ["h", "llo"], ["", "ello"], ["h", "lo"], ["hell"], ["hello"]]
  end
  
  specify "split with regexp should return an array of substrings separated by pattern" do
    example do
      [ "hello".split(/l/),
        "hello".split(/[aeiou]/),
        "hello".split(/h/),
        "hello".split(/el/),
        "hello".split(/[abcde]/),
        "hello".split(/def/) ]
    end.should == [["he", "", "o"], ["h", "ll"], ["", "ello"], ["h", "lo"], ["h", "llo"], ["hello"]]
  end

  specify "squeeze should return a string replacing runs of characters specified by the union of arguments with a single character" do
    example do
      [ "yellow moon".squeeze,
        " now is the".squeeze(" "),
        "putters putt balls".squeeze("m-z") ]
    end.should == ["yelow mon", " now is the", "puters put balls"]
  end

  specify "squeeze! should modify self in place by performing squeeze" do
    example do
      @s = "yellow moon"
      @t = "putters putt balls"
      [ @s.squeeze!, @s, @t.squeeze!("m-z"), @t ]
    end.should == ["yelow mon", "yelow mon", "puters put balls", "puters put balls"]
  end
  
  specify "squeeze! should return nil if no changes are made" do
    example do
      "squeeze".squeeze!("u", "sq")
    end.should == nil
  end

  specify "strip should return a string with trailing \\000, leading and trailing spaces removed" do
    example do
      [ " hello ".strip,
        "\tgoodbye\r\n".strip,
        "goodbye \000".strip ]
    end.should == ["hello", "goodbye", "goodbye"]
  end

  specify "strip! should modify self to remove trailing \\000, and leading and trailing spaces" do
    example do
      @s = "  strip  \000"
      @r = "  rip \000\000"
      [@s.strip!, @s, @r.strip!, @r]
    end.should == ["strip", "strip", "rip", "rip"]
  end

  specify "sub should return a string with the first occurrence of pattern replaced with string" do
    example do
      "abcdbab".sub(/ab/, "qrs")
    end.should == "qrscdbab"
  end
  
  specify "sub with block should return a string replacing the first occurrence of pattern with the value of the block" do
    example do
      "test this".sub(/test/) { |i| "spec" }
    end.should == "spec this"
  end

  specify "sub! should modify self to replace the first occurrence of pattern with string" do
    example do
      @s = "abbaabba"
      @r = "abbaabba"
      [@s.sub!(/a/,"bab"), @s, @r.sub!(/ba+b/, "cab"), @r]
    end.should == ["babbbaabba", "babbbaabba", "abcabba", "abcabba"]
  end
  
  specify "sub! should return nil if no changes are made" do
    example do
      ["abby".sub!(/ca/,"ba"), "bad".sub!("c", "b")]
    end.should == [nil, nil]
  end

  specify "succ should return the string that is the successor of self" do
    example do
      [ "abcd".succ,
        "THX1138".succ, 
        "<<koala>>".succ,
        "1999zzz".succ,
        "ZZZ9999".succ,
        "***".succ ]
    end.should == ["abce", "THX1139", "<<koalb>>", "2000aaa", "AAAA0000", "**+"]
  end

  specify "succ! should modify self to be its successor" do
    example do
      @a = "abcd"
      @b = "THX1138"
      @c = "<<koala>>"
      @d = "1999zzz"
      @e = "ZZZ9999"
      @f = "***"
      [ @a.succ!, @a,
        @b.succ!, @b,
        @c.succ!, @c,
        @d.succ!, @d,
        @e.succ!, @e,
        @f.succ!, @f ]
    end.should == ["abce", "abce", "THX1139", "THX1139", "<<koalb>>", "<<koalb>>",
        "2000aaa", "2000aaa", "AAAA0000", "AAAA0000", "**+", "**+"]
  end

  specify "sum should return a basic n-bit checksum of self with default n = 16" do
    example do
      ["ruby".sum, "ruby".sum(10), "rubinius".sum(23)]
    end.should == [450, 450, 881]
  end

  specify "swapcase should return a string with lowercase characters converted to uppercase and vice versa" do
    example do
      "MenTaLguY".swapcase
    end.should == "mENtAlGUy"
  end

  specify "swapcase! should modify self to convert lowercase characters to uppercase and vice versa" do
    example do
      @s = "MenTaLguY"
      [@s.swapcase!, @s]
    end.should == ["mENtAlGUy", "mENtAlGUy"]
  end
  
  specify "swapcase! should return nil if no changes were made" do
    example do
      "".swapcase!
    end.should == nil
  end

  specify "to_f should return convert the leading characters to a floating-point number" do
    example do
      ["0".to_f, "0.1".to_f, ".14159".to_f, "-3.14".to_f, "help".to_f, "1.upto".to_f]
    end.should == [0.0, 0.1, 0.14159, -3.14, 0.0, 1.0]
  end

  specify "to_i should convert the string to an integer base (2, 8, 10, or 16)" do
    example do
      [ "12345".to_i,
        " 12345".to_i,
        "+12345".to_i,
        "-12345".to_i,
        " ".to_i,
        "hello".to_i,
        "99 red balloons".to_i,
        "0a".to_i,
        "0a".to_i(16),
        "0b1100101".to_i(0),
        "0o1100101".to_i(0),
        "0x1100101".to_i(0),
        "01100101".to_i(0),
        "1100101".to_i(0),
        "hello".to_i,
        "1100101".to_i(2),
        "1100101".to_i(8),
        "1100101".to_i(10),
        "1100101".to_i(16) ]
    end.should == [12345, 12345, 12345, -12345, 0, 0, 99, 0, 10, 101, 294977, 17826049, 294977, 1100101, 0, 101, 294977, 1100101, 17826049]
  end
  
  specify "to_s should return self" do
    example do
      @s = "self"
      [@s, @s.to_s, @s.to_s.equal?(@s)]
    end.should == ["self", "self", true]
  end

  specify "to_str should be a synonym for to_s" do
    example do
      @s = "self"
      [@s, @s.to_s, @s.to_s.equal?(@s)]
    end.should == ["self", "self", true]
  end

  specify "to_sym should return a symbol created from self" do
    example do
      ["ruby".to_sym, "rubinius".to_sym]
    end.should == [:ruby, :rubinius]
  end

  specify "tr should replace characters in to_string with corresponding characters in from_string" do
    example do
      "Lisp".tr("Lisp", "Ruby")
    end.should == "Ruby"
  end
  
  specify "tr should accept c1-c2 notation to denote ranges of characters" do
    example do
      "123456789".tr("2-5","abcdefg")
    end.should == "1abcd6789"
  end
  
  specify "tr should accept from_string starting with ^ to denote all characters except those listed" do
    example do
      ["123456789".tr("^345", "abc"), "abcdefghijk".tr("^d-g", "9131")]
    end.should == ["cc345cccc", "111defg1111"]
  end
  
  specify "tr should pad to_string with its last character if it is short than from_string" do
    example do
      "this".tr("this","x")
    end.should == "xxxx"
  end

  specify "tr! should modify self in place by performing tr on it" do
    example do
      @s = "abcdefghijklmnopqR"
      [@s.tr!("cdefg", "12"), @s]
    end.should == ["ab12222hijklmnopqR", "ab12222hijklmnopqR"]
  end

  specify "tr_s should return a string processed according to tr with duplicate characters removed" do
    example do
      [ "Lisp".tr_s("Lisp", "Ruby"),
        "123456789".tr_s("2-5","abcdefg"),
        "this".tr_s("this","x"),
        "abcdefghijklmnopqR".tr_s("cdefg", "12") ]
    end.should == ["Ruby", "1abcd6789", "x", "ab12hijklmnopqR"]
  end

  specify "tr_s! should modify self in place by applying tr_s" do
    example do
      @a = "54321"
      @b = "Ruby"
      @c = "chocolate"
      [@a.tr_s!("432", "ab"), @a, @b.tr_s!("R", "c"), @b, @c.tr_s!("oa", ""), @c]
    end.should == ["5ab1", "5ab1", "cuby", "cuby", "chclte", "chclte"]
  end

  specify "unpack should return an array by decoding self according to the format string" do
    example do
    end.should == 'not implemented'
  end

  specify "upcase should return a string with a-z characters replaced with A-Z" do
    example do
      "123Abc456dEf".upcase
    end.should == "123ABC456DEF"
  end

  specify "upcase! should modify self in place by applying upcase" do
    example do
      @a = "MenTaLguY"
      @b = "ruby"
      [@a.upcase!, @a, @b.upcase!, @b]
    end.should == ["MENTALGUY", "MENTALGUY", "RUBY", "RUBY"]
  end
  
  specify "upcase! should return nil if no changes are made" do
    example do
      "UPCASE".upcase!
    end.should == nil
  end

  specify "upto should use String#succ to iterate from self to other passing each string to block" do
    example do
      @a = []
      "*+".upto("*3") { |s| @a << s }
      @a
    end.should == ["*+", "*,", "*-", "*.", "*/", "*0", "*1", "*2", "*3"]
  end
end
