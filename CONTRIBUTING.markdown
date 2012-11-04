# Contributing

We want to start off by saying **thank you** for using Rubinius.

This project is a labor of love, and we appreciate all of the users that catch 
bugs, make performance improvements, and help with documentation.

Every contribution is meaningful, so thank you for participating.

That being said, here are a few guidelines that we ask you to follow so we can
successfully address your issue.

## Submitting Issues

Please include the following:

* The Rubinius version (`rbx -v`)

* Your OS (`uname -a`) RVM/rbenv/chruby/etc version or the commit hash from git
if you're building off of a clone

* Stack trace (preferably as a gist, since they're easier to read) If you can
add a failing spec, that's great!

* Please include the simplest possible reproduction you can. This last point is
vital to fixing issues.

## Running Specs

MSpec provides several different scripts to run the specs under different
conditions. The default behavior is to simply run all the specs.

If you invoke the following command, it will run all the Ruby Array specs in
the default language mode, which should be 1.8 unless you configured a different
mode to be the default:

  `bin/mspec core/array`

To Run the specs in 1.9 mode, add the `-tx19` option:

  `bin/mspec -tx19 core/array`

The `-t` option specifies which Ruby implementation to run the specs under.

The default in Rubinius is to run them with Rubinius, so `-tx` is implied.

You can easily run with another target by giving the name of an executable on
your PATH or the full path to an executable.

Since the specs are intended to show the behavior of MRI, if you are writing
new specs you need to run them under the current stable releases of MRI 1.8,
1.9, and 2.0. For example, if you have `ruby1.8.7` and `ruby1.9.3` executables
on your PATH, you can do the following:

  `bin/mspec -t ruby1.8.7 core/array`

  `bin/mspec -t ruby1.9.3 core/array`

Finally, if you are running bin/mspec in the Rubinius source directory, the
location of the RubySpecs are known (spec/ruby/), so you can use the full path
or the shortened version core/array above.

## Fixing a bug

* Fork the repository; Create a topic branch; Include a spec, if appropriate.

* Pull requests that need a spec but are submitted without one will be delayed
until one is written. The spec should be in a separate commit.

* Please follow the
[Coding Style Guide](http://rubini.us/doc/en/contributing/style-guide).

* ALWAYS RUN THE FULL SPEC SUITE! `rake` will run the VM specs, plus all 1.8 and
1.9 specs.

* Please add a detailed commit message.
  [Here](https://github.com/rubinius/rubinius/commit/1f9ddd1) is a fantastic
example by @ryoqun.

  The preference is for a (max) 50 character summary as line one, a blank line,
then any number of lines, no longer than 80 characters.

* Send in that pull request!

* Follow up with us on the ticket if we haven't merged or commented in a few
days. We strive to address issues in a reasonable time. If we miss yours,
please remind us.

## Performance patches

We love these!

* Include benchmarks before and after the change. Please include your hardware
specs, namely CPU speed, # of cores, speed of hard drive (if SSD, then SSD is
fine) and amount of RAM.

* ALWAYS RUN THE FULL SPEC SUITE! rake will ensure you didn't accidentally
break anything.

For more details on how to contribute, please see
[Contributing to Rubinius](http://rubini.us/2011/10/18/contributing-to-rubinius/)
Also, feel free to hang out in #rubinius on Freenode.

Again, thank you!
