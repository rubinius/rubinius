require File.dirname(__FILE__) + '/../../spec_helper'

context "Execution literal" do
  specify "`` should return the result of the executed sub-process" do
    example do
      @ip = 'world'
      `echo disc #{@ip}`
    end.should == "disc world\n" # it changes from hello world
  end

  # NOTE: Interpolation ? It's not consistant with %w for example.
  specify "%x() is the same (with also interpolation)" do
    example do
      @ip = 'world'
      %x(echo disc #{@ip})
    end.should == "disc world\n"
  end

  # NOTE: %X doesn't exist.
end
