shared :sha256_update do |cmd|
  
  describe "Digest::SHA256##{cmd}" do
    it 'can update' do
      cur_digest = Digest::SHA256.new
      cur_digest.send cmd, SHA256Constants::Contents
      cur_digest.digest.should == SHA256Constants::Digest
    end

  end

end
