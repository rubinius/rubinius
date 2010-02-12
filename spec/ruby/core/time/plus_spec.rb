require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/methods'

describe "Time#+" do
  it "increments the time by the specified amount" do
    (Time.at(0) + 100).should == Time.at(100)
  end

  it "is a commutative operator" do
    (Time.at(1.1) + 0.9).should == Time.at(0.9) + 1.1
  end

  it "understands negative additions" do
    t = Time.at(100) + -1.3
    t.usec.should == 700000
    t.to_i.should == 98
  end

  it "rounds micro seconds rather than truncates" do
    # The use of 8.9999999 is intentional. This is because
    # Time treats the fractional part as the number of micro seconds.
    # Thusly it multiplies the result by 1_000_000 to go from
    # seconds to microseconds. That conversion should be rounded
    # properly. In this case, it's rounded up to 1,000,000, and thus
    # contributes a full extra second to the Time object.
    t = Time.at(0) + 8.9999999
    t.should == Time.at(9)
    t.usec.should == 0

    # Check the non-edge case works properly, that the fractional part
    # contributes to #usecs
    t2 = Time.at(0) + 8.9
    t2.usec.should == 900000
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
