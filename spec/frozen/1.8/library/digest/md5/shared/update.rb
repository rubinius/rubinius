shared :md5_update do |cmd|
  
  describe "Digest::MD5##{cmd}" do
    it 'can update' do
      cur_digest = Digest::MD5.new
      cur_digest.send cmd, MD5Constants::Contents
      cur_digest.digest.should == MD5Constants::Digest
    end

  end

end
