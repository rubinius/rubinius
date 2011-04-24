require File.expand_path('../../../spec_helper', __FILE__)

describe "Symbol#===" do
  it "returns true when the other is a Symbol" do
    (Symbol === :ruby).should == true
    (Symbol === :"ruby").should == true
    (Symbol === :'ruby').should == true
    (Symbol === 'ruby').should == false
  end
end

# Symbol === :fnord
