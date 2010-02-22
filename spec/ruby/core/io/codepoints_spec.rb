require File.expand_path('../../../spec_helper', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require File.expand_path('../shared/codepoints', __FILE__)

# See redmine #1667
ruby_version_is "1.9" do
  describe "IO#codepoints" do
    it_behaves_like(:io_codepoints, :codepoints)
  end

  describe "IO#codepoints" do
    before :each do
      @io = IOSpecs.io_fixture "lines.txt"
    end

    after :each do
      @io.close unless @io.closed?
    end

    it "does not yield to a block" do
      @io.codepoints{ flunk }.should be_an_instance_of(enumerator_class)
    end
  end
end
