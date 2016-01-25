require File.expand_path('../../../spec_helper', __FILE__)

describe "Hash#to_proc" do
  before(:all) do
    @hash = new_hash(foo: :bar)
  end

  it "returns a new Proc" do
    proc = @hash.to_proc
    proc.should be_kind_of(Proc)
  end

  it "sends :[] with argument passed to the Hash when calling #call on the Proc" do
    @hash.to_proc.call(:foo).should == :bar
  end

  it "raises an ArgumentError when calling #call on the Proc without receiver" do
    lambda { @hash.to_proc.call }.should raise_error(ArgumentError)
  end
end
