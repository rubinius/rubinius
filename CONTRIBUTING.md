# Thanks!

We want to start off by saying thank you for using Rubinius. This project is a labor of love, and we appreciate all of the users that catch bugs, make performance improvements, and help with documentation. Every contribution is meaningful, so thank you for participating. That being said, here are a few guidelines that we ask you to follow so we can successfully address your issue.

## Submitting Issues

Please include the following:

1. The Rubinius version (`rbx -v`)
1. Your OS (`uname -a`)
1. RVM/rbenv/chruby/etc version or the commit hash from git if you're building off of a clone
1. Stack trace (preferably as a gist, since they're easier to read)
1. If you can add a failing spec, that's great!

## Fixing a bug

1. Fork the repo
1. Create a topic branch
1. Include a spec! Pull requests without a spec will be delayed. The spec should be in a separate commit, and it should include an mspec tag.
1. Please follow the [Coding Style Guide](http://rubini.us/doc/en/contributing/style-guide)
1. ALWAYS RUN THE FULL SPEC SUITE! `rake` will run the VM specs, plus all `1.8` and `1.9` specs.
1. Please add a detailed commit message. Here is a [fantastic example](https://github.com/rubinius/rubinius/commit/1f9ddd10c10386f4e12f9a52f41ff77731061612) by @ryoqun.
1. Send in that pull request!
1. Follow up with us on the ticket if we haven't merged or commented in a few days. We strive to address issues in a reasonable time. If we miss yours, please remind us.

## Performance patches

We love these!

1. Include benchmarks before and after the change.
1. Please include your hardware specs, namely CPU speed, # of cores, speed of hard drive (if SSD, then SSD is fine) and amount of RAM.
1. ALWAYS RUN THE FULL SPEC SUITE! `rake` will ensure you didn't accidentally break anything.

For more details on how to contribute, please see [Contributing to Rubinius](http://rubini.us/2011/10/18/contributing-to-rubinius/). Feel free to hang out in #rubinius on Freenode.

Again, thank you!
