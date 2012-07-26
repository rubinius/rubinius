
def write_md5_digest_file(filename)
  require 'digest/md5'
  digest = Digest::MD5.new

  File.open(filename, "rb") do |f|
    until f.eof?
      digest << f.read(1024)
    end
  end

  digest_file = "#{filename}.md5"
  File.open(digest_file, "w") do |f|
    f.puts digest.hexdigest
  end

  puts "Computed MD5 to #{digest_file}"
end

def write_sha1_digest_file(filename)
  require 'digest/sha1'
  digest = Digest::SHA1.new

  File.open(filename, "rb") do |f|
    until f.eof?
      digest << f.read(1024)
    end
  end

  digest_file = "#{filename}.sha1"
  File.open(digest_file, "w") do |f|
    f.puts digest.hexdigest
  end

  puts "Computed SHA1 to #{digest_file}"
end
