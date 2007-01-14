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
  
  specify "== should " do
    example do
    end.should == 'not implemented'
  end
  
  specify "=~ should " do
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
  
  specify "[] with string should " do
    example do
      [ "hello"["el"],
        "hello"["bye"],
        "hello"["hello"] ]
    end.should == ["el", nil, "hello"]
  end
  
  specify "[]= should " do
    example do
    end.should == 'not implemented'
  end

  specify "capitalize should " do
    example do
    end.should == 'not implemented'
  end
  
  specify "capitalize! should " do
    example do
    end.should == 'not implemented'
  end

  specify "casecmp should " do
    example do
    end.should == 'not implemented'
  end

  specify "center should " do
    example do
    end.should == 'not implemented'
  end

  specify "chomp should " do
    example do
    end.should == 'not implemented'
  end

  specify "chomp! should " do
    example do
    end.should == 'not implemented'
  end

  specify "chop should " do
    example do
    end.should == 'not implemented'
  end

  specify "chop! should " do
    example do
    end.should == 'not implemented'
  end

  specify "concat should " do
    example do
    end.should == 'not implemented'
  end

  specify "count should " do
    example do
    end.should == 'not implemented'
  end

  specify "crypt should " do
    example do
    end.should == 'not implemented'
  end

  specify "delete should " do
    example do
    end.should == 'not implemented'
  end

  specify "delete! should " do
    example do
    end.should == 'not implemented'
  end

  specify "downcase should " do
    example do
    end.should == 'not implemented'
  end

  specify "downcase! should " do
    example do
    end.should == 'not implemented'
  end

  specify "dump should " do
    example do
    end.should == 'not implemented'
  end

  specify "each should " do
    example do
    end.should == 'not implemented'
  end

  specify "each_byte should " do
    example do
    end.should == 'not implemented'
  end

  specify "each_char should " do
    example do
    end.should == 'not implemented'
  end

  specify "each_line should " do
    example do
    end.should == 'not implemented'
  end

  specify "empty? should " do
    example do
    end.should == 'not implemented'
  end

  specify "eql? should " do
    example do
    end.should == 'not implemented'
  end

  specify "gsub should " do
    example do
    end.should == 'not implemented'
  end

  specify "gsub! should " do
    example do
    end.should == 'not implemented'
  end

  specify "hash should " do
    example do
    end.should == 'not implemented'
  end

  specify "hex should " do
    example do
    end.should == 'not implemented'
  end

  specify "import should " do
    example do
    end.should == 'not implemented'
  end

  specify "include? should " do
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

  specify "initialize_copy should " do
    example do
    end.should == 'not implemented'
  end

  specify "insert should " do
    example do
    end.should == 'not implemented'
  end

  specify "inspect should " do
    example do
    end.should == 'not implemented'
  end

  specify "intern should " do
    example do
    end.should == 'not implemented'
  end

  specify "length should " do
    example do
    end.should == 'not implemented'
  end

  specify "ljust should " do
    example do
    end.should == 'not implemented'
  end

  specify "lstrip should " do
    example do
    end.should == 'not implemented'
  end

  specify "lstrip! should " do
    example do
    end.should == 'not implemented'
  end

  specify "match should " do
    example do
    end.should == 'not implemented'
  end

  specify "next should " do
    example do
    end.should == 'not implemented'
  end

  specify "next! should " do
    example do
    end.should == 'not implemented'
  end

  specify "oct should " do
    example do
    end.should == 'not implemented'
  end

  specify "quote should " do
    example do
    end.should == 'not implemented'
  end

  specify "replace should " do
    example do
    end.should == 'not implemented'
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

  specify "rindex should " do
    example do
    end.should == 'not implemented'
  end

  specify "rjust should " do
    example do
    end.should == 'not implemented'
  end

  specify "rstrip should " do
    example do
    end.should == 'not implemented'
  end

  specify "rstrip! should " do
    example do
    end.should == 'not implemented'
  end

  specify "scan should " do
    example do
    end.should == 'not implemented'
  end

  specify "scanf should " do
    example do
    end.should == 'not implemented'
  end

  specify "size should " do
    example do
    end.should == 'not implemented'
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
  
  specify "slice! should " do
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

  specify "strip should " do
    example do
    end.should == 'not implemented'
  end

  specify "strip! should " do
    example do
    end.should == 'not implemented'
  end

  specify "sub should " do
    example do
    end.should == 'not implemented'
  end

  specify "sub! should " do
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

  specify "swapcase should " do
    example do
    end.should == 'not implemented'
  end

  specify "swapcase! should " do
    example do
    end.should == 'not implemented'
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
  
  specify "to_s should " do
    example do
    end.should == 'not implemented'
  end

  specify "to_str should " do
    example do
    end.should == 'not implemented'
  end

  specify "to_sym should " do
    example do
    end.should == 'not implemented'
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
