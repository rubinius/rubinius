require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/eql', __FILE__)

# Do not use #should_receive(:eql?) mocks in these specs
# because MSpec uses Hash for mocks and Hash calls #eql?.

describe "Array#eql?" do
  it_behaves_like :array_eql, :eql?

  it "returns false if any corresponding elements are not #eql?" do
    [1, 2, 3, 4].send(@method, [1, 2, 3, 4.0]).should be_false
  end

  it "returns false if other is not a kind of Array" do
    obj = mock("array eql?")
    obj.should_not_receive(:to_ary)
    obj.should_not_receive(:==)

    [1, 2, 3].eql?(obj).should be_false
  end
end
