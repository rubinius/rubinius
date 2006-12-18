require File.dirname(__FILE__) + '/../spec_helper'

context "Fixnum" do
  
  specify "'%' (modulo) should return self modulo other" do
    rubinius(<<-CODE
        puts [451 % 2, 93 % 3].inspect
      CODE
    ).should == '[1, 0]'
  end
end
