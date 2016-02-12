require File.expand_path('../../../spec_helper', __FILE__)

describe "NameError" do
  it "is a superclass of NoMethodError" do
    NameError.should be_ancestor_of(NoMethodError)
  end
end

describe "NameError.new" do
  it "NameError.new should take optional name argument" do
    NameError.new("msg","name").name.should == "name"
  end
end

describe 'NameError#receiver' do
  it 'returns the receiver if available' do
    x = 'foo'

    begin
      x.foo
    rescue NameError => error
      error.receiver.should == x
    end
  end

  it 'raises ArgumentError when no receiver is available' do
    error = NameError.new('msg', 'name')

    proc { error.receiver }.should raise_error(ArgumentError)
  end
end
