require File.expand_path('../../spec_helper', __FILE__)

describe "The BEGIN keyword" do
  before :each do
    ScratchPad.record []
  end

  it "runs in a new isolated scope" do
    lambda do
      eval "BEGIN { var_in_begin = 'foo' }; var_in_begin"
    end.should raise_error NameError
  end

  it "does not access variables outside the eval scope" do
    lambda do
      outside_var = 'foo'
      eval "BEGIN { outside_var }"
    end.should raise_error NameError
  end
end
