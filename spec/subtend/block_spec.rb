require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_block')
require File.dirname(__FILE__) + '/ext/subtend_block'

describe "SubtendBlock" do
  before :each do
    @f = SubtendBlock.new
  end
  
  it "identifies blocks using rb_block_given_p" do
    @f.block_given?.should == false
    (@f.block_given? { puts "FOO" } ).should == true
  end

end
