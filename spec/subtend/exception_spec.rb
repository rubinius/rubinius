require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_exception')
require File.dirname(__FILE__) + '/ext/subtend_exception'

describe "SubtendRaiser" do
  setup do
    @s = SubtendRaiser.new
  end

  specify "rb_raise should raise an exception" do
    lambda { @s.raise! }.should raise_error(TypeError)
  end

  specify "rb_raise terminates the function early" do
    h = {}
    lambda { @s.raise_early(h) }.should raise_error(TypeError)
    h[:screwed].should == false
  end
end
