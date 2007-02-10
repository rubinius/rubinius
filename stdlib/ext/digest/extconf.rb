# $RoughId: extconf.rb,v 1.6 2001/07/13 15:38:27 knu Exp $
# $Id: extconf.rb 11122 2006-10-11 05:15:15Z knu $

require "mkmf"

$INSTALLFILES = {
  "digest.h" => "$(RUBYARCHDIR)"
}

create_makefile("digest")
