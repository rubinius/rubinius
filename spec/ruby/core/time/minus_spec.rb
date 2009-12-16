require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

# MRI 1.9 broke this test, so we use a combination of version guards and bug
# guards to indicate that it is expected to pass on versions < 1.9, and,
# currently, expected to fail on 1.9. Once this bug is fixed and backported,
# these guards can be removed, along with the duplicate describe block.

ruby_version_is ""..."1.9" do
  describe "Time#-" do
    it "decrements the time by the specified amount" do
      (Time.at(100) - 100).should == Time.at(0)
      (Time.at(100) - Time.at(99)).should == 1.0
      (Time.at(1.1) - 0.2).should == Time.at(0.9)
    end

    it "accepts arguments that can be coerced into Float" do
      (obj = mock('9.5')).should_receive(:to_f).and_return(9.5)
      (Time.at(100) - obj).should == Time.at(90.5)    
    end

    it "raises TypeError on argument that can't be coerced into Float" do
      lambda { Time.now - Object.new }.should raise_error(TypeError)
      lambda { Time.now - "stuff" }.should raise_error(TypeError)
    end

    it "raises TypeError on nil argument" do
      lambda { Time.now - nil }.should raise_error(TypeError)
    end

    it "tracks microseconds" do
      time = Time.at(0.777777)
      time -= 0.654321
      time.usec.should == 123456
      time -= 0.123456
      time.usec.should == 0
    end
  end
end

ruby_version_is "1.9" do
  ruby_bug "[ruby-dev:38446]", "1.9.2" do
    describe "Time#-" do
      it "decrements the time by the specified amount" do
        (Time.at(100) - 100).should == Time.at(0)
        (Time.at(100) - Time.at(99)).should == 1.0
        (Time.at(1.1) - 0.2).should == Time.at(0.9)
      end

      it "accepts arguments that can be coerced into Float" do
        (obj = mock('9.5')).should_receive(:to_f).and_return(9.5)
        (Time.at(100) - obj).should == Time.at(90.5)    
      end

      it "raises TypeError on argument that can't be coerced into Float" do
        lambda { Time.now - Object.new }.should raise_error(TypeError)
        lambda { Time.now - "stuff" }.should raise_error(TypeError)
      end

      it "raises TypeError on nil argument" do
        lambda { Time.now - nil }.should raise_error(TypeError)
      end

      it "tracks microseconds" do
        time = Time.at(0.777777)
        time -= 0.654321
        time.usec.should == 123456
        time -= 0.123456
        time.usec.should == 0
      end
    end
  end
end  
