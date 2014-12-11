# -*- encoding: ascii-8bit -*-

describe :regexp_new, :shared => true do
  it "requires one argument and creates a new regular expression object" do
    Regexp.send(@method, '').is_a?(Regexp).should == true
  end

  it "works by default for subclasses with overridden #initialize" do
    class RegexpSpecsSubclass < Regexp
      def initialize(*args)
        super
        @args = args
      end

      attr_accessor :args
    end

    class RegexpSpecsSubclassTwo < Regexp; end

    RegexpSpecsSubclass.send(@method, "hi").should be_kind_of(RegexpSpecsSubclass)
    RegexpSpecsSubclass.send(@method, "hi").args.first.should == "hi"

    RegexpSpecsSubclassTwo.send(@method, "hi").should be_kind_of(RegexpSpecsSubclassTwo)
  end
end

describe :regexp_new_string, :shared => true do
  it "uses the String argument as an unescaped literal to construct a Regexp object" do
    Regexp.send(@method, "^hi{2,3}fo.o$").should == /^hi{2,3}fo.o$/
  end

  it "raises a RegexpError when passed an incorrect regexp" do
    lambda { Regexp.send(@method, "^[$", 0) }.should raise_error(RegexpError)
  end

  it "does not set Regexp options if only given one argument" do
    r = Regexp.send(@method, 'Hi')
    (r.options & Regexp::IGNORECASE).should     == 0
    (r.options & Regexp::MULTILINE).should      == 0
    (r.options & Regexp::EXTENDED).should       == 0
  end

  it "does not set Regexp options if second argument is nil or false" do
    r = Regexp.send(@method, 'Hi', nil)
    (r.options & Regexp::IGNORECASE).should     == 0
    (r.options & Regexp::MULTILINE).should      == 0
    (r.options & Regexp::EXTENDED).should       == 0

    r = Regexp.send(@method, 'Hi', false)
    (r.options & Regexp::IGNORECASE).should     == 0
    (r.options & Regexp::MULTILINE).should      == 0
    (r.options & Regexp::EXTENDED).should       == 0
  end

  it "sets options from second argument if it is one of the Fixnum option constants" do
    r = Regexp.send(@method, 'Hi', Regexp::IGNORECASE)
    (r.options & Regexp::IGNORECASE).should_not == 0
    (r.options & Regexp::MULTILINE).should      == 0
    (r.options & Regexp::EXTENDED).should       == 0

    r = Regexp.send(@method, 'Hi', Regexp::MULTILINE)
    (r.options & Regexp::IGNORECASE).should     == 0
    (r.options & Regexp::MULTILINE).should_not  == 0
    (r.options & Regexp::EXTENDED).should       == 0

    r = Regexp.send(@method, 'Hi', Regexp::EXTENDED)
    (r.options & Regexp::IGNORECASE).should     == 0
    (r.options & Regexp::MULTILINE).should      == 0
    (r.options & Regexp::EXTENDED).should_not   == 1
  end

  it "accepts a Fixnum of two or more options ORed together as the second argument" do
    r = Regexp.send(@method, 'Hi', Regexp::IGNORECASE | Regexp::EXTENDED)
    (r.options & Regexp::IGNORECASE).should_not == 0
    (r.options & Regexp::MULTILINE).should      == 0
    (r.options & Regexp::EXTENDED).should_not   == 0
  end

  it "treats any non-Fixnum, non-nil, non-false second argument as IGNORECASE" do
    r = Regexp.send(@method, 'Hi', Object.new)
    (r.options & Regexp::IGNORECASE).should_not == 0
    (r.options & Regexp::MULTILINE).should      == 0
    (r.options & Regexp::EXTENDED).should       == 0
  end

  it "does not enable multibyte support by default" do
    r = Regexp.send @method, 'Hi', true
    r.kcode.should_not == 'euc'
    r.kcode.should_not == 'sjis'
    r.kcode.should_not == 'utf8'
  end

  it "enables EUC encoding if third argument is 'e' or 'euc' (case-insensitive)" do
    Regexp.send(@method, 'Hi', nil, 'e').kcode.should     == 'euc'
    Regexp.send(@method, 'Hi', nil, 'E').kcode.should     == 'euc'
    Regexp.send(@method, 'Hi', nil, 'euc').kcode.should   == 'euc'
    Regexp.send(@method, 'Hi', nil, 'EUC').kcode.should   == 'euc'
    Regexp.send(@method, 'Hi', nil, 'EuC').kcode.should   == 'euc'
  end

  it "enables SJIS encoding if third argument is 's' or 'sjis' (case-insensitive)" do
    Regexp.send(@method, 'Hi', nil, 's').kcode.should     == 'sjis'
    Regexp.send(@method, 'Hi', nil, 'S').kcode.should     == 'sjis'
    Regexp.send(@method, 'Hi', nil, 'sjis').kcode.should  == 'sjis'
    Regexp.send(@method, 'Hi', nil, 'SJIS').kcode.should  == 'sjis'
    Regexp.send(@method, 'Hi', nil, 'sJiS').kcode.should  == 'sjis'
  end

  it "enables UTF-8 encoding if third argument is 'u' or 'utf8' (case-insensitive)" do
    Regexp.send(@method, 'Hi', nil, 'u').kcode.should     == 'utf8'
    Regexp.send(@method, 'Hi', nil, 'U').kcode.should     == 'utf8'
    Regexp.send(@method, 'Hi', nil, 'utf8').kcode.should  == 'utf8'
    Regexp.send(@method, 'Hi', nil, 'UTF8').kcode.should  == 'utf8'
    Regexp.send(@method, 'Hi', nil, 'uTf8').kcode.should  == 'utf8'
  end

  it "disables multibyte support if third argument is 'n' or 'none' (case insensitive)" do
    Regexp.send(@method, 'Hi', nil, 'N').kcode.should == 'none'
    Regexp.send(@method, 'Hi', nil, 'n').kcode.should == 'none'
    Regexp.send(@method, 'Hi', nil, 'nONE').kcode.should == 'none'
    Regexp.send(@method, '[^\\x0d\\x22\\x5c\\x80-\\xff]', nil, 'n').kcode.should == 'none'
  end

  describe "with escaped characters" do
    it "raises a Regexp error if there is a trailing backslash" do
      lambda { Regexp.send(@method, "\\") }.should raise_error(RegexpError)
    end

    it "accepts a backspace followed by a character" do
      Regexp.send(@method, "\\N").should == /#{"\x5cN"}/
    end

    it "accepts a one-digit octal value" do
      Regexp.send(@method, "\0").should == /#{"\x00"}/
    end

    it "accepts a two-digit octal value" do
      Regexp.send(@method, "\11").should == /#{"\x09"}/
    end

    it "accepts a three-digit octal value" do
      Regexp.send(@method, "\315").should == /#{"\xcd"}/
    end

    it "interprets a digit following a three-digit octal value as a character" do
      Regexp.send(@method, "\3762").should == /#{"\xfe2"}/
    end

    it "accepts a one-digit hexadecimal value" do
      Regexp.send(@method, "\x9n").should == /#{"\x09n"}/
    end

    it "accepts a two-digit hexadecimal value" do
      Regexp.send(@method, "\x23").should == /#{"\x23"}/
    end

    it "interprets a digit following a two-digit hexadecimal value as a character" do
      Regexp.send(@method, "\x420").should == /#{"\x420"}/
    end

    # TODO: Add version argument to compliance guards
    not_supported_on :rubinius do
      it "raises a RegexpError if \\x is not followed by any hexadecimal digits" do
        lambda { Regexp.send(@method, "\\" + "xn") }.should raise_error(RegexpError)
      end
    end

    it "accepts an escaped string interpolation" do
      Regexp.send(@method, "\#{abc}").should == /#{"\#{abc}"}/
    end

    it "accepts '\\n'" do
      Regexp.send(@method, "\n").should == /#{"\x0a"}/
    end

    it "accepts '\\t'" do
      Regexp.send(@method, "\t").should == /#{"\x09"}/
    end

    it "accepts '\\r'" do
      Regexp.send(@method, "\r").should == /#{"\x0d"}/
    end

    it "accepts '\\f'" do
      Regexp.send(@method, "\f").should == /#{"\x0c"}/
    end

    it "accepts '\\v'" do
      Regexp.send(@method, "\v").should == /#{"\x0b"}/
    end

    it "accepts '\\a'" do
      Regexp.send(@method, "\a").should == /#{"\x07"}/
    end

    it "accepts '\\e'" do
      Regexp.send(@method, "\e").should == /#{"\x1b"}/
    end

    it "accepts '\\C-\\n'" do
      Regexp.send(@method, "\C-\n").should == /#{"\x0a"}/
    end

    it "accepts '\\C-\\t'" do
      Regexp.send(@method, "\C-\t").should == /#{"\x09"}/
    end

    it "accepts '\\C-\\r'" do
      Regexp.send(@method, "\C-\r").should == /#{"\x0d"}/
    end

    it "accepts '\\C-\\f'" do
      Regexp.send(@method, "\C-\f").should == /#{"\x0c"}/
    end

    it "accepts '\\C-\\v'" do
      Regexp.send(@method, "\C-\v").should == /#{"\x0b"}/
    end

    it "accepts '\\C-\\a'" do
      Regexp.send(@method, "\C-\a").should == /#{"\x07"}/
    end

    it "accepts '\\C-\\e'" do
      Regexp.send(@method, "\C-\e").should == /#{"\x1b"}/
    end

    it "accepts '\\c\\n'" do
      Regexp.send(@method, "\C-\n").should == /#{"\x0a"}/
    end

    it "accepts '\\c\\t'" do
      Regexp.send(@method, "\C-\t").should == /#{"\x09"}/
    end

    it "accepts '\\c\\r'" do
      Regexp.send(@method, "\C-\r").should == /#{"\x0d"}/
    end

    it "accepts '\\c\\f'" do
      Regexp.send(@method, "\C-\f").should == /#{"\x0c"}/
    end

    it "accepts '\\c\\v'" do
      Regexp.send(@method, "\C-\v").should == /#{"\x0b"}/
    end

    it "accepts '\\c\\a'" do
      Regexp.send(@method, "\C-\a").should == /#{"\x07"}/
    end

    it "accepts '\\c\\e'" do
      Regexp.send(@method, "\C-\e").should == /#{"\x1b"}/
    end

    it "accepts '\\M-\\n'" do
      Regexp.send(@method, "\M-\n").should == /#{"\x8a"}/
    end

    it "accepts '\\M-\\t'" do
      Regexp.send(@method, "\M-\t").should == /#{"\x89"}/
    end

    it "accepts '\\M-\\r'" do
      Regexp.send(@method, "\M-\r").should == /#{"\x8d"}/
    end

    it "accepts '\\M-\\f'" do
      Regexp.send(@method, "\M-\f").should == /#{"\x8c"}/
    end

    it "accepts '\\M-\\v'" do
      Regexp.send(@method, "\M-\v").should == /#{"\x8b"}/
    end

    it "accepts '\\M-\\a'" do
      Regexp.send(@method, "\M-\a").should == /#{"\x87"}/
    end

    it "accepts '\\M-\\e'" do
      Regexp.send(@method, "\M-\e").should == /#{"\x9b"}/
    end

    it "accepts '\\M-\\C-\\n'" do
      Regexp.send(@method, "\M-\n").should == /#{"\x8a"}/
    end

    it "accepts '\\M-\\C-\\t'" do
      Regexp.send(@method, "\M-\t").should == /#{"\x89"}/
    end

    it "accepts '\\M-\\C-\\r'" do
      Regexp.send(@method, "\M-\r").should == /#{"\x8d"}/
    end

    it "accepts '\\M-\\C-\\f'" do
      Regexp.send(@method, "\M-\f").should == /#{"\x8c"}/
    end

    it "accepts '\\M-\\C-\\v'" do
      Regexp.send(@method, "\M-\v").should == /#{"\x8b"}/
    end

    it "accepts '\\M-\\C-\\a'" do
      Regexp.send(@method, "\M-\a").should == /#{"\x87"}/
    end

    it "accepts '\\M-\\C-\\e'" do
      Regexp.send(@method, "\M-\e").should == /#{"\x9b"}/
    end

    it "accepts '\\M-\\c\\n'" do
      Regexp.send(@method, "\M-\n").should == /#{"\x8a"}/
    end

    it "accepts '\\M-\\c\\t'" do
      Regexp.send(@method, "\M-\t").should == /#{"\x89"}/
    end

    it "accepts '\\M-\\c\\r'" do
      Regexp.send(@method, "\M-\r").should == /#{"\x8d"}/
    end

    it "accepts '\\M-\\c\\f'" do
      Regexp.send(@method, "\M-\f").should == /#{"\x8c"}/
    end

    it "accepts '\\M-\\c\\v'" do
      Regexp.send(@method, "\M-\v").should == /#{"\x8b"}/
    end

    it "accepts '\\M-\\c\\a'" do
      Regexp.send(@method, "\M-\a").should == /#{"\x87"}/
    end

    it "accepts '\\M-\\c\\e'" do
      Regexp.send(@method, "\M-\e").should == /#{"\x9b"}/
    end

    it "accepts multiple consecutive '\\' characters" do
      Regexp.send(@method, "\\\\\\N").should == /#{"\\\\\\N"}/
    end

    it "accepts characters and escaped octal digits" do
      Regexp.send(@method, "abc\076").should == /#{"abc\x3e"}/
    end

    it "accepts escaped octal digits and characters" do
      Regexp.send(@method, "\076abc").should == /#{"\x3eabc"}/
    end

    it "accepts characters and escaped hexadecimal digits" do
      Regexp.send(@method, "abc\x42").should == /#{"abc\x42"}/
    end

    it "accepts escaped hexadecimal digits and characters" do
      Regexp.send(@method, "\x3eabc").should == /#{"\x3eabc"}/
    end

    it "accepts escaped hexadecimal and octal digits" do
      Regexp.send(@method, "\061\x42").should == /#{"\x31\x42"}/
    end
  end
