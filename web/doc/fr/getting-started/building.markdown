---
layout: doc_fr
title: Compiler Rubinius
previous: Pré-requis
previous_url: getting-started/requirements
next: Exécuter Rubinius
next_url: getting-started/running-rubinius
---

Vous pouvez compiler et exécuter Rubinius à partir du répertoire des sources.
Vous n'avez pas besoin d'installer Rubinius pour l'exécuter.
Les instructions ci-dessous détaillent ces deux points, installer Rubinius
et l'exécuter à partir du répertoire des sources.

Rubinius utilise LLVM dans son compilateur JIT. Rubinius dépend d'une version
particulière de LLVM, et LLVM doit être compilé avec les options de RTTI C++
("run-time type information") activées.
Le script `configure` vérifiera automatiquement ces pré-requis en cherchant une
version installée de LLVM.
Si LLVM n'est pas installé ou que Rubinius échoue à l'édition de liens pour une
raison quelconque, passez `--skip-system` au script `configure` dans les
instructions suivantes.

### Récupérer les sources

Le code source de Rubinius est disponible sous la forme d'une archive et d'un projet sur Github.
Vous pouvez [télécharger l'archive
ici](https://github.com/rubinius/rubinius/tarball/master).

Pour utiliser Git :

  1. Changez de répertoire courant vers votre répertoire de développement
  2. `git clone git://github.com/rubinius/rubinius.git`


### Installer Rubinius

Si vous prévoyez d'utiliser Rubinius pour exécuter votre application, c'est
une bonne solution. Cependant, vous pouvez aussi exécuter Rubinius directement
à partir du répertoire des sources. Consultez la section suivante pour plus
de détails sur ce sujet.

Nous recommandons d'installer Rubinius à un emplacement qui ne nécessite pas `sudo` ou
des privilèges super-utilisateur. Pour installer Rubinius :

  1. `./configure --prefix=/path/to/install/dir`
  2. `rake install`
  3. Suivez les instructions pour ajouter le répertoire _bin_ de Rubinius à votre `PATH`


### Exécuter à partir du répertoire des sources

Si vous prévoyez de travailler sur Rubinius lui-même, vous devriez choisir cette option.

  1. `./configure`
  2. `rake`

Si vous souhaitez simplement évaluer Rubinius, suivez les instructions pour ajouter
le répertoire _bin_ de Rubinius à votre `PATH`.

Cependant, si vous développez Rubinius, vous ne devriez PAS ajouter le répertoire _bin_
à votre PATH car le système de construction de Rubinius va utiliser les liens
`ruby` et `rake` qui pointent vers l'exécutable Rubinius.
Rubinius nécessite un exécutable Ruby séparé pour s'amorcer lors de la phase de
compilation.

