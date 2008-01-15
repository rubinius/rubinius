shared :sha256_length do |cmd|
  
  describe "Digest::SHA256##{cmd}" do

    it 'returns the length of the digest' do
      cur_digest = Digest::SHA256.new
      cur_digest.send(cmd).should == SHA256Constants::BlankDigest.size
      cur_digest << SHA256Constants::Contents
      cur_digest.send(cmd).should == SHA256Constants::Digest.size
    end

  end

end