end

describe :regexp_new_regexp, :shared => true do
  it "uses the argument as a literal to construct a Regexp object" do
    Regexp.send(@method, /^hi{2,3}fo.o$/).should == /^hi{2,3}fo.o$/
  end

  it "preserves any options given in the Regexp literal" do
    (Regexp.send(@method, /Hi/i).options & Regexp::IGNORECASE).should_not == 0
    (Regexp.send(@method, /Hi/m).options & Regexp::MULTILINE).should_not == 0
    (Regexp.send(@method, /Hi/x).options & Regexp::EXTENDED).should_not == 0

    r = Regexp.send @method, /Hi/imx
    (r.options & Regexp::IGNORECASE).should_not == 0
    (r.options & Regexp::MULTILINE).should_not == 0
    (r.options & Regexp::EXTENDED).should_not == 0

    r = Regexp.send @method, /Hi/
    (r.options & Regexp::IGNORECASE).should == 0
    (r.options & Regexp::MULTILINE).should == 0
    (r.options & Regexp::EXTENDED).should == 0
  end

  it "does not honour options given as additional arguments" do
    r = Regexp.send @method, /hi/, Regexp::IGNORECASE
    (r.options & Regexp::IGNORECASE).should == 0
  end

  it "does not enable multibyte support by default" do
    r = Regexp.send @method, /Hi/
    r.kcode.should_not == 'euc'
    r.kcode.should_not == 'sjis'
    r.kcode.should_not == 'utf8'
  end

  it "enables multibyte support if given in the literal" do
    Regexp.send(@method, /Hi/u).kcode.should == 'utf8'
    Regexp.send(@method, /Hi/e).kcode.should == 'euc'
    Regexp.send(@method, /Hi/s).kcode.should == 'sjis'
    Regexp.send(@method, /Hi/n).kcode.should == 'none'
  end

  describe "with $KCODE set" do

    before :each do
      @kcode = $KCODE
      $KCODE = 'u'
    end

    after :each do
      $KCODE = @kcode
    end

    it "falls back to ASCII for an invalid UTF-8 regexp" do
      (Regexp.send(@method, /^([\x00-\x7F]|[\xC2-\xDF][\x80-\xBF])*$/) =~ "hellø").should == 0
    end

  end
end
