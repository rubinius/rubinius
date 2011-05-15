require File.expand_path('../../../spec_helper', __FILE__)
require 'mspec/helpers/datetime'
require 'date'

describe "DateTime#min" do
  it "returns 0 if no argument is passed" do
    DateTime.new.min.should == 0
  end

  it "returns the minute passed as argument" do
    new_datetime(:minute => 5).min.should == 5
  end

  it "adds 60 to negative minutes" do
    new_datetime(:minute => -20).min.should == 40
  end

  it "returns the absolute value of a Rational" do
    new_datetime(:minute => 5 + Rational(1,2)).min.should == 5
  end

  ruby_version_is "" .. "1.9" do
    it "raises an error for Float" do
      lambda { new_datetime :minute => 5.5 }.should raise_error(NoMethodError)
    end
  end

  ruby_version_is "1.9" do
    it "returns the absolute value of a Float" do
      new_datetime(:minute => 5.5).min.should == 5
    end
  end

  it "returns a fraction of an hour" do
    new_datetime(:hour => 2 + Rational(1,2)).min.should == 30
  end

  it "raises an error, when the minute is smaller than -60" do
    lambda { new_datetime(:minute => -61) }.should raise_error(ArgumentError)
  end

  it "raises an error, when the minute is greater or equal than 60" do
    lambda { new_datetime(:minute => 60) }.should raise_error(ArgumentError)
  end

  it "raises an error for minute fractions smaller than -60" do
    lambda { new_datetime(:minute => -60 - Rational(1,2))}.should(
      raise_error(ArgumentError))
  end

  ruby_version_is "1.8.7" do
    it "takes a minute fraction near 60" do
      new_datetime(:minute => 59 + Rational(1,2)).min.should == 59
    end
  end
end
