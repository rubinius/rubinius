require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'
require File.dirname(__FILE__) + '/shared/eql'

# Do not use #should_receive(:eql?) mocks in these specs
# because MSpec uses Hash for mocks and Hash calls #eql?.

describe "Array#eql?" do
  it_behaves_like :array_eql, :eql?

  it "returns false if any corresponding elements are not #eql?" do
    [1, 2, 3, 4].send(@method, [1, 2, 3, 4.0]).should be_false
  end

end
