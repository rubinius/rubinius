require File.expand_path('../../../spec_helper', __FILE__)
require 'rational'

describe :rational_new, :shared => true do
  it "is private" do
    Rational.should have_private_method(:new)
  end
end
