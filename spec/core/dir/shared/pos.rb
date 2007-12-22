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
