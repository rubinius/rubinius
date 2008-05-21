shared :sha384_update do |cmd|
  
  describe "Digest::SHA384##{cmd}" do
    it 'can update' do
      cur_digest = Digest::SHA384.new
      cur_digest.send cmd, SHA384Constants::Contents
      cur_digest.digest.should == SHA384Constants::Digest
    end

  end

end
