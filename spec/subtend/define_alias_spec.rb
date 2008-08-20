require File.dirname(__FILE__) + '/../spec_helper'
require File.dirname(__FILE__) + '/subtend_helper'

compile_extension('subtend_define_alias')
require File.dirname(__FILE__) + '/ext/subtend_define_alias'

class DefineAliasTest
	def old_name
		return true;
	end
end

describe "SubtendDefineAlias" do
  before :each do
    @s = SubtendDefineAlias.new;
  end

	it "should allow method aliases" do
		(DefineAliasTest.method_defined? :old_name).should == true;
		(DefineAliasTest.method_defined? :new_name).should_not == true;
		@s.rb_define_alias(DefineAliasTest, "new_name", "old_name");
		(DefineAliasTest.method_defined? :new_name).should == true;
		
		t = DefineAliasTest.new;
		t.new_name.should == true;
  end
end
