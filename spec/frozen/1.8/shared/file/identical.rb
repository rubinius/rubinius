shared :file_identical do |cmd, klass, name|
  describe "#{name || "#{klass}.#{cmd}"}" do
    before :each do    
      @file1 = 'test.txt'
      @file2 = 'test2.txt'
      @file3 = 'test.lnk'
      File.delete(@file3) if File.exists?(@file3)

      File.open(@file1,"w+") { |f| f.puts "file1" }
      File.open(@file2,"w+") { |f| f.puts "file2" }
      File.link(@file1, @file3)
    end

    after :each do
      File.unlink(@file3)
      File.delete(@file1) if File.exists?(@file1)    
      File.delete(@file2) if File.exists?(@file2)     
      @file1 = nil
      @file1 = nil
      @file1 = nil
    end

    it "return a Boolean class" do 
      klass.send(cmd, @file1, @file2).class.should == FalseClass
      klass.send(cmd, @file1, @file1).class.should == TrueClass
    end

    it "return true if they are identical" do
      klass.send(cmd, @file1, @file1).should == true
      klass.send(cmd, @file1, @file2).should == false
      klass.send(cmd, @file1, @file3).should == true
    end

    it "raises an ArgumentError if not passed two arguments" do
      lambda { klass.send(cmd, @file1, @file2, @file3) }.should raise_error(ArgumentError)
      lambda { klass.send(cmd, @file1) }.should raise_error(ArgumentError)
    end

    it "raises a TypeError if not passed String types" do
      lambda { klass.send(cmd, 1,1) }.should raise_error(TypeError)
    end

    it "identical? should return true if both named files are identical" do
      begin
        file = tmp('i_exist')
        file2 = tmp('i_exist_too')
        File.open(file,'w'){|f| f.write 'rubinius'}
        File.open(file2,'w'){|f| f.write 'ruby'}
        klass.send(cmd, file,file).should == true
        klass.send(cmd, file,file2).should == false
      ensure
        File.delete(file) rescue nil
        File.delete(file2) rescue nil
      end
    end  
  end
end
