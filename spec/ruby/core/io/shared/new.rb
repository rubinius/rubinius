require File.expand_path('../../fixtures/classes', __FILE__)

# This group of specs may ONLY contain specs that do successfully create
# an IO instance from the file descriptor returned by #new_fd helper.
describe :io_new, :shared => true do
  before :each do
    @name = tmp("io_new.txt")
    @fd = new_fd @name
  end

  after :each do
    @io.close if @io and !@io.closed?
    rm_r @name
  end

  it "creates an IO instance from a Fixnum argument" do
    @io = IO.send(@method, @fd, "w")
    @io.should be_an_instance_of(IO)
  end

  it "calls #to_int on an object to convert to a Fixnum" do
    obj = mock("file descriptor")
    obj.should_receive(:to_int).and_return(@fd)
    @io = IO.send(@method, obj, "w")
    @io.should be_an_instance_of(IO)
  end

  ruby_version_is "1.9" do
    it "uses the external encoding specified in the mode argument" do
      @io = IO.send(@method, @fd, 'w:utf-8')
      @io.external_encoding.to_s.should == 'UTF-8'
    end

    it "uses the external and the internal encoding specified in the mode argument" do
      @io = IO.send(@method, @fd, 'w:utf-8:ISO-8859-1')
      @io.external_encoding.to_s.should == 'UTF-8'
      @io.internal_encoding.to_s.should == 'ISO-8859-1'
    end

    it "uses the external encoding specified via the :external_encoding option" do
      @io = IO.send(@method, @fd, 'w', {:external_encoding => 'utf-8'})
      @io.external_encoding.to_s.should == 'UTF-8'
    end

    it "uses the internal encoding specified via the :internal_encoding option" do
      @io = IO.send(@method, @fd, 'w', {:internal_encoding => 'iso-8859-1'})
      @io.internal_encoding.to_s.should == 'ISO-8859-1'
    end

    it "uses the colon-separated encodings specified via the :encoding option" do
      @io = IO.send(@method, @fd, 'w', {:encoding => 'utf-8:ISO-8859-1'})
      @io.external_encoding.to_s.should == 'UTF-8'
      @io.internal_encoding.to_s.should == 'ISO-8859-1'
    end

    it "ingores the :encoding option when the :external_encoding option is present" do
      @io = IO.send(@method, @fd, 'w', {:external_encoding => 'utf-8', :encoding => 'iso-8859-1:iso-8859-1'})
      @io.external_encoding.to_s.should == 'UTF-8'
    end

    it "ingores the :encoding option when the :internal_encoding option is present" do
      @io = IO.send(@method, @fd, 'w', {:internal_encoding => 'iso-8859-1', :encoding => 'iso-8859-1:iso-8859-1'})
      @io.internal_encoding.to_s.should == 'ISO-8859-1'
    end

    it "uses the encoding specified via the :mode option hash" do
      @io = IO.send(@method, @fd, {:mode => 'w:utf-8:ISO-8859-1'})
      @io.external_encoding.to_s.should == 'UTF-8'
      @io.internal_encoding.to_s.should == 'ISO-8859-1'
    end

    it "ignores the :internal_encoding option when the same as the external encoding" do
      @io = IO.send(@method, @fd, 'w', {:external_encoding => 'utf-8', :internal_encoding => 'utf-8'})
      @io.external_encoding.to_s.should == 'UTF-8'
      @io.internal_encoding.to_s.should == ''
    end
  end
end

# This group of specs may ONLY contain specs that do not actually create
# an IO instance from the file descriptor returned by #new_fd helper.
describe :io_new_errors, :shared => true do
  before :each do
    @name = tmp("io_new.txt")
    @fd = new_fd @name
  end

  after :each do
    IO.new(@fd, "w").close
    rm_r @name
  end

  it "raises an Errno::EBADF if the file descriptor is not valid" do
    lambda { IO.send(@method, -1, "w") }.should raise_error(Errno::EBADF)
  end

  it "raises an IOError if passed a closed stream" do
    lambda { IO.send(@method, IOSpecs.closed_io.fileno, 'w') }.should raise_error(IOError)
  end

  it "raises an Errno::EINVAL if the new mode is not compatible with the descriptor's current mode" do
    lambda { IO.send(@method, @fd, "r") }.should raise_error(Errno::EINVAL)
  end
end
