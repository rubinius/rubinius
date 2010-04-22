require File.dirname(__FILE__) + '/spec_helper'

load_extension("time")

describe "CApiTimeSpecs" do
  before :each do
    @s = CApiTimeSpecs.new
  end

  describe "rb_time_new" do
    it "creates a Time from the sec and usec" do
      @s.rb_time_new(1232141421, 1413123123).should == Time.at(1232141421, 1413123123)
    end
  end
end