require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/each'

describe "StringIO#fileno" do
  it "returns nil" do
    StringIO.new("nuffin").fileno.should be_nil
  end
end
