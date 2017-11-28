require 'digest/sha2'

def digest_files(files)
  digest = Digest::SHA256.new

  files.each do |name|
    File.open name, "r" do |file|
      while chunk = file.read(4096)
        digest << chunk
      end
    end
  end

  digest.hexdigest
end
