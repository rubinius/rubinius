shared :sha384_length do |cmd|
  
  describe "Digest::SHA384##{cmd}" do

    it 'returns the length of the digest' do
      cur_digest = Digest::SHA384.new
      cur_digest.send(cmd).should == SHA384Constants::BlankDigest.size
      cur_digest << SHA384Constants::Contents
      cur_digest.send(cmd).should == SHA384Constants::Digest.size
    end

  end

end
