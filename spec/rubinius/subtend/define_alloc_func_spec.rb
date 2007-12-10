require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

extension :rubinius do
  compile_extension('subtend_define_alloc_func')
  require File.dirname(__FILE__) + '/ext/subtend_define_alloc_func'

  context "SubtendAlloc (a class with an alloc func defined)" do
    specify "calls the alloc func" do
      @s = SubtendAlloc.new
      @s.wrapped_data.should == 42 # not defined in initialize
    end
  end
end
