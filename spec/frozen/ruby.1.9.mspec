class MSpecScript
  # 1.9 does not have these libraries
  obsolete_libs = %w[
    cgi-lib date2 eregex finalize ftools generator getopts importenv
    jcode mailread parsearg parsedate ping readbytes rubyunit runit
    soap wsdl xsd
    Win32API
  ]

  # on 1.9 these implementation was re-written.
  replaced_libs = %w[ cgi csv test/unit complex rational dl ]

  # replaced but compatible enough to share specs with 1.8
  enough_compatible_libs = %w[ cgi csv complex rational ]

  libs_to_ignore = obsolete_libs + replaced_libs - enough_compatible_libs
  spec_dirs = File.dirname(__FILE__) + '/1.8/library/*'
  ruby_1_8_compatible_libraries = Dir.glob(spec_dirs).select{|path|
    path.gsub!(File::PATH_SEPARATOR, '/')
    %r[.*/1.8/library/([^/]*)] =~ path and !libs_to_ignore.include?($1)
  }

  # An ordered list of the directories containing specs to run
  set :files, [
    '1.8/language',
    '1.8/core',
    ruby_1_8_compatible_libraries,
    '1.9',
  ].flatten

  # The default implementation to run the specs.
  # TODO: this needs to be more sophisticated since the
  # executable is not consistently named.
  set :target, 'ruby19'
end
