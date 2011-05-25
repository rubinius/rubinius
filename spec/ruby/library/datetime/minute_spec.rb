require File.expand_path('../../../spec_helper', __FILE__)
require 'mspec/helpers/datetime'
require 'date'

describe "DateTime#min" do
  ruby_version_is "1.9" do
    it "returns 0 if no argument is passed" do
      DateTime.new.minute.should == 0
    end

    it "returns the minute passed as argument" do
      new_datetime(:minute => 5).minute.should == 5
    end

    it "adds 60 to negative minutes" do
      new_datetime(:minute => -20).minute.should == 40
    end

    it "returns the absolute value of a Rational" do
      new_datetime(:minute => 5 + Rational(1,2)).minute.should == 5
    end

    it "returns the absolute value of a Float" do
      new_datetime(:minute => 5.5).minute.should == 5
    end

    it "returns a fraction of an hour" do
      new_datetime(:hour => 2 + Rational(1,2)).minute.should == 30
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

    it "takes a minute fraction near 60" do
      new_datetime(:minute => 59 + Rational(1,2)).minute.should == 59
    end
  end
end
