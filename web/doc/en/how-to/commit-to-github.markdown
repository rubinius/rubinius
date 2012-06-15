---
layout: doc_en
title: How-To - Commit Changes to Github
previous: Translate Documentation
previous_url: how-to/translate-documentation
next: Appendix A - Glossary
next_url: appendix-a-glossary
---

The Rubinius Project does a majority of its work on the master
branch. The goal is to keep master "clean" so that it always builds
a working binary and provides a snapshot of the latest fixes
and enhancements.

### Committers With Read/Write Access to Rubinius Repository

We recommend that committers who have read/write access to the
repository do their work on a branch in their local repository.
As the changes stabilize, they should be committed in two steps.
The first step should commit the spec that highlights the
behavior under construction while the second commit adds the
behavior and allows the spec to pass.

After committing to the local repository's branch, the commit
should be merged back to master and pushed to github. To avoid
superfluous git merge messages, we ask that the committer first
rebase the master branch prior to the merge.

1. git branch name-of-fix-branch
2. git checkout name-of-fix-branch
3. <write the spec>
4. <write code to pass the spec>
5. git add <list of spec files>
6. git commit
7. git add <list of code files>
8. git commit
9. git checkout master
10. git pull --rebase
11. git checkout name-of-fix-branch
12. git rebase master
13. git checkout master
14. git merge name-of-fix-branch
15. git push origin master

Steps 9 through 15 can be automated via a script to save on all
of that typing.

### Committers With Read-only Access to Rubinius Repository

Please read:

  *  [How-To Write a Ticket](/doc/en/how-to/write-a-ticket)

