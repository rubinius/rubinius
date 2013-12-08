require File.expand_path('../../../spec_helper', __FILE__)

describe "Array#sample" do
  it "passes the size of the Array to the object passed with :random" do
    obj = mock("array_sample_random")
    obj.should_receive(:rand).with(3).and_return(0)

    [1, 2, 3].sample(:random => obj).should be_an_instance_of(Fixnum)
  end
end
