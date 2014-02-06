require './rakelib/digest_files'

def build_ruby
  bin = RbConfig::CONFIG["RUBY_INSTALL_NAME"] || RbConfig::CONFIG["ruby_install_name"]
  bin += (RbConfig::CONFIG['EXEEXT'] || RbConfig::CONFIG['exeext'] || '')
  File.join(RbConfig::CONFIG['bindir'], bin)
end

def build_signature_name
  ".build_signature"
end

def load_build_signature
  File.read(build_signature_name).chomp if File.exist? build_signature_name
end

def write_build_signature
  File.open build_signature_name, "w" do |f|
    f.puts build_signature
  end
end

def build_ruby_name
  ".build_ruby"
end

def write_build_ruby
  File.open build_ruby_name, "w" do |f|
    f.puts build_ruby
  end
end

def build_signature
  write_build_ruby
  digest_files Dir[*%W[configure config.rb #{build_ruby} Rakefile rakelib/*]]
end

def verify_build_signature
  load_build_signature == build_signature
end
