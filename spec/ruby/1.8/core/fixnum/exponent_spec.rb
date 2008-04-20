require File.dirname(__FILE__) + '/../../spec_helper'

describe "Fixnum#**" do
  it "returns self raised to the given power" do
    (2 ** 0).should == 1
    (2 ** 1).should == 2
    (2 ** 2).should == 4

    (9 ** 0.5).to_s.should == '3.0'
    (5 ** -1).to_f.to_s.should == '0.2'

    (2 ** 40).should == 1099511627776
  end

  conflicts_with :Rational do
    it "raises a TypeError when given a non-Integer" do
      lambda {
        (obj = mock('10')).should_receive(:to_int).any_number_of_times.and_return(10)
        13 ** obj
      }.should raise_error(TypeError)
      lambda { 13 ** "10"    }.should raise_error(TypeError)
      lambda { 13 ** :symbol }.should raise_error(TypeError)
    end
  end
end
