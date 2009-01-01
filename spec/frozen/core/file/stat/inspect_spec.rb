require File.dirname(__FILE__) + '/../../../spec_helper'

describe "File::Stat#inspect" do

  before :each do
    @file = tmp('i_exist')
    File.open(@file,'w'){|f| f.write 'rubinius'}
  end

  after :each do
    File.delete(@file) if File.exist?(@file)
  end
  
  it "produces a nicely formatted description of a File::Stat object" do
    st = File.stat(@file)  
    #p "#<File::Stat dev=0x#{st.dev.to_s(16)}, ino=#{st.ino}, mode=#{sprintf("%06s", st.mode.to_s(8))}, nlink=#{st.nlink}, uid=#{st.uid}, gid=#{st.gid}, rdev=0x#{st.rdev.to_s(16)}, size=#{st.size}, blksize=#{st.blksize}, blocks=#{st.blocks}, atime=#{st.atime}, mtime=#{st.mtime}, ctime=#{st.ctime}>"
    st.inspect.should == "#<File::Stat dev=0x#{st.dev.to_s(16)}, ino=#{st.ino}, mode=#{sprintf("%07d", st.mode.to_s(8).to_i)}, nlink=#{st.nlink}, uid=#{st.uid}, gid=#{st.gid}, rdev=0x#{st.rdev.to_s(16)}, size=#{st.size}, blksize=#{st.blksize}, blocks=#{st.blocks}, atime=#{st.atime}, mtime=#{st.mtime}, ctime=#{st.ctime}>"
    
  end


end
