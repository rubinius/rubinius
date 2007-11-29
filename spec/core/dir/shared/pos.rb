shared :dir_pos do |cmd|
  describe "Dir##{cmd}" do
    before :each do
      @dir = Dir.open DirSpecs.mock_dir
    end

    after :each do
      @dir.close rescue nil
    end

    platform :darwin do
      version '1.8'..'1.8.5' do
        it "gives the current dir position" do
          @dir.pos.should == 1
          @dir.tell.should == 2
          @dir.pos.should == 3
          @dir.tell.should == 4
        end
      end
      
      version '1.8.6' do
        it "gives the current dir position" do
          @dir.pos.should == 0
          @dir.tell.should == 0
          @dir.pos.should == 0
          @dir.tell.should == 0
        end
      end
    end

    platform :not, :darwin do
      it "gives the current dir position" do
        @dir.pos.should == 0
        @dir.tell.should == 0
        @dir.pos.should == 0
        @dir.tell.should == 0
      end
    end

    it "seeks to a certain position and returns a location number" do
      pos = @dir.pos
      a   = @dir.read
      b   = @dir.read
      ret = @dir.pos = pos
      c   = @dir.read
  
      a.should_not == b
      b.should_not == c
      c.should     == a

      ret.should == pos
    end
  end
end
