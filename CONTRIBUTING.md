## Contributing To Rubinius

We want to start off by saying thank you for using Rubinius. This project is a
labor of love, and we appreciate all of the users that catch bugs, make
performance improvements, and help with documentation. Every contribution is
meaningful, so thank you for participating. That being said, here are a few
guidelines that we ask you to follow so we can successfully address your issue.

### Version Managers

We can *not* help you with any issues that might be caused by a Ruby version
manager. In the event of any issues please try building from source instead to
see if the issue is caused by Rubinius itself or your version manager of choice.

Issues involving version managers will be closed if they can either not be
reproduced when building from source, or when the original report makes no
mention about the author having tried building from source.

Note that this only applies to the installation procedure. Problems with the
runtime can of course still be reported, even when using a version manager.

### Rubinius Versions

Rubinius releases quite often, at least more often than most other
implementations. As such we ask users to try out the latest version prior to
reporting an issue. This ensures we don't have to start digging through the
code, only to find out the problem has already been resolved in a more recent
release.

### Running Specs

MSpec provides several different scripts to run the specs under different
conditions. The default behavior is to simply run all the specs. If you invoke
the following command, it will run all the Ruby Array specs:

    $ bin/mspec core/array

The `-t` option specifies which Ruby implementation to run the specs under. The
default in Rubinius is to run them with Rubinius, so `-tx` is implied. You can
easily run with another target by giving the name of an executable on your PATH
or the full path to an executable. Since the specs are intended to show the
behavior of MRI, if you are writing new specs you need to run them under the
current stable release of MRI 2.0. For example, if you have a `ruby2.0.0`
executable on your `PATH`, you can do the following:

    $ bin/mspec -t ruby2.0.0 core/array

Finally, if you are running `bin/mspec` in the Rubinius source directory, the
location of the RubySpecs are known (`spec/ruby/`), so you can use the full path
or the shortened version `core/array` above.

### Fixing A Bug

* Fork the repo, create a topic branch, and include a spec, if appropriate.
  Pull requests that need a spec but are submitted without one will be delayed
until one is written. The spec should be in a separate commit.
* Please follow the [Coding Style
  Guide](http://rubini.us/doc/en/contributing/style-guide)
* **Always run the full spec suite**. `rake` will run the VM specs plus
  RubySpec.
* Please add a detailed commit message. Here is a [fantastic
  example](https://github.com/rubinius/rubinius/commit/1f9ddd1) by @ryoqun .
The preference is for a (max) 50 character summary as line one, a blank line,
then any number of lines, no longer than 80 characters.
* Send in that pull request!
* Follow up with us on the ticket if we haven't merged or commented in a few
  days. We strive to address issues in a reasonable time. If we miss yours,
please remind us.
* When unsure about the changes, associated specs or other topics, please ask!
  We're more than eager to help.

### Writing Specs

A lot of this is already covered in [How To Write A
Spec](http://rubini.us/doc/en/how-to/write-a-spec/) but the basic gist of it is
as following:

* Spec descriptions (for both `describe` and `it` blocks) should be written in
  natural English.
* Specs should include only the bare minimum that is required to test
  something.
* Setup code that is re-used between examples should be placed in a `before()`
  block
* When unsure, please ask!

### Performance Patches

We love these!

* Include benchmarks before and after the change. Please include your hardware
  specs, namely CPU speed, # of cores, speed of hard drive (if SSD, then SSD is
fine) and amount of RAM.
* **Always run the full spec suite**. `rake` will ensure you didn't
  accidentally break anything.

For more details on how to contribute, please see [Contributing to
Rubinius](http://rubini.us/2011/10/18/contributing-to-rubinius/).

For more help, visit the [Rubinius Gitter chat
room](https://gitter.im/rubinius/rubinius)

Again, thank you!
