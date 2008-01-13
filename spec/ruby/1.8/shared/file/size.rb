shared :file_size do |cmd, klass, name|
  describe "#{name || klass}.#{cmd}" do
    before :each do
      @file = "i_dont_exist"
      File.delete(@file) if File.exists?(@file)
      File.size?(@file).should == nil
      @file = '/tmp/i_exist'
      File.open(@file,'w') { |f| f.write 'rubinius' }
    end

    after :each do
      File.delete(@file) if File.exist?(@file)
    end

    it "returns nil if file_name doesn't exist or has zero size, the size of the file otherwise" do
      klass.send(cmd, @file).should == 8
    end
  end
end
