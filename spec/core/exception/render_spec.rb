require File.expand_path('../../../spec_helper', __FILE__)

describe "Exception#render" do
  it "renders a multi-line message with the class name included in the first line" do
    io = IOStub.new
    Exception.new("line1\nline2\nline3").render("", io)
    io.should include("line1 (Exception)\n")
  end
end
