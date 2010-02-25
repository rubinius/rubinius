describe "Regexps with capture group modifers" do
  describe '/.../ (named group is not used, no option)' do
    it 'treats (...) as a captured group' do
      /foo(bar)/.match('foobar').captures.should == ['bar']
    end
  end

  describe '/.../g (named group is not used, "g" option)' do
    it 'treats (...) as a non-captured group (?...)' do
      /foo(bar)/g.match('foobar').captures.should == []
    end
  end

  describe '/..(?<name>..)../ (named group is used, no option)' do
    it 'treats (...) as a non-captured group (?:...)' do
      /(foo)(?<name>bar)(duh)/.match('foobarduh').captures.should == ['bar']
    end

    it "doesn't allow numbered-backref/call" do
      lambda{ /(foo)(?<name>bar)\1/ }.should raise_error(RegexpError)
    end
  end

  describe '/..(?<name>..)../G (named group is used, "G" option)' do
    it 'treats (...) as a captured group' do
      /(foo)(?<name>bar)(duh)/G.match('foobarduh').captures.should == ['foo', 'bar', 'duh']
    end

    it 'allows numbered-backref/call' do
      /(foo)(?<name>bar)(\1)/G.match('foobarfoo').captures.should == ['foo', 'bar', 'foo']
    end
  end

  it 'instantiates a Regexp with a flag equivalent to /g' do
    Regexp.new('', Regexp::DONT_CAPTURE_GROUP).options.should == //g.options
  end

  it 'instantiates a Regexp with a flag equivalent to /G' do
    Regexp.new('', Regexp::CAPTURE_GROUP).options.should == //G.options
  end
end
