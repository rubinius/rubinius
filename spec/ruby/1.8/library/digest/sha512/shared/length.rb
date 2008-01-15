shared :sha512_length do |cmd|
  
  describe "Digest::SHA512##{cmd}" do

    it 'returns the length of the digest' do
      cur_digest = Digest::SHA512.new
      cur_digest.send(cmd).should == SHA512Constants::BlankDigest.size
      cur_digest << SHA512Constants::Contents
      cur_digest.send(cmd).should == SHA512Constants::Digest.size
    end

  end

end
