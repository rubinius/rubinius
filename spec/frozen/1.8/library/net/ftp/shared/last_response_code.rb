describe :net_ftp_last_response_code, :shared => true do
  before(:each) do
    @socket = mock("Socket")

    @ftp = Net::FTP.new
    @ftp.instance_variable_set(:@sock, @socket)
  end

  it "returns the response code for the last response" do
    responses = [ "200 Command okay.", "212 Directory status." ]
    @socket.should_receive(:readline).and_return(*responses)

    @ftp.send(:getresp)
    @ftp.send(@method).should == "200"

    @ftp.send(:getresp)
    @ftp.send(@method).should == "212"
  end
end
