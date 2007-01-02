require File.dirname(__FILE__) + '/../spec_helper'

# class methods
# compile, escape, last_match, new, quote, union

# ==, ===, =~, casefold?, eql?, hash, inspect, kcode, match, options,
# source, to_s, ~

context "Regexp class method" do
  specify "compile should be a synonym for new" do
    example do
      p Regexp.compile('').is_a?(Regexp)
    end.should == 'true'
  end
  
  specify "escape should escape any characters with special meaning in a regular expression" do
    example do
      puts Regexp.escape('\*?{}.+^[]()-')
    end.should == '\\\\\*\?\{\}\.\+\^\[\]\(\)\-'
  end
  
  specify "last_match with no argument should return MatchData" do
    example do
      /c(.)t/ =~ 'cat'
      puts Regexp.last_match.is_a?(MatchData)
    end.should == 'true'
  end
  
  specify "last_match with a fixnum argument should return the nth field in this MatchData" do
    example do
      /c(.)t/ =~ 'cat'
      puts Regexp.last_match(1)
    end.should == 'a'
  end
  
  specify "new should create a new regular expression object" do
    example do
      p Regexp.new('').is_a?(Regexp)
    end.should == 'true'
  end
  
  specify "quote should be a synonym for escape" do
    example do
      puts Regexp.escape('\*?{}.+^[]()-')
    end.should == '\\\\\*\?\{\}\.\+\^\[\]\(\)\-'
  end
  
  specify "union with no arguments should return /(?!)/" do
    example do
      puts Regexp.union
    end.should == '(?-mix:(?!))'
  end
  
  specify "union with arguments should return a regular expression that will match any part" do
    example do
      p [Regexp.union("penzance"), Regexp.union("skiing", "sledding"), Regexp.union(/dogs/, /cats/i)]
    end.should == '[/penzance/, /skiing|sledding/, /(?-mix:dogs)|(?i-mx:cats)/]'
  end
end

context "Regexp instance method" do
  specify "== should be true if self and other have the same pattern, character set code, and casefold? values" do
    example do
      p [/abc/ == /abc/, /abc/ == /abc/x, /abc/ == /abc/i, /abc/u == /abc/n]
    end.should == '[true, false, false, false]'
  end
  
  specify "=== should be true if there is a match" do
    example do
      p /abc/ === "aabcc"
    end.should == 'true'
  end
  
  specify "=== should be false if there is no match" do
    example do
      p /abc/ === "xyz"
    end.should == 'false'
  end
  
  specify "=~ should return a MatchData object describing the match" do
    example do
      /(.)(.)(.)/ =~ "abc"
      puts $~[2]
    end.should == 'b'
  end
  
  specify "=~ should return nil if there is no match" do
    example do
      /xyz/ =~ "abxyc"
      p $~
    end.should == 'nil'
  end
  
  specify "casefold? should return the value of the case-insensitive flag" do
    example do
      p [/abc/i.casefold?, /xyz/.casefold?]
    end.should == '[true, false]'
  end
  
  specify "eql? should be a synonym for ==" do
    example do
      p [/abc/.eql?(/abc/), /abc/.eql?(/abc/x), /abc/.eql?(/abc/i), /abc/u.eql?(/abc/n)]
    end.should == '[true, false, false, false]'
  end
  
  specify "should provide hash" do
    example do
      p Regexp.new('').respond_to?(:hash)
    end.should == 'true'
  end

  specify "inspect should produce a formatted string" do
    example do
      p [/ab+c/ix.inspect, /a(.)+s/n.inspect]
    end.should == '["/ab+c/ix", "/a(.)+s/n"]'
  end

  specify "kcode should return the character set code" do
    example do
      p [/ab+c/s.kcode, /a(.)+s/n.kcode, /xyz/e.kcode, /cars/u.kcode]
    end.should == '["sjis", "none", "euc", "utf8"]'
  end

  specify "match should be a synonym for =~" do
    example do
      p [/(.)(.)(.)/.match("abc")[2], /xyz/.match("abxyc")]
    end.should == '["b", nil]'
  end

  specify "options should return set of bits for options used" do
    example do
      p [ /cat/.options,
          /cat/ix.options,
          Regexp.new('cat', true).options,
          Regexp.new('cat', 0, 's').options ]
    end.should == '[0, 3, 1, 48]'
  end

  specify "source should return the original string of the pattern" do
    example do
      p [/ab+c/ix.source, /x(.)xz/.source]
    end.should == '["ab+c", "x(.)xz"]'
  end

  specify "to_s should return a string in (?xxx:yyy) notation" do
    example do
      p [/ab+c/ix.to_s, /xyz/.to_s, /jis/s.to_s]
    end.should == '["(?ix-m:ab+c)", "(?-mix:xyz)", "(?-mix:jis)"]'
  end

  specify "~ should match against the contents of $_" do
    example do
      $_ = "input data"
      puts ~ /at/
    end.should == '7'
  end
end
