require 'digest/sha1'

def digest_files(files)
  digest = Digest::SHA1.new

  files.each do |name|
    File.open name, "r" do |file|
      while chunk = file.read(1024)
        digest << chunk
      end
    end
  end

  digest.hexdigest
end
