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
    end.should == 'not implemented'
  end
  
  specify "* should return a new string that is n copies of self" do
    example do
    end.should == 'not implemented'
  end
  
  specify "+ should concatentate self with other" do
    example do
    end.should == 'not implemented'
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
    end.should == 'not implemented'
  end
  
  specify "== with other String should return true if <=> returns 0" do
    example do
    end.should == 'not implemented'
  end
  
  specify "== with other not String should return self == other.to_str if other responds to to_str" do
    example do
    end.should == 'not implemented'
  end
  
  specify "== with other not String should return false if other does not respond to to_str" do
    example do
    end.should == 'not implemented'
  end
  
  specify "=== should be a synonym for ==" do
    example do
    end.should == 'not implemented'
  end
  
  specify "=~ should return the position of match start" do
    example do
    end.should == 'not implemented'
  end
  
  specify "=~ should return nil if there is no match" do
    example do
    end.should == 'not implemented'
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
    end.should == 'not implemented'
  end
  
  specify "[]= with index should replace the character at index with string" do
    example do
    end.should == 'not implemented'
  end
  
  specify "[]= with start, length should replace length characters at start with string" do
    example do
    end.should == 'not implemented'
  end
  
  specify "[]= with range should replace characters in range with string" do
    example do
    end.should == 'not implemented'
  end
  
  specify "[]= with regexp should replace the characters that match pattern with string" do
    example do
    end.should == 'not implemented'
  end

  specify "[]= with regexp and index should replace the specified portion of the match with string" do
    example do
    end.should == 'not implemented'
  end

  specify "[]= with string should replace the string with other string" do
    example do
    end.should == 'not implemented'
  end
  
  specify "~ should be equivalent to $_ =~ self" do
    example do
    end.should == 'not implemented'
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
      [@s.chomp!, @t.chomp!, @u.chomp!, @v.chomp!]
    end.should == ["one", "two", "three", nil]
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
    end.should == 'not implemented'
  end

  specify "count should " do
    example do
    end.should == 'not implemented'
  end

  specify "crypt should return a one-way cryptographic hash of self using the standard libary function crypt" do
    example do
    end.should == 'not implemented'
  end

  specify "delete should return a copy of self removing all characters in the intersection of its arguments" do
    example do
    end.should == 'not implemented'
  end

  specify "delete! should perform method delete in place on self" do
    example do
    end.should == 'not implemented'
  end
  
  specify "delete! should return nil if no changes are made" do
    example do
      "help".delete!("q", "r", "biff")
    end.should == nil
  end

  specify "downcase should return a copy of self with A-Z converted to a-z" do
    example do
    end.should == 'not implemented'
  end

  specify "downcase! should perform downcase in place on self" do
    example do
    end.should == 'not implemented'
  end

  specify "dump should return a string with all non-printing characters replaced with \\nnn notation" do
    example do
    end.should == 'not implemented'
  end

  specify "each should split self on the argument ($/ default) and pass each substring to block" do
    example do
    end.should == 'not implemented'
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
    end.should == 'not implemented'
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
    end.should == 'not implemented'
  end
  
  specify "gsub with block should return a string with all instances of pattern replaced by the value of block" do
    example do
    end.should == 'not implemented'
  end

  specify "gsub! should perform gsub in place on self" do
    example do
    end.should == 'not implemented'
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
    end.should == 'not implemented'
  end

  specify "index with fixnum should return the index of the given character" do
    example do
      [ "hello".index(101),
        "hello".index(52) ]
    end.should == [1, nil]
  end
  
  specify "index with string should return the index of the beginning of string" do
    example do
      [ "hello".index('e'),
        "hello".index('lo', -3),
        "hello".index('lo', -1),
        "hello".index('a') ]
    end.should == [1, 3, nil, nil]
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
      ["intern".intern, "rubinius".intern, "$4.99 for one or two".intern]
    end.should == [:intern, :rubinius, :'$4.99 for one or two']
  end

  specify "length should return the length of self" do
    example do
      ["".length, "one".length, "two".length, "three".length, "four".length]
    end.should == [0, 3, 3, 5, 4]
  end

  specify "ljust should return a string left justified in width characters padded on the right" do
    example do
    end.should == 'not implemented'
  end
  
  specify "ljust should return self if length of self is greater than width" do
    example do
    end.should == 'not implemented'
  end

  specify "lstrip should return self with leading whitespace characters removed" do
    example do
    end.should == 'not implemented'
  end

  specify "lstrip! should modify self removing leading whitespace characters" do
    example do
    end.should == 'not implemented'
  end
  
  specify "lstrip! should return nil if no changes were made" do
    example do
      "this".lstrip!
    end.should == nil
  end

  specify "match should convert pattern to a Regexp and invoke its match method on self" do
    example do
    end.should == 'not implemented'
  end

  specify "next should be a synonym for succ" do
    example do
    end.should == 'not implemented'
  end

  specify "next! should be a synonym for succ!" do
    example do
    end.should == 'not implemented'
  end

  specify "oct should convert the leading characters (+/- optional) to a number base 8" do
    example do
      ["0845".oct, "7712".oct, "-012345678".oct]
    end.should == [0, 4042, -342391]
  end

  specify "replace should replace the contents and taintedness of self with other" do
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
    end.should == 'not implemented'
  end
  
  specify "rindex with string should return the last index of the substring" do
    example do
    end.should == 'not implemented'
  end

  specify "rindex with regexp should return the last index of the substring" do
    example do
    end.should == 'not implemented'
  end
  
  specify "rjust should return a string right justified in width characters padded on the left" do
    example do
    end.should == 'not implemented'
  end
  
  specify "rjust should return self if length of self is greater than width" do
    example do
    end.should == 'not implemented'
  end

  specify "rstrip should return a string first removing trailing \\000 characters and then removing trailing spaces" do
    example do
    end.should == 'not implemented'
  end

  specify "rstrip! should modify self by first removing trailing \\000 characters and then removing trailing spaces " do
    example do
    end.should == 'not implemented'
  end

  specify "scan should return an array containing each substring matching pattern" do
    example do
    end.should == 'not implemented'
  end
  
  specify "scan with block should pass each substring matching pattern to block" do
    example do
    end.should == 'not implemented'
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
    end.should == 'not implemented'
  end
  
  specify "slice! with index should raise IndexError if the value is out of range" do
    example do
    end.should == 'not implemented'
  end
  
  specify "slice! with range should raise RangeError if the value is out of range" do
    example do
    end.should == 'not implemented'
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

  specify "squeeze should " do
    example do
    end.should == 'not implemented'
  end

  specify "squeeze! should " do
    example do
    end.should == 'not implemented'
  end

  specify "strip should return a string with trailing \\000, leading and trailing spaces removed" do
    example do
    end.should == 'not implemented'
  end

  specify "strip! should modify self to remove trailing \\000, and leading and trailing spaces" do
    example do
    end.should == 'not implemented'
  end

  specify "sub should return a string with the first occurrence of pattern replaced with string" do
    example do
    end.should == 'not implemented'
  end
  
  specify "sub with block should return a string replacing the first occurrence of pattern with the value of the block" do
    example do
    end.should == 'not implemented'
  end

  specify "sub! should modify self to replace the first occurrence of pattern with string" do
    example do
    end.should == 'not implemented'
  end
  
  specify "sub! should return nil if no changes are made" do
    example do
    end.should == 'not implemented'
  end

  specify "succ should " do
    example do
    end.should == 'not implemented'
  end

  specify "succ! should " do
    example do
    end.should == 'not implemented'
  end

  specify "sum should " do
    example do
    end.should == 'not implemented'
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

  specify "to_f should " do
    example do
    end.should == 'not implemented'
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

  specify "to_str should " do
    example do
    end.should == 'not implemented'
  end

  specify "to_sym should return a symbol created from self" do
    example do
      ["ruby".to_sym, "rubinius".to_sym, "&smack".to_sym]
    end.should == [:ruby, :rubinius, :'&smack']
  end

  specify "tr should " do
    example do
    end.should == 'not implemented'
  end

  specify "tr! should " do
    example do
    end.should == 'not implemented'
  end

  specify "tr_s should " do
    example do
    end.should == 'not implemented'
  end

  specify "tr_s! should " do
    example do
    end.should == 'not implemented'
  end

  specify "unpack should " do
    example do
    end.should == 'not implemented'
  end

  specify "upcase should " do
    example do
    end.should == 'not implemented'
  end

  specify "upcase! should " do
    example do
    end.should == 'not implemented'
  end

  specify "upto should " do
    example do
    end.should == 'not implemented'
  end
end
