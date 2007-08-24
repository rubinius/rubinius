require File.dirname(__FILE__) + '/../spec_helper'

# class methods
# compile, escape, last_match, new, quote, union

# ==, ===, =~, casefold?, eql?, hash, inspect, kcode, match, options,
# source, to_s, ~

regexp_new = shared "Regexp.new" do |cmd|
  describe "Regexp.#{cmd}" do
    it "new should create a new regular expression object" do
      Regexp.send(cmd, '').is_a?(Regexp).should == true
    end
  end
end

describe "Range.new" do
  it_behaves_like(regexp_new, :new)
end

describe "Range.compile" do
  it_behaves_like(regexp_new, :compile)
end

regexp_quote = shared "Regexp.quote" do |cmd|
  describe "Regexp.#{cmd}" do
    it "escapes any characters with special meaning in a regular expression" do
      Regexp.send(cmd, '\*?{}.+^[]()-').should == '\\\\\*\?\{\}\.\+\^\[\]\(\)\-'
    end
  end
end

describe "Range.escape" do
  it_behaves_like(regexp_quote, :escape)
end

describe "Range.quote" do
  it_behaves_like(regexp_quote, :quote)
end

describe "Range.last_match" do
  it "returns MatchData instance when not passed arguments" do
    /c(.)t/ =~ 'cat'
    Regexp.last_match.is_a?(MatchData).should == true
  end
  
  it "returns the nth field in this MatchData when passed a Fixnum" do
    /c(.)t/ =~ 'cat'
    Regexp.last_match(1).should == 'a'
  end
end

describe "Range.union" do
  it "returns /(?!)/ when passed no arguments" do
    Regexp.union.should == /(?!)/
  end
  
  it "returns a regular expression that will match passed arguments" do
    Regexp.union("penzance").should == /penzance/
    Regexp.union("skiing", "sledding").should == /skiing|sledding/
    Regexp.union(/dogs/, /cats/i).should == /(?-mix:dogs)|(?i-mx:cats)/
  end
end

regexp_eql = shared "Regexp#==" do |cmd|
  describe "Regexp##{cmd}" do
    it "is true if self and other have the same pattern" do
      /abc/.send(cmd, /abc/).should == true
      /abc/.send(cmd, /abd/).should == false
    end

    it "is true if self and other have the same character set code" do
      /abc/.send(cmd, /abc/x).should == false
      /abc/x.send(cmd, /abc/x).should == true
      /abc/u.send(cmd, /abc/n).should == false
      /abc/u.send(cmd, /abc/u).should == true
      /abc/n.send(cmd, /abc/n).should == true
    end

    it "is true if other has the same #casefold? values" do
      /abc/.send(cmd, /abc/i).should == false
      /abc/i.send(cmd, /abc/i).should == true
    end
  end
end

describe "Regexp#==" do
  it_behaves_like(regexp_eql, :==)
end

describe "Regexp#===" do
  it "is true if there is a match" do
    (/abc/ === "aabcc").should == true
  end
  
  it "is false if there is no match" do
    (/abc/ === "xyz").should == false
  end
end

regexp_match = shared "Regexp#=~" do |cmd|
  describe "Regexp##{cmd}" do
    it "returns the first position of the match" do
      (/(.)(.)(.)/ =~ "abc").should == 0
      $~.begin(0).should == 0
    end

    it "returns nil if there is no match" do
      /xyz/ =~ "abxyc"
      $~.should == nil
    end
  end
end

describe "Regexp#=~" do
  it_behaves_like(regexp_match, :=~)
end

describe "Regexp#casefold?" do
  it "returns the value of the case-insensitive flag" do
    /abc/i.casefold?.should == true
    /xyz/.casefold?.should == false
  end
end

describe "Regexp#eql?" do
  it_behaves_like(regexp_eql, :eql?)
end

describe "Regexp#hash" do
  it "is provided" do
    Regexp.new('').respond_to?(:hash).should == true
  end

  it "is based on the text and options of Regexp" do
    (/cat/ix.hash == /cat/ixn.hash).should == true
    (/dog/m.hash  == /dog/m.hash).should == true
    (/cat/.hash   == /cat/ix.hash).should == false
    (/cat/.hash   == /dog/.hash).should == false
  end
end

describe "Regexp#inspect" do
  it "returns a formatted string" do
    /ab+c/ix.inspect.should == "/ab+c/ix"
    /a(.)+s/n.inspect.should == "/a(.)+s/n"
  end
end

describe "Regexp#kcode" do
  it "returns the character set code" do
    /ab+c/s.kcode.should == "sjis"
    /a(.)+s/n.kcode.should == "none"
    /xyz/e.kcode.should == "euc"
    /cars/u.kcode.should == "utf8"
  end
end

describe "Regexp#match" do
  it_behaves_like(regexp_match, :match)
end

describe "Regexp#options" do
  it "returns set of bits for options used" do
    /cat/.options.should == 0
    /cat/ix.options.should == 3
    Regexp.new('cat', true).options.should == 1
    Regexp.new('cat', 0, 's').options.should == 48
  end
end

describe "Regexp#source" do
  it "returns the original string of the pattern" do
    /ab+c/ix.source.should == "ab+c"
    /x(.)xz/.source.should == "x(.)xz"
  end
end

describe "Regexp#to_s" do
  it "returns a string in (?xxx:yyy) notation" do
    /ab+c/ix.to_s.should == "(?ix-m:ab+c)"
    /xyz/.to_s.should == "(?-mix:xyz)"
    /jis/s.to_s.should == "(?-mix:jis)"
  end
end

describe "Regexp#~" do
  it "matchs against the contents of $_" do
    $_ = "input data"
    (~ /at/).should == 7
  end
end
