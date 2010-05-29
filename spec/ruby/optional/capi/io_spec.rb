require File.expand_path('../spec_helper', __FILE__)

load_extension('io')

describe "C-API IO function" do
  before :each do
    @o = CApiIOSpecs.new
  end

  describe "rb_io_write" do
    it "sends #write to the object passing the object to be written" do
      io = mock("IO")
      io.should_receive(:write).with("test").and_return(:written)
      @o.rb_io_write(io, "test").should == :written
    end
  end
end
