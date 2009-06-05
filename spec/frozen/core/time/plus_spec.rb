require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

describe "Time#+" do
  it "increments the time by the specified amount" do
    (Time.at(0) + 100).should == Time.at(100)
  end

  it "is a commutative operator" do
    (Time.at(1.1) + 0.9).should == Time.at(0.9) + 1.1
  end

  ruby_version_is "" ... "1.9" do
    it "increments the time by the specified amount as float numbers" do
      (Time.at(1.1) + 0.9).should == Time.at(2)
    end

    it "accepts arguments that can be coerced into Float" do
      (obj = mock('10.5')).should_receive(:to_f).and_return(10.5)
      (Time.at(100) + obj).should == Time.at(110.5)
    end

    it "raises TypeError on argument that can't be coerced into Float" do
      lambda { Time.now + Object.new }.should raise_error(TypeError)
      lambda { Time.now + "stuff" }.should raise_error(TypeError)
    end
  end

  ruby_version_is "1.9.2" do
    it "increments the time by the specified amount as rational numbers" do
      (Time.at(1.1) + 0.9).should_not == Time.at(2)
    end

    it "accepts arguments that can be coerced into Rational" do
      (obj = mock('10')).should_receive(:to_r).and_return(Rational(10))
      (Time.at(100) + obj).should == Time.at(110)
    end

    ruby_bug "#1583", "1.9.2" do
      it "raises TypeError on argument that can't be coerced into Rational" do
        lambda { Time.now + Object.new }.should raise_error(TypeError)
        lambda { Time.now + "stuff" }.should raise_error(TypeError)
      end
    end
  end


  it "raises TypeError on Time argument" do
    lambda { Time.now + Time.now }.should raise_error(TypeError)
  end

  it "raises TypeError on nil argument" do
    lambda { Time.now + nil }.should raise_error(TypeError)
  end

  ruby_bug "[ruby-dev:38446]", "1.9.2" do
    it "tracks microseconds" do
      time = Time.at(0)
      time += 0.123456
      time.usec.should == 123456
      time += 0.654321
      time.usec.should == 777777
    end  
  end
end
