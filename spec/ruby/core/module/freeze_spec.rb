require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)

describe "Module#freeze" do
  it "prevents further modifications to self" do
    m = Module.new.freeze
    m.frozen?.should == true

    # Does not raise
    class << m; end

    lambda {
      class << m
        def test() "test" end
      end
    }.should raise_error(frozen_object_error_class)

    lambda { def m.test() "test" end }.should raise_error(frozen_object_error_class)
  end
end
