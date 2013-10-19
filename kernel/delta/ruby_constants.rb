# Ruby constants
#

RUBY_ENGINE       = "rbx"
RUBY_VERSION      = Rubinius::RUBY_VERSION
RUBY_PATCHLEVEL   = 0
RUBY_PLATFORM     = "#{Rubinius::CPU}-#{Rubinius::OS}"
RUBY_RELEASE_DATE = Rubinius::RELEASE_DATE.dup
RUBY_COPYRIGHT    = "rubinius - Copyright (C) 2006-2011 Evan Phoenix"
RUBY_REVISION     = 0
# Must be last, it reads the above constants
RUBY_DESCRIPTION  = Rubinius.version

