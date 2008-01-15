shared :sha512_update do |cmd|
  
  describe "Digest::SHA512##{cmd}" do
    it 'can update' do
      cur_digest = Digest::SHA512.new
      cur_digest.send cmd, SHA512Constants::Contents
      cur_digest.digest.should == SHA512Constants::Digest
    end

  end

end
