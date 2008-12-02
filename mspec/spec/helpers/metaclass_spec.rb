require File.dirname(__FILE__) + '/../spec_helper'
require 'mspec/helpers/metaclass'

describe Object, "#metaclass" do
  it "returns the singleton class for an object" do
    obj = Object.new
    def obj.metaclass_test; end
    meta = obj.metaclass

    meta.instance_methods(false).should include("metaclass_test")
    obj.should_not be_instance_of(meta)
    obj.should be_kind_of(meta)
  end
end
