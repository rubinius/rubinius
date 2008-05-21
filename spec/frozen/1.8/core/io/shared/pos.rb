shared :io_pos do |cmd|
  describe "IO##{cmd}" do
    before :each do
      @fname = 'test.txt'
      File.open @fname, 'w' do |f| f.write "123" end
    end

    after :each do
      File.unlink @fname
    end

    it "gets the offset" do
      File.open @fname do |f|
        f.send(cmd).should == 0
        f.read 1
        f.send(cmd).should == 1
        f.read 2
        f.send(cmd).should == 3
      end
    end

    it "raises IOError on closed stream" do
      lambda { IOSpecs.closed_file.send(cmd) }.should raise_error(IOError)
    end

    it "resets #eof?" do
      open @fname do |io|
        io.read 1
        io.read 1
        io.send(cmd)
        io.eof?.should == false
      end
    end
  end
end
