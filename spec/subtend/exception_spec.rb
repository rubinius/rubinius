require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/exception/subtend_exception'

context "SubtendRaiser" do
  setup do
    @s = SubtendRaiser.new
  end
  
  specify "raise! should raise a TypeError" do
    should_raise(TypeError) do
      @s.raise!
    end
  end
end
