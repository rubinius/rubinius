shared :regexp_new do |cmd|
  describe "Regexp.#{cmd}" do
    it "requires one argument and creates a new regular expression object" do
      Regexp.send(cmd, '').is_a?(Regexp).should == true
    end

    it "returns a subclass instance when called on a subclass" do
      class RegexpSpecsSubclass < Regexp; end

      RegexpSpecsSubclass.send(cmd, "hi").class.should == RegexpSpecsSubclass
    end
  end

  describe "Regexp.#{cmd} given a String" do
    it "uses the String argument as an unescaped literal to construct a Regexp object" do
      Regexp.send(cmd, "^hi{2,3}fo.o$").should == /^hi{2,3}fo.o$/
    end

    it "does not set Regexp options if only given one argument or second argument is nil" do
      r = Regexp.send(cmd, 'Hi')
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

      r = Regexp.send(cmd, 'Hi', false)
      (r.options & Regexp::IGNORECASE).should     == 0
      (r.options & Regexp::MULTILINE).should      == 0
      (r.options & Regexp::EXTENDED).should       == 0
    end

    it "does not enable multibyte support by default" do
      r = Regexp.send cmd, 'Hi', true
      r.kcode.should_not == 'euc'
      r.kcode.should_not == 'sjis'
      r.kcode.should_not == 'utf8'
    end

    it "enables multibyte support if third argument is '[eEsSuU]' (EUC, SJIS, UTF-8)" do
      Regexp.send(cmd, 'Hi', nil, 'e').kcode.should == 'euc'
      Regexp.send(cmd, 'Hi', nil, 'S').kcode.should == 'sjis'
      Regexp.send(cmd, 'Hi', nil, 'u').kcode.should == 'utf8'
    end

    it "explicitly disables multibyte support if third argument is '[nN]' for None" do
      Regexp.send(cmd, 'Hi', nil, 'N').kcode.should == 'none'
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
