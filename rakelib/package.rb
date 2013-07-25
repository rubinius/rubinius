
def write_md5_digest_file(filename)
  require 'digest/md5'

  digest_file = "#{filename}.md5"
  File.open(digest_file, "w") do |f|
    f.puts Digest::MD5.file(filename).hexdigest
  end

  puts "Computed MD5 to #{digest_file}"
end

def write_sha1_digest_file(filename)
  require 'digest/sha1'

  digest_file = "#{filename}.sha1"
  File.open(digest_file, "w") do |f|
    f.puts Digest::SHA1.file(filename).hexdigest
  end

  puts "Computed SHA1 to #{digest_file}"
end

def decode_release_label(label)
  case label
  when /^n\d+$/
    "nightly"
  when /^w\d+$/
    "weekly"
  when /^m\d+$/
    "monthly"
  when /^$/
    "final"
  else
    label
  end
end

class RubiniusPackager
  attr_writer :ruby_version, :release, :prefix, :root, :bin, :config, :archive, :package

  def initialize(options={})
    @ruby_version = options[:ruby_version]
    @release_type = options[:release]
    @release_date = options[:release_date]
    @prefix = options[:prefix]
    @root = options[:root]
    @bin = options[:bin]
    @config = options[:config]
    @archive = options[:archive]
    @package = options[:package]
  end

  # eg "18", "1.8", "18,19", "1.8,1.9"
  def ruby_version
    (@ruby_version || "18").gsub(/\s|\./, "")
  end

  def single_version?
    not ruby_version.index(",")
  end

  def rbx_version
    BUILD_CONFIG[:version].match(/^(\d+\.\d+\.\d+)/)[0]
  end

  # "nightly", "weekly", "monthly". no value indicates standard release
  def release_type
    @release_type
  end

  def release_label
    BUILD_CONFIG[:release] unless BUILD_CONFIG[:release].empty?
  end

  def release_date
    @release_date || DateTime.now.strftime("%F")
  end

  # passed verbatim to --prefix
  def prefix
    default = "/rubinius/#{rbx_version}"
    default += ".#{release_label}" if release_label
    default += "-#{ruby_version}" if single_version?
    @prefix || default
  end

  # root directory of the build
  def root
    if BUILD_CONFIG[:stagingdir]
      default = BUILD_CONFIG[:stagingdir][0...-BUILD_CONFIG[:prefixdir].size]
    else
      default = BUILD_CONFIG[:sourcedir]
    end
    @root || default
  end

  # path for a binary symlink
  def bin
    @bin
  end

  # any configure options
  def config
    config = ["--prefix=#{prefix} --preserve-prefix"]
    config << "--enable-version=#{ruby_version}" if single_version?
    config << "--release=#{release_type}" if release_type
    config << "--release-date=#{release_date}"
    config << @config
    config.join(" ")
  end

  # "zip", "tar.gz", "tar.bz2"
  def archive
    @archive || "tar.bz2"
  end

  # name of the final package file minus #archive
  def package
    default = "rubinius-#{rbx_version}"
    case release_type
    when /^rc\d+/
      default += "-#{release_type}"
    when "final", nil
      # stable release, no release label
    else
      default += "-#{release_type}#{date_stamp}"
    end
    default += "-d#{ruby_version}" if single_version?
    @package || default
  end

  def date_stamp
    Time.now.strftime("%Y%m%d")
  end

  def create_archive(package_name)
    name = "#{BUILD_CONFIG[:sourcedir]}/#{package_name}"

    Dir.chdir root do
      case archive
      when "zip"
        sh "zip --symlinks -r #{name} *"
      when "tar.gz"
        sh "tar -c -f - * | gzip > #{name}"
      when "tar.bz2"
        sh "tar -c -f - * | bzip2 -9 > #{name}"
      else
        raise RuntimeError, "unknown archive format: #{archive}"
      end
    end
  end

  def build
    sh "rm -rf #{BUILD_CONFIG[:sourcedir]}/staging"

    package_name = package + "." + archive
    sh "rm -rf #{package_name}*"

    ENV["RELEASE"] = "1"
    sh "./configure #{config}"
    load_configuration

    sh "rake -q clean; rake -q build"
    sh "strip -S #{BUILD_CONFIG[:stagingdir]}#{BUILD_CONFIG[:bindir]}/#{BUILD_CONFIG[:program_name]}"

    if bin
      sh "mkdir -p #{root}#{File.dirname(bin)}"

      bin = "#{prefix}#{BUILD_CONFIG[:bindir]}"
      bin_link = "#{root}#{bin}"

      sh "ln -sf #{bin} #{bin_link}"
    end

    create_archive package_name
    write_md5_digest_file package_name
    write_sha1_digest_file package_name
  rescue Object => e
    # Some rake versions swallow the backtrace, so we do it explicitly.
    STDERR.puts e.message, e.backtrace
  end
end
