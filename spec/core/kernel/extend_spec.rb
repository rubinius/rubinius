require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "Object#extend" do
  it "raises a TypeError if self is frozen" do
    module Mod; end
    o = Object.new
    o.freeze
    should_raise(TypeError) { o.extend Mod }
  end
end