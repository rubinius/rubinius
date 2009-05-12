require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Literal Regexps" do
  it "matches against $_ (last input) in a conditional if no explicit matchee provided" do
    $_ = nil

    (true if /foo/).should_not == true

    $_ = "foo"

    (true if /foo/).should == true
  end

  it "yields a Regexp" do
    /Hello/.class.should == Regexp
  end
  
  it "caches the Regexp object" do
    rs = []
    2.times do |i|
      x = 1
      rs << /foo/
    end
    rs[0].should equal(rs[1])
  end

  it "throws SyntaxError for malformed literals" do
    lambda { eval('/(/') }.should raise_error(SyntaxError)
  end

  #############################################################################
  # %r
  #############################################################################

  it "supports paired delimiters with %r" do    
    LanguageSpecs.paired_delimiters.each do |p0, p1|
      eval("%r#{p0} foo #{p1}").should == / foo /
    end
  end
  
  it "supports grouping constructs that are also paired delimiters" do    
    LanguageSpecs.paired_delimiters.each do |p0, p1|
      eval("%r#{p0} () [c]{1} #{p1}").should == / () [c]{1} /      
    end
  end
  
  it "allows second part of paired delimiters to be used as non-paired delimiters" do    
    LanguageSpecs.paired_delimiters.each do |p0, p1|
      eval("%r#{p1} foo #{p1}").should == / foo /
    end
  end
  
  it "disallows first part of paired delimiters to be used as non-paired delimiters" do    
    LanguageSpecs.paired_delimiters.each do |p0, p1|
      lambda { eval("%r#{p0} foo #{p0}") }.should raise_error(SyntaxError)
    end
  end
  
  it "supports non-paired delimiters delimiters with %r" do    
    LanguageSpecs.non_paired_delimiters.each do |c|
      eval("%r#{c} foo #{c}").should == / foo /
    end
  end
  
  it "disallows alphabets as non-paired delimiter with %r" do    
    lambda { eval('%ra foo a') }.should raise_error(SyntaxError)
  end
  
  it "disallows spaces after %r and delimiter" do    
    lambda { eval('%r !foo!') }.should raise_error(SyntaxError)
  end
  
  it "allows unescaped / to be used with %r" do
    %r[/].to_s.should == /\//.to_s
  end
  
  #############################################################################
  # Substitution
  #############################################################################
  
  it "allow substitution of strings" do
    str = "foo|bar"
    /#{str}/.should == /foo|bar/
  end

  it "allow substitution of literal regexps" do
    re = /foo|bar/
    /#{re}/.should == /(?-mix:foo|bar)/
  end
  
  it "allow substitution of any class that responds to to_s" do
    o = LanguageSpecs::ClassWith_to_s.new
    /#{o}/.should == /class_with_to_s/
  end
  
  it "throws NoMethodError on missing to_s" do
    o = LanguageSpecs::ClassWithout_to_s.new
    lambda { /#{o}/ }.should raise_error(NoMethodError)
  end
  
  it "allows substitution which mixes modifiers" do
    re = /foo/i
    /#{re} bar/m.should == /(?i-mx:foo) bar/m
  end
  
  it "allows substitution to interact with other Regexp constructs" do
    str = "foo)|(bar"
    /(#{str})/.should == /(foo)|(bar)/
    
    str = "a"
    /[#{str}-z]/.should == /[a-z]/
  end
  
  it "gives precedence to escape sequences over substitution" do
    str = "J"
    /\c#{str}/.to_s.should == '(?-mix:\c#' + '{str})'
  end  

  it "throws RegexpError for malformed substitution" do
    s = ""
    lambda { /(#{s}/ }.should raise_error(RegexpError)
    s = "("
    lambda { /#{s}/ }.should raise_error(RegexpError)
  end

  it "allows substituion in extended mode" do
    var = "#comment\n  foo  #comment\n  |  bar"
    (/#{var}/x =~ "foo").should == (/foo|bar/ =~ "foo")
  end
  
  #############################################################################
  # Specs for the matching semantics
  #############################################################################
    
  it 'supports escape characters' do
    /\t/.match("\t").to_a.should == ["\t"] # horizontal tab
    /\v/.match("\v").to_a.should == ["\v"] # vertical tab
    /\n/.match("\n").to_a.should == ["\n"] # newline
    /\r/.match("\r").to_a.should == ["\r"] # return
    /\f/.match("\f").to_a.should == ["\f"] # form feed
    /\a/.match("\a").to_a.should == ["\a"] # bell
    /\e/.match("\e").to_a.should == ["\e"] # escape

    # \nnn         octal char            (encoded byte value)
  end
  
  it 'supports quoting meta-characters via escape sequence' do
    /\\/.match("\\").to_a.should == ["\\"]
    /\//.match("/").to_a.should == ["/"]
    # parenthesis, etc
    /\(/.match("(").to_a.should == ["("]
    /\)/.match(")").to_a.should == [")"]
    /\[/.match("[").to_a.should == ["["]
    /\]/.match("]").to_a.should == ["]"]
    /\{/.match("{").to_a.should == ["{"]
    /\}/.match("}").to_a.should == ["}"]
    # alternation separator
    /\|/.match("|").to_a.should == ["|"]
    # quantifiers
    /\?/.match("?").to_a.should == ["?"]
    /\./.match(".").to_a.should == ["."]
    /\*/.match("*").to_a.should == ["*"]
    /\+/.match("+").to_a.should == ["+"]
    # line anchors
    /\^/.match("^").to_a.should == ["^"]
    /\$/.match("$").to_a.should == ["$"]    
  end
  
  it 'allows any character to be escaped' do
    /\y/.match("y").to_a.should == ["y"]  
  end
  
  it 'supports \x (hex characters)' do
    /\xA/.match("\nxyz").to_a.should == ["\n"]
    /\x0A/.match("\n").to_a.should == ["\n"]
    /\xAA/.match("\nA").should be_nil
    /\x0AA/.match("\nA").to_a.should == ["\nA"]
    /\xAG/.match("\nG").to_a.should == ["\nG"]
    # Non-matches
    lambda { eval('/\xG/') }.should raise_error(SyntaxError)
    
    # \x{7HHHHHHH} wide hexadecimal char (character code point value)
  end
  
  it 'supports \c (control characters)' do
    #/\c \c@\c`/.match("\00\00\00").to_a.should == ["\00\00\00"]
    /\c#\cc\cC/.match("\03\03\03").to_a.should == ["\03\03\03"]
    /\c'\cG\cg/.match("\a\a\a").to_a.should == ["\a\a\a"]
    /\c(\cH\ch/.match("\b\b\b").to_a.should == ["\b\b\b"]
    /\c)\cI\ci/.match("\t\t\t").to_a.should == ["\t\t\t"]
    /\c*\cJ\cj/.match("\n\n\n").to_a.should == ["\n\n\n"]
    /\c+\cK\ck/.match("\v\v\v").to_a.should == ["\v\v\v"]
    /\c,\cL\cl/.match("\f\f\f").to_a.should == ["\f\f\f"]
    /\c-\cM\cm/.match("\r\r\r").to_a.should == ["\r\r\r"]
    
    /\cJ/.match("\r").should be_nil

    # Parsing precedence
    /\cJ+/.match("\n\n").to_a.should == ["\n\n"] # Quantifers apply to entire escape sequence
    /\\cJ/.match("\\cJ").to_a.should == ["\\cJ"]
    lambda { eval('/[abc\x]/') }.should raise_error(SyntaxError) # \x is treated as a escape sequence even inside a character class
    # Syntax error
    lambda { eval('/\c/') }.should raise_error(SyntaxError)

    # \cx          control char          (character code point value)
    # \C-x         control char          (character code point value)
    # \M-x         meta  (x|0x80)        (character code point value)
    # \M-\C-x      meta control char     (character code point value)
  end
  
  it 'supports ^ (line start anchor)' do
    # Basic matching
    /^foo/.match("foo").to_a.should == ["foo"]
    /^bar/.match("foo\nbar").to_a.should == ["bar"]
    # Basic non-matching
    /^foo/.match(" foo").should be_nil
    /foo^/.match("foo\n\n\n").should be_nil
    
    # A bit advanced
    /^^^foo/.match("foo").to_a.should == ["foo"]
    (/^[^f]/ =~ "foo\n\n").should == "foo\n".size and $~.to_a.should == ["\n"]
    (/($^)($^)/ =~ "foo\n\n").should == "foo\n".size and $~.to_a.should == ["", "", ""]

    # Different start of line chars
    /^bar/.match("foo\rbar").should be_nil
    /^bar/.match("foo\0bar").should be_nil

    # Trivial
    /^/.match("foo").to_a.should == [""]

    # Grouping
    /(^foo)/.match("foo").to_a.should == ["foo", "foo"]
    /(^)/.match("foo").to_a.should == ["", ""]
    /(foo\n^)(^bar)/.match("foo\nbar").to_a.should == ["foo\nbar", "foo\n", "bar"]   
  end
  
  it 'does not match ^ after trailing \n' do
    /^(?!\A)/.match("foo\n").should be_nil # There is no (empty) line after a trailing \n
  end

  it 'supports $ (line end anchor)' do
    # Basic  matching
    /foo$/.match("foo").to_a.should == ["foo"]
    /foo$/.match("foo\nbar").to_a.should == ["foo"]
    # Basic non-matching
    /foo$/.match("foo ").should be_nil
    /$foo/.match("\n\n\nfoo").should be_nil

    # A bit advanced
    /foo$$$/.match("foo").to_a.should == ["foo"]
    (/[^o]$/ =~ "foo\n\n").should == ("foo\n".size - 1) and $~.to_a.should == ["\n"]

    # Different end of line chars
    /foo$/.match("foo\r\nbar").should be_nil
    /foo$/.match("foo\0bar").should be_nil

    # Trivial
    (/$/ =~ "foo").should == "foo".size and $~.to_a.should == [""]

    # Grouping
    /(foo$)/.match("foo").to_a.should == ["foo", "foo"]
    (/($)/ =~ "foo").should == "foo".size and $~.to_a.should == ["", ""]
    /(foo$)($\nbar)/.match("foo\nbar").to_a.should == ["foo\nbar", "foo", "\nbar"]
  end
  
  it 'supports . (any character except line terminator)' do
    # Basic matching
    /./.match("foo").to_a.should == ["f"]
    # Basic non-matching
    /./.match("").should be_nil
    /./.match("\n").should be_nil
    /./.match("\0").to_a.should == ["\0"]
  end
  
  it 'supports . with /m' do
    # Basic matching
    /./m.match("\n").to_a.should == ["\n"]
  end
  
  it 'supports \w (word character)' do
    /\w/.match("a").to_a.should == ["a"]
    /\w/.match("1").to_a.should == ["1"]
    /\w/.match("_").to_a.should == ["_"]
    
    # Non-matches
    /\w/.match(LanguageSpecs.white_spaces).should be_nil
    /\w/.match(LanguageSpecs.non_alphanum_non_space).should be_nil
    /\w/.match("\0").should be_nil
  end
  
  it 'supports \W (non-word character)' do
    /\W+/.match(LanguageSpecs.white_spaces).to_a.should == [LanguageSpecs.white_spaces]
    /\W+/.match(LanguageSpecs.non_alphanum_non_space).to_a.should == [LanguageSpecs.non_alphanum_non_space]
    /\W/.match("\0").to_a.should == ["\0"]

    # Non-matches
    /\W/.match("a").should be_nil
    /\W/.match("1").should be_nil
    /\W/.match("_").should be_nil
  end
  
  it 'supports \s (space character)' do
    /\s+/.match(LanguageSpecs.white_spaces).to_a.should == [LanguageSpecs.white_spaces]
    
    # Non-matches
    /\s/.match("a").should be_nil
    /\s/.match("1").should be_nil
    /\s/.match(LanguageSpecs.non_alphanum_non_space).should be_nil
    /\s/.match("\0").should be_nil
  end
  
  it 'supports \S (non-space character)' do    
    /\S/.match("a").to_a.should == ["a"]
    /\S/.match("1").to_a.should == ["1"]
    /\S+/.match(LanguageSpecs.non_alphanum_non_space).to_a.should == [LanguageSpecs.non_alphanum_non_space]
    /\S/.match("\0").to_a.should == ["\0"]

    # Non-matches
    /\S/.match(LanguageSpecs.white_spaces).should be_nil
  end
  
  it 'supports \d (numeric digit)' do
    /\d/.match("1").to_a.should == ["1"]
    
    # Non-matches
    /\d/.match("a").should be_nil
    /\d/.match(LanguageSpecs.white_spaces).should be_nil
    /\d/.match(LanguageSpecs.non_alphanum_non_space).should be_nil
    /\d/.match("\0").should be_nil
  end
  
  it 'supports \D (non-digit)' do   
    /\D/.match("a").to_a.should == ["a"]
    /\D+/.match(LanguageSpecs.white_spaces).to_a.should == [LanguageSpecs.white_spaces]
    /\D+/.match(LanguageSpecs.non_alphanum_non_space).to_a.should == [LanguageSpecs.non_alphanum_non_space]
    /\D/.match("\0").to_a.should == ["\0"]

    # Non-matches
    /\D/.match("1").should be_nil
  end
  
  it 'supports \A (string start anchor)' do
    # Basic matching
    /\Afoo/.match("foo").to_a.should == ["foo"]
    # Basic non-matching
    /\Abar/.match("foo\nbar").should be_nil
    /\Afoo/.match(" foo").should be_nil
    
    # A bit advanced
    /\A\A\Afoo/.match("foo").to_a.should == ["foo"]
    /(\A\Z)(\A\Z)/.match("").to_a.should == ["", "", ""]

    # Different start of line chars
    /\Abar/.match("foo\0bar").should be_nil

    # Grouping
    /(\Afoo)/.match("foo").to_a.should == ["foo", "foo"]
    /(\A)/.match("foo").to_a.should == ["", ""]
  end
  
  it 'supports \Z (string end anchor, including before trailing \n)' do
    # Basic matching
    /foo\Z/.match("foo").to_a.should == ["foo"]
    /foo\Z/.match("foo\n").to_a.should == ["foo"]
    # Basic non-matching
    /foo\Z/.match("foo\nbar").should be_nil
    /foo\Z/.match("foo ").should be_nil
    
    # A bit advanced
    /foo\Z\Z\Z/.match("foo\n").to_a.should == ["foo"]
    (/($\Z)($\Z)/ =~ "foo\n").should == "foo".size and $~.to_a.should == ["", "", ""]
    (/(\z\Z)(\z\Z)/ =~ "foo\n").should == "foo\n".size and $~.to_a.should == ["", "", ""]

    # Different end of line chars
    /foo\Z/.match("foo\0bar").should be_nil
    /foo\Z/.match("foo\r\n").should be_nil

    # Grouping
    /(foo\Z)/.match("foo").to_a.should == ["foo", "foo"]
    (/(\Z)/ =~ "foo").should == "foo".size and $~.to_a.should == ["", ""]
  end
  
  it 'supports \z (string end anchor)' do
    # Basic matching
    /foo\z/.match("foo").to_a.should == ["foo"]
    # Basic non-matching
    /foo\z/.match("foo\nbar").should be_nil
    /foo\z/.match("foo\n").should be_nil
    /foo\z/.match("foo ").should be_nil
    
    # A bit advanced
    /foo\z\z\z/.match("foo").to_a.should == ["foo"]
    (/($\z)($\z)/ =~ "foo").should == "foo".size and $~.to_a.should == ["", "", ""]

    # Different end of line chars
    /foo\z/.match("foo\0bar").should be_nil
    /foo\z/.match("foo\r\nbar").should be_nil

    # Grouping
    /(foo\z)/.match("foo").to_a.should == ["foo", "foo"]
    (/(\z)/ =~ "foo").should == "foo".size and $~.to_a.should == ["", ""]
  end
  
  it 'supports \b (word boundary)' do
    # Basic matching
    /foo\b/.match("foo").to_a.should == ["foo"]
    /foo\b/.match("foo\n").to_a.should == ["foo"]
    LanguageSpecs.white_spaces.scan(/./).each do |c|
    /foo\b/.match("foo" + c).to_a.should == ["foo"]
    end
    LanguageSpecs.non_alphanum_non_space.scan(/./).each do |c|
    /foo\b/.match("foo" + c).to_a.should == ["foo"]
    end
    /foo\b/.match("foo\0").to_a.should == ["foo"]
    # Basic non-matching
    /foo\b/.match("foobar").should be_nil
    /foo\b/.match("foo123").should be_nil
    /foo\b/.match("foo_").should be_nil
  end
  
  it 'supports \B (non-word-boundary)' do
    # Basic matching
    /foo\B/.match("foobar").to_a.should == ["foo"]
    /foo\B/.match("foo123").to_a.should == ["foo"]
    /foo\B/.match("foo_").to_a.should == ["foo"]
    # Basic non-matching
    /foo\B/.match("foo").should be_nil
    /foo\B/.match("foo\n").should be_nil
    LanguageSpecs.white_spaces.scan(/./).each do |c|
    /foo\B/.match("foo" + c).should be_nil
    end
    LanguageSpecs.non_alphanum_non_space.scan(/./).each do |c|
    /foo\B/.match("foo" + c).should be_nil
    end
    /foo\B/.match("foo\0").should be_nil
  end
  
  it 'supports [] (character class)' do
    /[a-z]+/.match("fooBAR").to_a.should == ["foo"]
    /[\b]/.match("\b").to_a.should == ["\b"] # \b inside character class is backspace
  end
  
  it 'supports [[:alpha:][:digit:][:etc:]] (predefined character classes)' do
    /[[:alnum:]]+/.match("a1").to_a.should == ["a1"]
    /[[:alpha:]]+/.match("Aa1").to_a.should == ["Aa"]
    /[[:blank:]]+/.match(LanguageSpecs.white_spaces).to_a.should == [LanguageSpecs.blanks]
    # /[[:cntrl:]]/.match("").to_a.should == [""] # TODO: what should this match?
    /[[:digit:]]/.match("1").to_a.should == ["1"]    
    # /[[:graph:]]/.match("").to_a.should == [""] # TODO: what should this match?
    /[[:lower:]]+/.match("Aa1").to_a.should == ["a"]
    /[[:print:]]+/.match(LanguageSpecs.white_spaces).to_a.should == [" "]     # include all of multibyte encoded characters
    /[[:punct:]]+/.match(LanguageSpecs.punctuations).to_a.should == [LanguageSpecs.punctuations]
    /[[:space:]]+/.match(LanguageSpecs.white_spaces).to_a.should == [LanguageSpecs.white_spaces]
    /[[:upper:]]+/.match("123ABCabc").to_a.should == ["ABC"]
    /[[:xdigit:]]+/.match("xyz0123456789ABCDEFabcdefXYZ").to_a.should == ["0123456789ABCDEFabcdef"]
    ruby_version_is "1.9" do
    eval('/[[:ascii:]]/').match("a").to_a.should == ["a"]
    end
    
    # Parsing
    /[[:lower:][:digit:]A-C]+/.match("a1ABCDEF").to_a.should == ["a1ABC"] # can be composed with other constructs in the character class
    /[^[:lower:]A-C]+/.match("abcABCDEF123def").to_a.should == ["DEF123"] # negated character class
    /[:alnum:]+/.match("a:l:n:u:m").to_a.should == ["a:l:n:u:m"] # should behave like regular character class composed of the individual letters
    /[\[:alnum:]+/.match("[:a:l:n:u:m").to_a.should == ["[:a:l:n:u:m"] # should behave like regular character class composed of the individual letters
    lambda { eval('/[[:alpha:]-[:digit:]]/') }.should raise_error(SyntaxError) # can't use character class as a start value of range
  end
  
  it 'supports ()' do
    /(a)/.match("a").to_a.should == ["a", "a"]
  end
  
  it 'supports * (0 or more of previous subexpression)' do
    /a*/.match("aaa").to_a.should == ["aaa"]
    /a*/.match("bbb").to_a.should == [""]
    /<.*>/.match("<a>foo</a>").to_a.should == ["<a>foo</a>"] # it is greedy
  end
  
  it 'supports *? (0 or more of previous subexpression - lazy)' do
    /a*?/.match("aaa").to_a.should == [""]
    /<.*?>/.match("<a>foo</a>").to_a.should == ["<a>"]
  end
  
  it 'supports + (1 or more of previous subexpression)' do
    /a+/.match("aaa").to_a.should == ["aaa"]
    /a+/.match("bbb").should be_nil
    /<.+>/.match("<a>foo</a>").to_a.should == ["<a>foo</a>"] # it is greedy
  end
  
  it 'supports +? (0 or more of previous subexpression - lazy)' do
    /a+?/.match("aaa").to_a.should == ["a"]
    /<.+?>/.match("<a>foo</a>").to_a.should == ["<a>"]
  end
  
  it 'supports {m,n} (m to n of previous subexpression)' do
    /a{2,4}/.match("aaaaaa").to_a.should == ["aaaa"]
    /<.{1,}>/.match("<a>foo</a>").to_a.should == ["<a>foo</a>"] # it is greedy
  end
  
  it 'supports {m,n}? (m to n of previous subexpression) - lazy)' do
    /<.{1,}?>/.match("<a>foo</a>").to_a.should == ["<a>"]
    not_compliant_on :ironruby do # CLR issue
    /.([0-9]){3,5}?foo/.match("9876543210foo").to_a.should == ["543210foo", "0"]
    end
  end
  
  it 'does not treat {m,n}+ as possessive' do
    /foo(A{0,1}+)Abar/.match("fooAAAbar").to_a.should == ["fooAAAbar", "AA"]
  end
  
  it 'supports ? (0 or 1 of previous subexpression)' do
    /a?/.match("aaa").to_a.should == ["a"]
    /a?/.match("bbb").to_a.should == [""]
  end
  
  it 'supports | (alternations)' do
    /a|b/.match("a").to_a.should == ["a"]
  end
  
  it 'supports (?= ) (positive lookahead)' do
    /foo.(?=bar)/.match("foo1 foo2bar").to_a.should == ["foo2"]
  end
  
  it 'supports (?! ) (negative lookahead)' do
    /foo.(?!bar)/.match("foo1bar foo2").to_a.should == ["foo2"]
  end
  
  it 'supports (?> ) (embedded subexpression)' do
    /(?>foo)(?>bar)/.match("foobar").to_a.should == ["foobar"]
    /(?>foo*)obar/.match("foooooooobar").should be_nil # it is possesive
  end
  
  it 'supports (?: ) (non-capturing group)' do
    /(?:foo)(bar)/.match("foobar").to_a.should == ["foobar", "bar"]
    # Parsing precedence
    /(?:xdigit:)/.match("xdigit:").to_a.should == ["xdigit:"]
  end
  
  it 'supports (?imx-imx) (inline modifiers)' do
    /(?i)foo/.match("FOO").to_a.should == ["FOO"]
    /foo(?i)/.match("FOO").should be_nil
    # Interaction with /i
    /(?-i)foo/i.match("FOO").should be_nil
    /foo(?-i)/i.match("FOO").to_a.should == ["FOO"]
    # Multiple uses
    /foo (?i)bar (?-i)baz/.match("foo BAR baz").to_a.should == ["foo BAR baz"]
    /foo (?i)bar (?-i)baz/.match("foo BAR BAZ").should be_nil
    
    /(?m)./.match("\n").to_a.should == ["\n"]
    /.(?m)/.match("\n").should be_nil
    # Interaction with /m
    /(?-m)./m.match("\n").should be_nil
    /.(?-m)/m.match("\n").to_a.should == ["\n"]
    # Multiple uses
    /. (?m). (?-m)./.match(". \n .").to_a.should == [". \n ."]
    /. (?m). (?-m)./.match(". \n \n").should be_nil
      
    /(?x) foo /.match("foo").to_a.should == ["foo"]
    / foo (?x)/.match("foo").should be_nil
    # Interaction with /x
    /(?-x) foo /x.match("foo").should be_nil
    / foo (?-x)/x.match("foo").to_a.should == ["foo"]
    # Multiple uses
    /( foo )(?x)( bar )(?-x)( baz )/.match(" foo bar baz ").to_a.should == [" foo bar baz ", " foo ", "bar", " baz "]
    /( foo )(?x)( bar )(?-x)( baz )/.match(" foo barbaz").should be_nil
    
    # Parsing
    /(?i-i)foo/.match("FOO").should be_nil
    /(?ii)foo/.match("FOO").to_a.should == ["FOO"]
    /(?-)foo/.match("foo").to_a.should == ["foo"]
    lambda { eval('/(?a)/') }.should raise_error(SyntaxError)
    lambda { eval('/(?o)/') }.should raise_error(SyntaxError)
  end
  
  it 'supports (?imx-imx:expr) (scoped inline modifiers)' do
    /foo (?i:bar) baz/.match("foo BAR baz").to_a.should == ["foo BAR baz"]
    /foo (?i:bar) baz/.match("foo BAR BAZ").should be_nil   
    /foo (?-i:bar) baz/i.match("foo BAR BAZ").should be_nil

    /. (?m:.) ./.match(". \n .").to_a.should == [". \n ."]
    /. (?m:.) ./.match(". \n \n").should be_nil
    /. (?-m:.) ./m.match("\n \n \n").should be_nil
      
    /( foo )(?x: bar )( baz )/.match(" foo bar baz ").to_a.should == [" foo bar baz ", " foo ", " baz "]
    /( foo )(?x: bar )( baz )/.match(" foo barbaz").should be_nil
    /( foo )(?-x: bar )( baz )/x.match("foo bar baz").to_a.should == ["foo bar baz", "foo", "baz"]
    
    # Parsing
    /(?i-i:foo)/.match("FOO").should be_nil
    /(?ii:foo)/.match("FOO").to_a.should == ["FOO"]
    /(?-:)foo/.match("foo").to_a.should == ["foo"]
    lambda { eval('/(?a:)/') }.should raise_error(SyntaxError)
    lambda { eval('/(?o:)/') }.should raise_error(SyntaxError)
  end
  
  it 'supports (?# )' do
    /foo(?#comment)bar/.match("foobar").to_a.should == ["foobar"]
    /foo(?#)bar/.match("foobar").to_a.should == ["foobar"]
  end
  
  it 'supports \<n> (backreference to previous group match)' do
    /(foo.)\1/.match("foo1foo1").to_a.should == ["foo1foo1", "foo1"]
    /(foo.)\1/.match("foo1foo2").should be_nil
  end
  
  not_compliant_on :ironruby do
    it 'resets nested \<n> backreference before match of outer subexpression' do
      /(a\1?){2}/.match("aaaa").to_a.should == ["aa", "a"]
    end
  end
  
  #############################################################################
  # Modifiers
  #############################################################################

  it 'supports /i (case-insensitive)' do
    /foo/i.match("FOO").to_a.should == ["FOO"]
  end
  
  it 'supports /m (multiline)' do
    /foo.bar/m.match("foo\nbar").to_a.should == ["foo\nbar"]
    /foo.bar/.match("foo\nbar").should be_nil
  end
  
  it 'supports /x (extended syntax)' do
    /\d +/x.match("abc123").to_a.should == ["123"] # Quantifiers can be separated from the expression they apply to
  end
  
  it 'supports /o (once)' do
    2.times do |i|
      /#{i}/o.should == /0/
    end
  end
  
  it 'invokes substitutions for /o only once' do
    ScratchPad.record []
    to_s_callback = Proc.new do
      ScratchPad << :to_s_callback
      "class_with_to_s"
    end
    o = LanguageSpecs::ClassWith_to_s.new(to_s_callback)
    2.times { /#{o}/o }
    ScratchPad.recorded.should == [:to_s_callback]
  end
  
  it 'does not do thread synchronization for /o' do
    ScratchPad.record []
    
    to_s_callback2 = Proc.new do
      ScratchPad << :to_s_callback2
      "class_with_to_s2"
    end

    to_s_callback1 = Proc.new do
      ScratchPad << :to_s_callback1
      t2 = Thread.new do
        o2 = LanguageSpecs::ClassWith_to_s.new(to_s_callback2)
        ScratchPad << LanguageSpecs.get_regexp_with_substitution(o2)
      end
      t2.join
      "class_with_to_s1"
    end
    
    o1 = LanguageSpecs::ClassWith_to_s.new(to_s_callback1)
    ScratchPad << LanguageSpecs.get_regexp_with_substitution(o1)

    ScratchPad.recorded.should == [:to_s_callback1, :to_s_callback2, /class_with_to_s2/, /class_with_to_s2/]
  end
  
  it 'supports modifier combinations' do
    /foo/imox.match("foo").to_a.should == ["foo"]
    /foo/imoximox.match("foo").to_a.should == ["foo"]

    lambda { eval('/foo/a') }.should raise_error(SyntaxError)
  end
  
  #############################################################################
  # Encodings
  #############################################################################

  not_compliant_on :ruby19 do
    it 'supports /e (EUC encoding)' do
      /./e.match("\303\251").to_a.should == ["\303\251"]
    end
    
    it 'supports /n (Normal encoding)' do
      /./n.match("\303\251").to_a.should == ["\303"]
    end
    
    it 'supports /s (SJIS encoding)' do
      /./s.match("\303\251").to_a.should == ["\303"]
    end
    
    it 'supports /u (UTF8 encoding)' do
      /./u.match("\303\251").to_a.should == ["\303\251"]
    end
    
    it 'selects last of multiple encoding specifiers' do
      /foo/ensuensuens.should == /foo/s
    end
  end

  #############################################################################
  # Back-refs
  #############################################################################

  it 'saves match data in the $~ pseudo-global variable' do
    "hello" =~ /l+/
    $~.to_a.should == ["ll"]
  end

  it 'saves captures in numbered $[1-9] variables' do
    "1234567890" =~ /(1)(2)(3)(4)(5)(6)(7)(8)(9)(0)/
    $~.to_a.should == ["1234567890", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"]
    $1.should == "1"
    $2.should == "2"
    $3.should == "3"
    $4.should == "4"
    $5.should == "5"
    $6.should == "6"
    $7.should == "7"
    $8.should == "8"
    $9.should == "9"
  end

  it 'will not clobber capture variables across threads' do
    cap1, cap2, cap3 = nil
    "foo" =~ /(o+)/
    cap1 = [$~.to_a, $1]
    Thread.new do
      cap2 = [$~.to_a, $1]
      "bar" =~ /(a)/
      cap3 = [$~.to_a, $1]
    end.join
    cap4 = [$~.to_a, $1]
    cap1.should == [["oo", "oo"], "oo"]
    cap2.should == [[], nil]
    cap3.should == [["a", "a"], "a"]
    cap4.should == [["oo", "oo"], "oo"]
  end
end

language_version __FILE__, "regexp"
