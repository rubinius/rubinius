module ARGFSpecs

  # create a temporary fixture file in tmp directory based on
  # the fixture file passed as an argument
  def self.fixture_file(*filename)
    fixture_file_path = File.join(File.dirname(__FILE__), filename)
    tmp_fixture_file_path = tmp(filename)
    File.open(tmp_fixture_file_path, 'w') { |fh| fh.write(File.read(fixture_file_path))}
    tmp_fixture_file_path
  end

  def self.fixture_file_delete(*filenames)
    filenames.each { |fn| File.delete(fn) if File.exists?(fn) }
  end

  def self.file_args(*args)
    files = args.collect do |filename|
      # if STDIN or abslute path then return as is
      # else append the fixture path to the file
      if filename == '-' || filename[0..0] == '/'
        filename
      else
        self.fixture_file(filename)
      end
    end
    ARGV.concat(files)
  end

  def self.ruby(cmd_args)
    begin
      ruby = self.rubybin
      code_file = tmp('spec_code.rb')
      File.open(code_file, 'w') { |fh| fh.write(cmd_args[:code])}
      cmd = ([ruby] + cmd_args[:options] + [code_file] + cmd_args[:args]).join(' ')
      f = IO.popen(cmd, 'r+')
      yield(f)
    ensure
      # TODO: reactivate this code when the pIO#reopen methos is fixed (for now
      # it keeps raising an IO error saying the popen IO can not be reopened.
      # Generally speaking the current implementation of IO#reopen is unable
      # f.close unless f.nil? || f.closed?
      File.delete(code_file) if File.exists?(code_file)
    end
  end

  # TODO: put this into a helper in MSpec and require it in
  # spec_helper.rb for RSpec.
  #
  # got it from test/ruby/envutil.rb from Ruby 1.9.x trunk
  def self.rubybin
    unless ENV["RUBYOPT"]

    end
    if ruby = ENV["RUBY"]
      return ruby
    end
    ruby = "ruby"
    rubyexe = ruby+".exe"
    3.times do
      if File.exist? ruby and File.executable? ruby and !File.directory? ruby
        return File.expand_path(ruby)
      end
      if File.exist? rubyexe and File.executable? rubyexe
        return File.expand_path(rubyexe)
      end
      ruby = File.join("..", ruby)
    end
    begin
      require "rbconfig"
      File.join(
        RbConfig::CONFIG["bindir"], RbConfig::CONFIG["ruby_install_name"] + RbConfig::CONFIG["EXEEXT"]
      )
    rescue LoadError
      "ruby"
    end
  end

end