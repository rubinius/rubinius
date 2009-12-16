require File.dirname(__FILE__) + '/../../spec_helper'
require File.dirname(__FILE__) + '/fixtures/classes'

describe "IO#initialize" do
  before :each do
    @io = IO.allocate
  end

  it "succeeds when fd is a Fixnum" do
    lambda { @io.send :initialize, $stdout.fileno, 'w' }.should_not raise_error(TypeError)
  end

  it "succeeds when fd responds to #to_int" do
    obj = mock('fileno')
    def obj.to_int() $stdout.fileno end

    lambda { @io.send :initialize, obj, 'w' }.should_not raise_error(TypeError)
  end

  it "raises a TypeError when fd is an IO" do
    lambda { @io.send :initialize, $stdout, 'w' }.should raise_error(TypeError)
  end

  it "raises a TypeError when given a non-integer" do
    lambda { @io.send :initialize, @fname, 'w' }.should raise_error(TypeError)
  end
  
  it "raises IOError on closed stream" do
    lambda { @io.send :initialize, IOSpecs.closed_file.fileno }.should raise_error(IOError)
  end

  it "raises an Errno::EBADF when given an invalid file descriptor" do
    lambda { @io.send :initialize, -1, 'w' }.should raise_error(Errno::EBADF)
  end
  
  ruby_version_is '1.9' do
    it "uses the external encoding specified in the mode argument" do
      io = IO.new(2, 'w:UTF-8')
      io.external_encoding.to_s.should == 'UTF-8'
    end

    it "uses the external and the internal encoding specified in the mode argument" do
      io = IO.new(2, 'w:UTF-8:iso-8859-1')
      io.external_encoding.to_s.should == 'UTF-8'
      io.internal_encoding.to_s.should == 'ISO-8859-1'
    end

    it "uses the external encoding specified via the :external_encoding option" do
      io = IO.new(2, 'w', {:external_encoding => 'UTF-8'})
      io.external_encoding.to_s.should == 'UTF-8'
    end

    it "uses the internal encoding specified via the :internal_encoding option" do
      io = IO.new(2, 'w', {:internal_encoding => 'ISO-8859-1'})
      io.internal_encoding.to_s.should == 'ISO-8859-1'
    end

    it "uses the colon-separated encodings specified via the :encoding option" do
      io = IO.new(2, 'w', {:encoding => 'UTF-8:iso-8859-1'})
      io.external_encoding.to_s.should == 'UTF-8'
      io.internal_encoding.to_s.should == 'ISO-8859-1'
    end

    it "ingores the :encoding option when the :external_encoding option is present" do
      io = IO.new(2, 'w', {:external_encoding => 'UTF-8', :encoding => 'iso-8859-1:iso-8859-1'})
      io.external_encoding.to_s.should == 'UTF-8'
    end
    
    it "ingores the :encoding option when the :internal_encoding option is present" do
      io = IO.new(2, 'w', {:internal_encoding => 'ISO-8859-1', :encoding => 'iso-8859-1:iso-8859-1'})
      io.internal_encoding.to_s.should == 'ISO-8859-1'
    end

    it "uses the encoding specified via the :mode option hash" do
      io = IO.new(2, {:mode => 'w:UTF-8:iso-8859-1'})
      io.external_encoding.to_s.should == 'UTF-8'
      io.internal_encoding.to_s.should == 'ISO-8859-1'
    end
    
    it "ignores the :internal_encoding option when the same as the external encoding" do
      io = IO.new(2, 'w', {:internal_encoding => 'UTF-8'})
      io.external_encoding.to_s.should == 'UTF-8'
      io.internal_encoding.to_s.should == ''
    end
  end
end
