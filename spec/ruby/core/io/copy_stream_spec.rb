require File.expand_path('../../../spec_helper', __FILE__)

ruby_version_is "1.9" do
  describe "IO.copy_stream" do
    it "needs to be reviewed for spec completeness"
  end

  describe "IO.copy_stream" do
    before :each do
      # TODO: multi-line content (esp. on Windows!)
      @content = "one-two-three"

      @filename_src = tmp("source")
      touch(@filename_src) { |f| f.write(@content) }
      @io_src = new_io(@filename_src, "rb")

      @filename_dest = tmp("destination")
      # TODO: new_io doesn't work here due to bug in mspsec
      # @io_dest = new_io(@filename_dest, "w")
      @io_dest = File.open(@filename_dest, "wb")
    end

    after :each do
      @io_src.close unless @io_src.closed?
      @io_dest.close unless @io_dest.closed?
      File.unlink @filename_src
      File.unlink @filename_dest
    end

    it "copies from an IO instance to a named file" do
      IO.copy_stream(@io_src, @filename_dest).should == @content.size
      File.read(@filename_dest).should == @content
    end

    it "copies from a named file to a named file" do
      IO.copy_stream(@filename_src, @filename_dest).should == @content.size
      File.read(@filename_dest).should == @content
    end

    it "copies from a named file to an IO instance" do
      IO.copy_stream(@filename_src, @io_dest).should == @content.size
      File.read(@filename_dest).should == @content
    end

    it "copies from an IO instance to an IO instance" do
      IO.copy_stream(@io_src, @io_dest).should == @content.size
      File.read(@filename_dest).should == @content
    end
  end
end
