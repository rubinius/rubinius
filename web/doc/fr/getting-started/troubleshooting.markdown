---
layout: doc_fr
title: Dépannage
previous: Exécuter Rubinius
previous_url: getting-started/running-rubinius
next: Contribuer
next_url: contributing
---

Cette section rassemble les erreurs que vous pouvez rencontrer lors de la compilation,
de l'installation ou de l'exécution de Rubinius, avec des suggestions de solutions.

Pour chaque erreur, la première étape est de s'assurer que vous avez une copie de travail
à jour et propre de Rubinius. Avant d'aller plus loin, pensez à exécuter les étapes suivantes :

    $ git co master
    $ git reset --hard
    $ git pull
    $ rake distclean
    $ rake


### Rubinius ne trouve pas le répertoire de `runtime`

  Après la compilation ou l'installation, l'erreur suivant apparaît lors d'une
  tentative d'exécution de Rubinius :

    ERROR: unable to find runtime directory

    Rubinius was configured to find the runtime directory at:

      /Users/brian/devel/rubinius/runtime

    but that directory does not exist.

    Set the environment variable RBX_RUNTIME to the location
    of the directory with the compiled Rubinius kernel files.

    You may have configured Rubinius for a different install
    directory but you have not run 'rake install' yet.

#### Solution:

  Si vous avez configuré Rubinius avec un `--prefix`, exécutez `rake install`.

  Si vous avez configuré Rubinius avec un `--prefix` et renommé le répertoire
  d'installation après avoir installé Rubinius, reconfigurez Rubinius et réinstallez-le.

  Si vous avez renommé le répertoir des sources après avoir compilé Rubinius, reconfigurez
  et recompilez-le.

  De manière générale, ne renommez pas le répertoire de compilation ou d'installation après
  avoir compilé ou installé Rubinius.


### Rubinius provoque une erreur de segmentation sur FreeBSD

  Sur FreeBSD, jusqu'à la version 8.1 stable incluse, existe un problème avec execinfo qui
  provoque une erreur de segmentation lorsque Rubinius se charge.

#### Solution:

  Désactivez execinfo en configurant Rubinius :

    ./configure --without-execinfo

### Installer Rubinius avec ruby-build

  Si vous souhaitez installer Rubinius en utilisant ruby-build, dans certains cas, vous
  pouvez rencontrer des problèmes lors de compilation de Rubinius.
  
#### Solution

  Pour palier à ce problème, le mieux est d'installer Rubinius en compilant les sources
  vous même :
  
    $ git clone https://github.com/rubinius/rubinius
    $ cd rubinius
    $ ./configure --prefix=/chemin/vers/rubinius
    $ rake install

  Vous n'êtes pas obligé de spécifier l'option `--prefix`. Cette option vous
  permet simplement de placer Rubinius où vous le shouhaitez sur votre machine.
  Il est conseillé de placer Rubinius dans un dossier contenu dans votre `PATH`.
