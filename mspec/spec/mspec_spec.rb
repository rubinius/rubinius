require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/../mspec'

describe MSpec, ".protect" do
  it "rescues any exceptions raised when executing the block argument" do
    lambda {
      MSpec.protect("") { raise Exception, "Now you see me..." }
    }.should_not raise_error
  end
end
