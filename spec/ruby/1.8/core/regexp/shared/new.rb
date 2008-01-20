shared :regexp_new do |cmd|
  describe "Regexp.#{cmd}" do
    it "requires one argument and creates a new regular expression object" do
      Regexp.send(cmd, '').is_a?(Regexp).should == true
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

      RegexpSpecsSubclass.send(cmd, "hi").class.should == RegexpSpecsSubclass
      RegexpSpecsSubclass.send(cmd, "hi").args.first.should == "hi"

      RegexpSpecsSubclassTwo.send(cmd, "hi").class.should == RegexpSpecsSubclassTwo
    end
  end

  describe "Regexp.#{cmd} given a String" do
    it "uses the String argument as an unescaped literal to construct a Regexp object" do
      Regexp.send(cmd, "^hi{2,3}fo.o$").should == /^hi{2,3}fo.o$/
    end

    it "should throw regexp error with incorrect regexp" do
      lambda { Regexp.send(cmd, "^[$", 0) }.should raise_error(RegexpError)
    end

    it "does not set Regexp options if only given one argument" do
      r = Regexp.send(cmd, 'Hi')
      (r.options & Regexp::IGNORECASE).should     == 0
      (r.options & Regexp::MULTILINE).should      == 0
      (r.options & Regexp::EXTENDED).should       == 0
    end

    it "does not set Regexp options if second argument is nil or false" do
      r = Regexp.send(cmd, 'Hi', nil)
      (r.options & Regexp::IGNORECASE).should     == 0
      (r.options & Regexp::MULTILINE).should      == 0
      (r.options & Regexp::EXTENDED).should       == 0

      r = Regexp.send(cmd, 'Hi', false)
      (r.options & Regexp::IGNORECASE).should     == 0
      (r.options & Regexp::MULTILINE).should      == 0
      (r.options & Regexp::EXTENDED).should       == 0
    end

    it "sets options from second argument if it is one of the Fixnum option constants" do
      r = Regexp.send(cmd, 'Hi', Regexp::IGNORECASE)
      (r.options & Regexp::IGNORECASE).should_not == 0
      (r.options & Regexp::MULTILINE).should      == 0
      (r.options & Regexp::EXTENDED).should       == 0

      r = Regexp.send(cmd, 'Hi', Regexp::MULTILINE)
      (r.options & Regexp::IGNORECASE).should     == 0
      (r.options & Regexp::MULTILINE).should_not  == 0
      (r.options & Regexp::EXTENDED).should       == 0

      r = Regexp.send(cmd, 'Hi', Regexp::EXTENDED)
      (r.options & Regexp::IGNORECASE).should     == 0
      (r.options & Regexp::MULTILINE).should      == 0
      (r.options & Regexp::EXTENDED).should_not   == 1
    end

    it "accepts a Fixnum of two or more options ORed together as the second argument" do
      r = Regexp.send(cmd, 'Hi', Regexp::IGNORECASE | Regexp::EXTENDED)
      (r.options & Regexp::IGNORECASE).should_not == 0
      (r.options & Regexp::MULTILINE).should      == 0
      (r.options & Regexp::EXTENDED).should_not   == 0
    end

    it "treats any non-Fixnum, non-nil, non-false second argument as IGNORECASE" do
      r = Regexp.send(cmd, 'Hi', Object.new)
      (r.options & Regexp::IGNORECASE).should_not == 0
      (r.options & Regexp::MULTILINE).should      == 0
      (r.options & Regexp::EXTENDED).should       == 0
    end

    it "does not enable multibyte support by default" do
      r = Regexp.send cmd, 'Hi', true
      r.kcode.should_not == 'euc'
      r.kcode.should_not == 'sjis'
      r.kcode.should_not == 'utf8'
    end

    it "enables EUC encoding if third argument is 'e' or 'euc' (case-insensitive)" do
      Regexp.send(cmd, 'Hi', nil, 'e').kcode.should     == 'euc'
      Regexp.send(cmd, 'Hi', nil, 'E').kcode.should     == 'euc'
      Regexp.send(cmd, 'Hi', nil, 'euc').kcode.should   == 'euc'
      Regexp.send(cmd, 'Hi', nil, 'EUC').kcode.should   == 'euc'
      Regexp.send(cmd, 'Hi', nil, 'EuC').kcode.should   == 'euc'
    end

    it "enables SJIS encoding if third argument is 's' or 'sjis' (case-insensitive)" do
      Regexp.send(cmd, 'Hi', nil, 's').kcode.should     == 'sjis'
      Regexp.send(cmd, 'Hi', nil, 'S').kcode.should     == 'sjis'
      Regexp.send(cmd, 'Hi', nil, 'sjis').kcode.should  == 'sjis'
      Regexp.send(cmd, 'Hi', nil, 'SJIS').kcode.should  == 'sjis'
      Regexp.send(cmd, 'Hi', nil, 'sJiS').kcode.should  == 'sjis'
    end

    it "enables UTF-8 encoding if third argument is 'u' or 'utf8' (case-insensitive)" do
      Regexp.send(cmd, 'Hi', nil, 'u').kcode.should     == 'utf8'
      Regexp.send(cmd, 'Hi', nil, 'U').kcode.should     == 'utf8'
      Regexp.send(cmd, 'Hi', nil, 'utf8').kcode.should  == 'utf8'
      Regexp.send(cmd, 'Hi', nil, 'UTF8').kcode.should  == 'utf8'
      Regexp.send(cmd, 'Hi', nil, 'uTf8').kcode.should  == 'utf8'
    end

    it "disables multibyte support if third argument is 'n' or 'none' (case insensitive)" do
      Regexp.send(cmd, 'Hi', nil, 'N').kcode.should == 'none'
      Regexp.send(cmd, 'Hi', nil, 'n').kcode.should == 'none'
      Regexp.send(cmd, 'Hi', nil, 'nONE').kcode.should == 'none'
    end
  end

  describe "Regexp.#{cmd} given a Regexp" do
    it "uses the argument as a literal to construct a Regexp object" do
      Regexp.send(cmd, /^hi{2,3}fo.o$/).should == /^hi{2,3}fo.o$/
    end

    it "preserves any options given in the Regexp literal" do
      (Regexp.send(cmd, /Hi/i).options & Regexp::IGNORECASE).should_not == 0
      (Regexp.send(cmd, /Hi/m).options & Regexp::MULTILINE).should_not == 0
      (Regexp.send(cmd, /Hi/x).options & Regexp::EXTENDED).should_not == 0

      r = Regexp.send cmd, /Hi/imx
      (r.options & Regexp::IGNORECASE).should_not == 0
      (r.options & Regexp::MULTILINE).should_not == 0
      (r.options & Regexp::EXTENDED).should_not == 0

      r = Regexp.send cmd, /Hi/
      (r.options & Regexp::IGNORECASE).should == 0
      (r.options & Regexp::MULTILINE).should == 0
      (r.options & Regexp::EXTENDED).should == 0
    end

    it "does not honour options given as additional arguments" do
      r = Regexp.send cmd, /hi/, Regexp::IGNORECASE
      (r.options & Regexp::IGNORECASE).should == 0
    end

    it "does not enable multibyte support by default" do
      r = Regexp.send cmd, /Hi/
      r.kcode.should_not == 'euc'
      r.kcode.should_not == 'sjis'
      r.kcode.should_not == 'utf8'
    end

    it "enables multibyte support if given in the literal" do
      Regexp.send(cmd, /Hi/u).kcode.should == 'utf8'
      Regexp.send(cmd, /Hi/e).kcode.should == 'euc'
      Regexp.send(cmd, /Hi/s).kcode.should == 'sjis'
      Regexp.send(cmd, /Hi/n).kcode.should == 'none'
    end
  end
end
