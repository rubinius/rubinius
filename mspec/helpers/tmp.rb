# The #tmp method provides a similar functionality
# to that of Dir.tmpdir. This helper can be overridden
# by different implementations to provide a more useful
# behavior if needed.
#
# Usage in a spec:
#
#   File.open(tmp("tags.txt"), "w") { |f| f.puts "" }

class Object
  def tmp(name)
    unless @spec_temp_directory
      [ "/tmp", ENV["TMPDIR"], ENV["TMP"],
        ENV["TEMP"], ENV["USERPROFILE"] ].each do |dir|
        if dir and File.directory?(dir) and File.writable?(dir)
          @spec_temp_directory = File.expand_path dir
          break
        end
      end
    end

    File.join @spec_temp_directory, name
  end
end
