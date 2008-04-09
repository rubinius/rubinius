require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Singleton._load" do

  it "is a private method" do
    lambda { SingletonSpecs::MyClass.instance._load("") }.should raise_error(NoMethodError)
  end
  
  it "returns the singleton instance for anything passed in" do
    SingletonSpecs::MyClass.send(:_load, "").equal?(SingletonSpecs::MyClass.instance).should == true
    SingletonSpecs::MyClass.send(:_load, "42").equal?(SingletonSpecs::MyClass.instance).should == true
    SingletonSpecs::MyClass.send(:_load, 42).equal?(SingletonSpecs::MyClass.instance).should == true
  end

  it "returns the singleton instance for anything passed in for a singleton subclass" do
    SingletonSpecs::MyClassChild.send(:_load, "").equal?(SingletonSpecs::MyClassChild.instance).should == true
    SingletonSpecs::MyClassChild.send(:_load, "42").equal?(SingletonSpecs::MyClassChild.instance).should == true
    SingletonSpecs::MyClassChild.send(:_load, 42).equal?(SingletonSpecs::MyClassChild.instance).should == true
  end

  it "returns the singleton instance for anything passed in for a singleton clone" do
    klone = SingletonSpecs::MyClass.clone
    klone.send(:_load, "").equal?(klone.instance).should == true
    klone.send(:_load, "42").equal?(klone.instance).should == true
    klone.send(:_load, 42).equal?(klone.instance).should == true
  end

end

describe "Marshal.load on singleton object previously serialised in this process" do
  ruby_bug do
    # http://rubyforge.org/tracker/index.php?func=detail&aid=19377&group_id=426&atid=1698
    it "returns the singleton instance" do
      instance = SingletonSpecs::MyClass.instance
      instance.equal?(Marshal.load(Marshal.dump(instance))).should == true
    end
  end
end
