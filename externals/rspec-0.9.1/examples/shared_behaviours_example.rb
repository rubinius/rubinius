require File.dirname(__FILE__) + '/spec_helper'

module SharedBehaviourExample
  class OneThing
    def what_things_do
      "stuff"
    end
  end
  
  class AnotherThing
    def what_things_do
      "stuff"
    end
  end
  
  describe "All Things", :shared => true do
    it "should do what things do" do
      @thing.what_things_do.should == "stuff"
    end
  end

  describe OneThing do
    it_should_behave_like "All Things"
    before(:each) { @thing = OneThing.new }
  end

  describe AnotherThing do
    it_should_behave_like "All Things"
    before(:each) { @thing = AnotherThing.new }
  end
end
