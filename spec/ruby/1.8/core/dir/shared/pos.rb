shared :dir_pos do |cmd|
  describe "Dir##{cmd}" do
    before :each do
      @dir = Dir.open DirSpecs.mock_dir
    end

    after :each do
      @dir.close rescue nil
    end

    it "returns an Integer representing the current position in the directory" do
      @dir.send(cmd).should be_kind_of(Integer)
      @dir.send(cmd).should be_kind_of(Integer)
      @dir.send(cmd).should be_kind_of(Integer)
    end

    it "returns a different Integer if moved from previous position" do
      a = @dir.send(cmd)
      @dir.read
      b = @dir.send(cmd)

      a.should be_kind_of(Integer)
      b.should be_kind_of(Integer)

      a.should_not == b
    end
  end
end

shared :dir_pos_set do |cmd|
  describe "Dir##{cmd}" do
    before(:each) do
      @dir = Dir.open DirSpecs.mock_dir
    end

    after(:each) do
      @dir.close
    end
    
    # NOTE: #seek/#pos= to a position not returned by #tell/#pos is undefined
    # and should not be spec'd.

    it "moves the read position to a previously obtained position" do
      pos = @dir.pos
      a   = @dir.read
      b   = @dir.read
      ret = @dir.send cmd, pos
      c   = @dir.read

      a.should_not == b
      b.should_not == c
      c.should     == a
    end
  end
end
