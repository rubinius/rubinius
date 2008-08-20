require File.dirname(__FILE__) + '/../spec_helper'

describe "Execution literal" do
  it "`` should return the result of the executed sub-process" do
    ip = 'world'
    `echo disc #{ip}`.should == "disc world\n"
  end

  # NOTE: Interpolation ? It's not consistant with %w for example.
  it "%x() is the same (with also interpolation)" do
    ip = 'world'
    %x(echo disc #{ip}).should == "disc world\n"
  end

  # NOTE: %X doesn't exist.
end
