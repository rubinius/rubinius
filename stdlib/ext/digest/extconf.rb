# $RoughId: extconf.rb,v 1.6 2001/07/13 15:38:27 knu Exp $
# $Id: extconf.rb 11708 2007-02-12 23:01:19Z shyouhei $

require "mkmf"

$INSTALLFILES = {
  "digest.h" => "$(RUBYARCHDIR)"
}

create_makefile("digest")
