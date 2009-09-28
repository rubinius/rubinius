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

    RegexpSpecsSubclass.send(@method, "hi").class.should == RegexpSpecsSubclass
    RegexpSpecsSubclass.send(@method, "hi").args.first.should == "hi"

    RegexpSpecsSubclassTwo.send(@method, "hi").class.should == RegexpSpecsSubclassTwo
  end
end

describe :regexp_new_string, :shared => true do
  it "uses the String argument as an unescaped literal to construct a Regexp object" do
    Regexp.send(@method, "^hi{2,3}fo.o$").should == /^hi{2,3}fo.o$/
  end

  it "should throw regexp error with incorrect regexp" do
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

  ruby_version_is ""..."1.9" do
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

  ruby_version_is ""..."1.9" do
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
  end

  ruby_version_is "1.9" do
    it "enables multibyte support if given in the literal" do
      Regexp.send(@method, /Hi/u).encoding.inspect.should == "#<Encoding:UTF-8>"
      Regexp.send(@method, /Hi/e).encoding.inspect.should == "#<Encoding:EUC-JP>"
      Regexp.send(@method, /Hi/s).encoding.inspect.should == "#<Encoding:Windows-31J>"
      Regexp.send(@method, /Hi/n).encoding.inspect.should == "#<Encoding:US-ASCII>"
    end
  end
end
