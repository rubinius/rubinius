require File.dirname(__FILE__) + '/../../spec_helper'

describe "File.open" do 
  before :each do         
    @file = 'test.txt'    
    File.delete(@file) if File.exists?(@file)
    File.delete("fake") if File.exists?("fake")
    @fh = nil
    @fd = nil
    @flags = File::CREAT | File::TRUNC | File::WRONLY
    File.open(@file, "w"){} # touch
  end
  
  after :each do
    File.delete("fake") rescue nil
    @fh.delete if @fh  rescue nil
    @fh.close if @fh rescue nil
    @fh    = nil
    @fd    = nil
    @file  = nil
    @flags = nil
  end
  
  it "open the file (basic case)" do 
    @fh = File.open(@file) 
    @fh.class.should == File
    File.exists?(@file).should == true
  end

  it "open file when call with a block (basic case)" do
    File.open(@file){ |fh| @fd = fh.fileno }
    should_raise(SystemCallError){ File.open(@fd) } # Should be closed by block
    File.exists?(@file).should == true
  end

  it "open with mode string" do
    @fh = File.open(@file, 'w') 
    @fh.class.should == File
    File.exists?(@file).should == true
  end

  it "open a file with mode string and block" do
    File.open(@file, 'w'){ |fh| @fd = fh.fileno }
    should_raise(SystemCallError){ File.open(@fd) }
    File.exists?(@file).should == true
  end

  it "open a file with mode num" do 
    @fh = File.open(@file, @flags)
    @fh.class.should == File
    File.exists?(@file).should == true
  end

  it "open a file with mode num and block" do
    File.open(@file, 'w'){ |fh| @fd = fh.fileno }
    should_raise(SystemCallError){ File.open(@fd) }
    File.exists?(@file).should == true
  end

  # For this test we delete the file first to reset the perms
  it "open the file when call with mode, num and permissions" do
    File.delete(@file)
    @fh = File.open(@file, @flags, 0755)
    File.stat(@file).mode.to_s(8).should == "100755"
    @fh.class.should == File
    File.exists?(@file).should == true
  end

  # For this test we delete the file first to reset the perms
  it "open the flie when call with mode, num, permissions and block" do
    File.delete(@file)
    File.open(@file, @flags, 0755){ |fh| @fd = fh.fileno }
    should_raise(SystemCallError){ File.open(@fd) }
    File.stat(@file).mode.to_s(8).should == "100755"
    File.exists?(@file).should == true
  end

  it "open the file when call with fd" do
    @fh = File.open(@file)
    @fh = File.open(@fh.fileno) 
    @fh.class.should == File
    File.exists?(@file).should == true
  end

  # Note that this test invalidates the file descriptor in @fh. That's
  # handled in the teardown via the 'rescue nil'.
  #
  it "open a file with a file descriptor d and a block" do 
    @fh = File.open(@file) 
    File.open(@fh.fileno){ |fh| @fd = fh.fileno }
    should_raise(SystemCallError){ File.open(@fd) }
    @fh.class.should == File
    File.exists?(@file).should == true
  end  
    
  it "open a file that no exists when use File::WRONLY mode" do 
    should_raise(Errno::ENOENT) { File.open("fake", File::WRONLY)  }
  end  
  
  it "open a file that no exists when use File::RDONLY mode" do 
    should_raise(Errno::ENOENT) { File.open("fake", File::RDONLY)  }
  end    
  
  it "open a file that no exists when use 'r' mode" do 
    should_raise(Errno::ENOENT) { File.open("fake", 'r')  }
  end  
  
  it "open a file that no exists when use File::EXCL mode" do 
    should_raise(Errno::ENOENT) { File.open("fake", File::EXCL)  }
  end  
  
  it "open a file that no exists when use File::NONBLOCK mode" do 
    should_raise(Errno::ENOENT) { File.open("fake", File::NONBLOCK)  }
  end  
  
  it "open a file that no exists when use File::TRUNC mode" do 
    should_raise(Errno::ENOENT) { File.open("fake", File::TRUNC)  }
  end  
  
  it "open a file that no exists when use File::NOCTTY mode" do 
    should_raise(Errno::ENOENT) { File.open("fake", File::NOCTTY)  }
  end  
  
  it "open a file that no exists when use File::CREAT mode" do 
    @fh = File.open("fake", File::CREAT)      
    @fh.class.should == File
    File.exists?(@file).should == true
  end  
  
  it "open a file that no exists when use 'a' mode" do 
    @fh = File.open("fake", 'a')      
    @fh.class.should == File
    File.exists?(@file).should == true
  end  
     
  it "open a file that no exists when use 'w' mode" do 
    @fh = File.open("fake", 'w')  
    @fh.class.should == File
    File.exists?(@file).should == true
  end  
  
  # Check the grants associated to the differents open modes combinations.   
  it "raise an ArgumentError exception when call with an unknown mode" do 
    should_raise(ArgumentError){File.open(@file, "q")  }
  end
  
  it "can read in a block when call open with RDONLY mode" do 
    File.open(@file, File::RDONLY) do |f| 
      f.gets.should == nil
    end
  end
  
  it "can read in a block when call open with 'r' mode" do 
    File.open(@file, "r") do |f| 
      f.gets.should == nil
    end
  end
  
  it "raise an IO exception when write in a block opened with RDONLY mode" do 
    File.open(@file, File::RDONLY) do |f| 
      should_raise(IOError) { f.puts "writing ..." }
    end
  end
  
  it "raise an IO exception when write in a block opened with 'r' mode" do 
    File.open(@file, "r") do |f| 
      should_raise(IOError) { f.puts "writing ..." }
    end
  end
  
  it "can't write in a block when call open with File::WRONLY||File::RDONLY mode" do  
    File.open(@file, File::WRONLY|File::RDONLY ) do |f|  
      f.puts("writing").should == nil 
    end 
  end  
  
  it "can't read in a block when call open with File::WRONLY||File::RDONLY mode" do 
    should_raise(IOError) do
      File.open(@file, File::WRONLY|File::RDONLY ) do |f| 
        f.gets.should == nil       
      end
    end
  end    
  
  it "can write in a block when call open with WRONLY mode" do 
    File.open(@file, File::WRONLY) do |f| 
      f.puts("writing").should == nil
    end
  end
  
  it "can write in a block when call open with 'w' mode" do 
    File.open(@file, "w") do |f| 
      f.puts("writing").should == nil
    end
  end
  
  it "raise an IO exception when read in a block opened with WRONLY mode" do 
    File.open(@file, File::WRONLY) do |f| 
      should_raise(IOError) { f.gets  }
    end
  end
  
  it "raise an IO exception when read in a block opened with 'w' mode" do 
    File.open(@file, "w") do |f| 
      should_raise(IOError) { f.gets   }
    end
  end
  
  it "raise an IO exception when read in a block opened with 'a' mode" do 
    File.open(@file, "a") do |f| 
      should_raise(IOError) { f.gets  }
    end
  end
  
  it "raise an IO exception when read in a block opened with 'a' mode" do 
    File.open(@file, "a") do |f|        
      f.puts("writing").should == nil      
      should_raise(IOError) { f.gets }
    end
  end  
  
  it "raise an IO exception when read in a block opened with 'a' mode" do 
    File.open(@file, File::WRONLY|File::APPEND ) do |f| 
      should_raise(IOError) { f.gets  }
    end
  end
  
  it "raise an IO exception when read in a block opened with File::WRONLY|File::APPEND mode" do 
    File.open(@file, File::WRONLY|File::APPEND ) do |f|        
      f.puts("writing").should == nil  
    end
  end
  
  it "raise an IO exception when read in a block opened with File::RDONLY|File::APPEND mode" do 
    should_raise(Errno::EINVAL) do 
      File.open(@file, File::RDONLY|File::APPEND ) do |f|        
        f.puts("writing")  
      end
    end
  end
  
  it "can read and write in a block when call open with RDWR mode" do 
    File.open(@file, File::RDWR) do |f| 
      f.gets.should == nil      
      f.puts("writing").should == nil
      f.rewind
      f.gets.should == "writing\n"
    end
  end  
  
  it "can't read in a block when call open with File::EXCL mode" do 
    should_raise(IOError) do
      File.open(@file, File::EXCL) do |f|  
        f.puts("writing").should == nil 
      end
    end
  end
  
  it "can read in a block when call open with File::EXCL mode" do  
    File.open(@file, File::EXCL) do |f|  
      f.gets.should == nil      
    end 
  end    
    
  it "can read and write in a block when call open with File::RDWR|File::EXCL mode" do 
    File.open(@file, File::RDWR|File::EXCL) do |f| 
      f.gets.should == nil      
      f.puts("writing").should == nil
      f.rewind
      f.gets.should == "writing\n"
    end     
  end
  
  it "raise an Errorno::EEXIST if the file exists when open with File::CREAT|File::EXCL" do 
    should_raise(Errno::EEXIST) do
      File.open(@file, File::CREAT|File::EXCL) do |f|  
        f.puts("writing")
      end
    end
  end
 
  it "create a new file when use File::WRONLY|File::APPEND mode" do 
    @fh = File.open(@file, File::WRONLY|File::APPEND) 
    @fh.class.should == File
    File.exists?(@file).should == true
  end  
  
  it "open a file when use File::WRONLY|File::APPEND mode" do 
    File.open(@file, File::WRONLY) do |f|
      f.puts("hello file")
    end    
    File.open(@file, File::RDWR|File::APPEND) do |f|
      f.puts("bye file") 
      f.rewind
      f.gets().should == "hello file\n"
      f.gets().should == "bye file\n"
      f.gets().should == nil
    end     
  end  
  
  it "raise an Errorno::EEXIST if the file exists when open with File::RDONLY|File::APPEND" do 
    should_raise( Errno::EINVAL) do
      File.open(@file, File::RDONLY|File::APPEND) do |f|  
        f.puts("writing").should == nil 
      end
    end
  end
  
  it "create a new file when use File::TRUNC mode" do 
    # create and write in the file
    File.open(@file, File::RDWR) do |f|
      f.puts "hello file" 
    end    
    # Truncate the file    
    @fh = File.new(@file, File::TRUNC)   
    @fh.gets.should == nil
  end 
    
  
  it "can't read in a block when call open with File::TRUNC mode" do  
    File.open(@file, File::TRUNC) do |f|  
      f.gets  
    end 
  end
    
  it "open a file when use File::WRONLY|File::TRUNC mode" do 
    File.open(@file, File::WRONLY|File::TRUNC) 
    @fh.class.should == NilClass
    File.exists?(@file).should == true
  end
  
  it "can't write in a block when call open with File::TRUNC mode" do 
    should_raise(IOError) do
      File.open(@file, File::TRUNC) do |f|  
        f.puts("writing")
      end
    end
  end  
      
  it "raise an Errorno::EEXIST if the file exists when open with File::RDONLY|File::TRUNC" do 
    should_raise(IOError) do
      File.open(@file, File::RDONLY|File::TRUNC) do |f|  
        f.puts("writing").should == nil 
      end
    end
  end
  
  it "should throw Errno::EACCES when opening non-permitted file" do
    @fh = File.open(@file, "w")
    @fh.chmod(000)
    should_raise(Errno::EACCES) do
      File.open(@file)
    end
  end
   
  specify "expected errors " do
    should_raise(TypeError){ File.open(true) }
    should_raise(TypeError){ File.open(false) }
    should_raise(TypeError){ File.open(nil) }
    should_raise(SystemCallError){ File.open(-1) } # kind_of ?
    should_raise(ArgumentError){ File.open(@file, File::CREAT, 0755, 'test') }
  end
end
