---
layout: doc_ja
title: Troubleshooting
previous: Running Rubinius
previous_url: getting-started/running-rubinius
next: Contributing
next_url: contributing
translated: true
---

あなたは、建物、または、インストール中に発生する可能性がある、次のエラー
推奨する解決方法と一緒にRubiniusのを実行している。

すべてのエラーは、最初のステップは、現在、クリーンなチェックアウトをれていること
を確認することですRubiniusの。先に進む前に、次の手順を実行することを検討：


    $ git co master
    $ git reset --hard
    $ git pull
    $ rake distclean
    $ rake


Error:

    ERROR: unable to find runtime directory

    Rubinius was configured to find the runtime directory at:

      /Users/brian/devel/rubinius/runtime

    but that directory does not exist.

    Set the environment variable RBX_RUNTIME to the location
    of the directory with the compiled Rubinius kernel files.

    You may have configured Rubinius for a different install
    directory but you have not run 'rake install' yet.

Solution:

  If you configured Rubinius with a `--prefix`, run rake install.

  If you configured Rubinius with a `--prefix` and renamed the install
  directory after installing Rubinius, re-configure Rubinius and reinstall it.

  If you renamed the source directory after building Rubinius, re-configure
  and rebuild it.

  In general, do not rename the source or build directory after building
  Rubinius.
