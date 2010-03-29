require File.expand_path('../../../spec_helper', __FILE__)

describe "Regexp.union" do
  it "returns /(?!)/ when passed no arguments" do
    Regexp.union.should == /(?!)/
  end

  it "returns a regular expression that will match passed arguments" do
    Regexp.union("penzance").should == /penzance/
    Regexp.union("skiing", "sledding").should == /skiing|sledding/
    Regexp.union(/dogs/, /cats/i).should == /(?-mix:dogs)|(?i-mx:cats)/
  end

  it "quotes any string arguments" do
    Regexp.union("n", ".").should == /n|\./
  end

  ruby_version_is '' ... '1.9' do
    it "propagates the kcode setting to the output Regexp" do
      Regexp.union(/./u, "meow").kcode.should == "utf8"
    end
  end

  it "raises ArgumentError if the kcodes conflict" do
    lambda {
      Regexp.union(/./u, /./e)
    }.should raise_error(ArgumentError)
  end

  it "uses to_str to convert arguments (if not Regexp)" do
    obj = mock('pattern')
    obj.should_receive(:to_str).and_return('foo')
    Regexp.union(obj, "bar").should == /foo|bar/
  end

  ruby_version_is '1.8.7' do
    it "accepts a single array of patterns as arguments" do
      Regexp.union(["skiing", "sledding"]).should == /skiing|sledding/
      Regexp.union([/dogs/, /cats/i]).should == /(?-mix:dogs)|(?i-mx:cats)/
      lambda{Regexp.union(["skiing", "sledding"], [/dogs/, /cats/i])}.should raise_error(TypeError)
    end
  end
end
