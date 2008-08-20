require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

compliant_on :ruby, :jruby do
  describe "Object#extend" do
    it "raises a TypeError if self is frozen" do
      module Mod; end
      o = mock('o')
      o.freeze
      lambda { o.extend Mod }.should raise_error(TypeError)
    end
  end
end

describe "Kernel#extend" do
  it "needs to be reviewed for spec completeness"
end
