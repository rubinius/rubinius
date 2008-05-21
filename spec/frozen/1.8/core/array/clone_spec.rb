require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/clone'

describe "Array#clone" do
  it_behaves_like :array_clone, :clone

  not_compliant_on :rubinius do
    it "copies frozen status from the original" do
      a = [1, 2, 3, 4]
      b = [1, 2, 3, 4]
      a.freeze
      aa = a.clone
      bb = b.clone

      aa.frozen?.should == true
      bb.frozen?.should == false
    end
  end
end
