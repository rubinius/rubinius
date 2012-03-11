# Here's the namespace to serve some optional website tasks.
namespace :web do

  # Here we work with doc translations.
  namespace :translations do

    # A helper library.
    require "rakelib/transcheck"

    # This is the 'leader' version: all is translated from here.
    LEADER = "web/doc/en"

    # Here we MANUALLY list all official doc translations
    TRANS  = [
      'de',
      'es',
      'fr',
      'it',
      'ja',
      'pl',
      'pt-br',
      'ru'
    ]


    desc "Check web documentation translations for existence and mtime consistency."
    task :check => LEADER do
      tc = Transcheck.new(LEADER, TRANS, true)
      tc.check
      tc.status
    end
  end

end
