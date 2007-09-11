require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes.rb'

describe "String#to_i with base 10" do
  it "interprets leading characters as a number in the given base" do
    "100110010010".to_i(2).should == 0b100110010010
    "100110201001".to_i(3).should == 186409
    "103110201001".to_i(4).should == 5064769
    "103110241001".to_i(5).should == 55165126
    "153110241001".to_i(6).should == 697341529
    "153160241001".to_i(7).should == 3521513430
    "153160241701".to_i(8).should == 14390739905
    "853160241701".to_i(9).should == 269716550518
    "853160241791".to_i(10).should == 853160241791
    
    "F00D_BE_1337".to_i(16).should == 0xF00D_BE_1337
    "-hello_world".to_i(32).should == -18306744
    "abcXYZ".to_i(36).should == 623741435
    
    ("z" * 24).to_i(36).should == 22452257707354557240087211123792674815

    "5e10".to_i.should == 5
    
    "0b-1".to_i(2).should == (2 ** 32) - 1
    "0d-1".to_i(10).should == (2 ** 32) - 1
    "0o-1".to_i(8).should == (2 ** 32) - 1
    "0x-1".to_i(16).should == (2 ** 32) - 1
  end
  
  noncompliant :rubinius do
    it "auto-detects base via base specifiers (default: 10) for base = 0" do
      "01778".to_i(0).should == 0177
      "0b112".to_i(0).should == 0b11
      "0d19A".to_i(0).should == 19
      "0o178".to_i(0).should == 0o17
      "0xFAZ".to_i(0).should == 0xFA
      "1234567890ABC".to_i(0).should == 1234567890

      "-01778".to_i(0).should == -0177
      "-0b112".to_i(0).should == -0b11
      "-0d19A".to_i(0).should == -19
      "-0o178".to_i(0).should == -0o17
      "-0xFAZ".to_i(0).should == -0xFA
      "-1234567890ABC".to_i(0).should == -1234567890
    
      "0b-1".to_i(0).should == 0
      "0d-1".to_i(0).should == 0
      "0o-1".to_i(0).should == 0
      "0x-1".to_i(0).should == 0
    end
  end
  
  it "doesn't handle foreign base specifiers when base is > 0" do
    [2, 3, 4, 8, 10].each do |base|
      "0111".to_i(base).should == "111".to_i(base)
      
      "0b11".to_i(base).should == (base ==  2 ? 0b11 : 0)
      "0d11".to_i(base).should == (base == 10 ? 0d11 : 0)
      "0o11".to_i(base).should == (base ==  8 ? 0o11 : 0)
      "0xFA".to_i(base).should == 0
    end
    
    "0xD00D".to_i(16).should == 0xD00D
    
    "0b11".to_i(16).should == 0xb11
    "0d11".to_i(16).should == 0xd11
    "0o11".to_i(25).should == 15026
    "0x11".to_i(34).should == 38183
  end
  
  it "tries to convert the base to an integer using to_int" do
    obj = Object.new
    def obj.to_int() 8 end
    
    "777".to_i(obj).should == 0777

    obj = Object.new
    obj.should_receive(:respond_to?, :with => [:to_int], :returning => true)
    obj.should_receive(:method_missing, :with => [:to_int], :returning => 8)

    "777".to_i(obj).should == 0777
  end
  
  it "raises ArgumentError for illegal bases (1, < 0 or > 36)" do
    should_raise(ArgumentError) { "".to_i(1) }
    should_raise(ArgumentError) { "".to_i(-1) }
    should_raise(ArgumentError) { "".to_i(37) }
  end
end
