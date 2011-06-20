require File.expand_path('../../../../spec_helper', __FILE__)
require File.expand_path('../../../../shared/file/socket', __FILE__)
require File.expand_path('../fixtures/classes', __FILE__)
require 'socket'

describe "File::Stat#socket?" do
  it_behaves_like :file_socket, :socket?, FileStat
end

describe "File::Stat#socket?" do
  it "returns false if the file is not a socket" do
    filename = tmp("i_exist")
    touch(filename)

    st = File.stat(filename)
    st.socket?.should == false

    rm_r filename
  end

  it "returns true if the file is a socket" do
    filename = tmp("i_am_a_socket")
    server = UNIXServer.new filename

    st = File.stat(filename)
    st.socket?.should == true

    rm_r filename
  end

end
